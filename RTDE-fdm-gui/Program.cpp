#include "Program.h"
#include "irtde.h"
#include "QMessageBox"
#include "loadingMsg.h"

void Program::compile() {
	LoadingMsg msgBox(nullptr);
	msgBox.setLabel("Compiling...");
	msgBox.updateProgress(0);
	msgBox.show();

	SharedData::state = State::COMPILING;
	for (int i(0); i < script.size(); i++) {
		QCoreApplication::processEvents();
		msgBox.updateProgress(float(i) / float(script.size())*100.0);
		Interpreter::compileLine(script[i].trimmed());
	}

	program = Interpreter::save();
	_valid = program.size() > 0;
	_hasError = Interpreter::errors() > 0;

	if (!isValid()) program.clear();
	msgBox.close();
}

void Program::start(long start) {
	SharedData::currentLine = start;
	SharedData::firstLine = start;

	thread = new QThread;
	worker = new Worker(getProgram());
	worker->moveToThread(thread);
	QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
	QObject::connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
}

void Program::resume() {
	SharedData::state = State::RUNNING;
}

void Program::stop() {
	SharedData::firstLine = 0;
	SharedData::currentLine = 0;

	SharedData::state = State::STOPPED;
}

void Program::pause() {
	SharedData::state = State::PAUSED;
}

long Program::getCurrentLine() {
	return SharedData::currentLine;
}

int Program::getProgress() {
	return (SharedData::currentLine / (program.size())-SharedData::firstLine) * 100;
}

State Program::getCurrentState() {
	return SharedData::state;
}


long SharedData::currentLine = 0;
long SharedData::firstLine = 0;
State SharedData::state = State::STOPPED;
QQueue<Console::Message> SharedData::consoleQueue;
QQueue<LineState> SharedData::lineQueue;

Worker::Worker(QVector<Operation> prgm) { // Constructor
	program = prgm;
}

void Worker::process() {
	SharedData::currentLine = SharedData::firstLine;
	SharedData::state = State::RUNNING;
	if (SharedData::currentLine >= program.size() || SharedData::currentLine < 0) return;
	while (SharedData::currentLine < program.size()) {
		SharedData::lineQueue.append({ SharedData::currentLine, State::RUNNING });
		if(SharedData::state == State::PAUSED) SharedData::lineQueue.append({ SharedData::currentLine, State::PAUSED });
		while (SharedData::state == State::PAUSED);
		if (SharedData::state == State::STOPPED) {
			SharedData::lineQueue.append({ SharedData::currentLine, State::FAILED });
			SharedData::currentLine = 0;
			return;
		}
		Operation op = program[SharedData::currentLine];
		if(executeLine(op)) SharedData::lineQueue.append({ SharedData::currentLine, State::FINISHED });
		else SharedData::lineQueue.append({ SharedData::currentLine, State::FAILED });
		SharedData::currentLine++;
	}
	emit finished();
}

bool Worker::executeLine(Operation op) {
	QString line = " at line " + QString::number(op.getLine()) + " -> ";
	if (!op.isEnded())
		if (op.getType() == FunctionType::COMMENT) {
			SharedData::log(line + op.getComment());
			op.setEnded();
			return true;
		}
		else if (op.getType() == FunctionType::COMMAND) {
			QString key = op.getCommand();
			Arguments args = op.getArguments();

			if (args.size() == 0) {
				SharedData::error("Null command argument : " + line);
				op.setEnded();
				return false;
			}

			if (key.size() == 0) {
				SharedData::error("Null command : " + line);
				op.setEnded();
				return false;
			}

			if (key == "setStandardDigitalOut") {
				int  pin = args[0].getInteger();
				bool state = args[1].getBoolean();

				SharedData::log(line + op.getCommand() + "(" + QString::number(pin) + "," + QString::number(state) + ")");

				UrRobot::setStandardDigitalOut(pin, state);
				op.setEnded();
				return true;
			}
			else if (key == "setAnalogOutputVoltage") {
				int  pin = args[0].getInteger();
				double value = args[1].getDouble();
				SharedData::log(line + op.getCommand() + "(" + QString::number(pin) + ", " + QString::number(value,'e',10) + ")");

				UrRobot::setAnalogOutputVoltage(pin, value);
				op.setEnded();
				return true;
			}
			else if (key == "moveJ") {
				std::vector<double> axis = args[0].getDoubleArray().toStdVector();

				double accel = args[1].getDouble();
				double speed = args[2].getDouble();

				SharedData::log(line + op.getCommand() + "(...)");

				UrRobot::moveJ(axis, speed, accel);
				op.setEnded();
				return true;
			}
			else if (key == "moveL") {
				std::vector<double> axis = args[0].getDoubleArray().toStdVector();

				double accel = args[1].getDouble();
				double speed = args[2].getDouble();

				SharedData::log(line + op.getCommand() + "(...)");

				UrRobot::moveL(axis, speed, accel);
				op.setEnded();
				return true;
			}
			else if (key == "servoc") {
				std::vector<double> axis = args[0].getDoubleArray().toStdVector();

				double accel = args[1].getDouble();
				double speed = args[2].getDouble();

				SharedData::log(line + op.getCommand() + "(...)");

				UrRobot::servoC(axis, speed, accel);
				op.setEnded();
				return true;
			}
			else {
				SharedData::error(line + "Invalid command : " + op.getCommand());
				op.setEnded();
				return false;
			}
		}
		else {
			SharedData::error(line + "Unknown error : " + op.getLine());
			op.setEnded();
			return false;
		}
}

void SharedData::log(QString msg) {
	consoleQueue.append({msg, Console::Level::INFO, "Program"});
}

void SharedData::error(QString msg) {
	consoleQueue.append({ msg, Console::Level::FATAL, "Program"});
}

void SharedData::warning(QString msg) {
	consoleQueue.append({ msg, Console::Level::WARNING, "Program"});
}