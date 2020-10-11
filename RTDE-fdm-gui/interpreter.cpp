#include "irtde.h"
#include "interpreter.h"

#include <stdio.h>
#include <ctype.h>

#include <QStringList>

QString Interpreter::buffer = "";
long Interpreter::_errors = 0;
bool Interpreter::compiling = false;
long Interpreter::index = 0;
QVector<Operation> Interpreter::program;

void Interpreter::newProgram(){ 
	program.clear(); 
	_errors = 0;
	compiling = true;
	index = 0;
}


bool Interpreter::isCommandValid(QString key) {
	if(key == "setStandardDigitalOut"){
		return true;
	}
	else if (key == "setAnalogOutputVoltage") {
		return true;
	}
	else if (key == "moveJ") {
		return true;
	}
	else if (key == "moveL") {
		return true;
	}
	else if (key == "servoc") {
		return true;
	}
	else {
		_errors++;
		return false;
	}
}

QVector<Operation> Interpreter::save() {
	compiling = false;
	return program;
}

void Interpreter::compileLine(QString line){ 
	if (compiling == false) newProgram();
	program.push_back(Operation(line.simplified(), index++));
}

QVector<Operation> Interpreter::parseText(){
	QVector<Operation> program;
	QStringList lines = buffer.split("\n", QString::SkipEmptyParts);

	long index = 0;
	for each (QString line in lines){
		program.push_back(Operation(line.simplified(), index++));
	}

	return program;
}


void Operation::parseText() {
	chop(buffer, ignoreSpaces(buffer)); //Really useful ??
	//Console::log(buffer);
	if (buffer[0] == '#') {
		setType(FunctionType::COMMENT);
		return;
	}

	setType(FunctionType::COMMAND);
	if (!parseCommand()) {
		setType(FunctionType::ERROR_CODE);
	}
	buffer = buffer.mid(command.size());
	if (!parseArguments()) {
		setType(FunctionType::ERROR_CODE);
	}
}

bool Operation::parseArguments() {

	if (!buffer.endsWith(')')) buffer = buffer.left(buffer.indexOf(')')+1);
	if (buffer.startsWith('(') && buffer.endsWith(')')) {
		chop(buffer, 1, buffer.size()-2);
		while (buffer.size() > 0) {
			QString buf = readToken(buffer);
			chop(buffer, buf.size());
			if (buffer[0] == ",") chop(buffer,1);
			content.push_back(buf);
			//Console::log(buf);
		}
		
	}
	else {
		Console::error("Invalid argument : " + buffer, "Interpreter");
		return false;
	}
	return true;
}


void Token::parseText() {
	//Console::warn("Token text : " + buffer);
}

ParamType Token::getType() {
	if (buffer[0] == '[') {
		if (buffer.contains("."))
			return ParamType::DOUBLE_ARRAY;
		else
			return ParamType::INTEGER_ARRAY;
	}
	else if (buffer[0].isDigit() || buffer[0] == '-') {
		if (buffer.contains("."))
			return ParamType::DOUBLE;
		else
			return ParamType::INTEGER;
	}
	else if (buffer[0].isLetter()) {
		return ParamType::BOOLEAN;
	}
}


bool Operation::parseCommand() {
	QString name = "";
	uint index = 0;
	while (index < buffer.size()) {
		if (!buffer[index].isLetter()) {
			command = name;
			if (name.size() == 0) {
				return false;
			}
			else {
				if (!Interpreter::isCommandValid(name)) {
					Console::error("at line " + QString::number(lineNumber) + " -> Invalid command : " + name);
				}
				return true;
			}
		}
		name += buffer[index];
		index++;
	}
}


void chop(QString& buf, uint start, uint end) {
	buf = buf.mid(start, end);
}

QString readToken(QString buffer) {
	int tokenStart = 0;
	int tokenLenght = 0;

	buffer = buffer.mid(ignoreSpaces(buffer), buffer.size());

	if (buffer[0] == '[') {
		tokenLenght = buffer.indexOf(']') - tokenStart + 1;
		return buffer.mid(tokenStart, tokenLenght);
	}
	else if (buffer[0].isDigit() || buffer[0] == '-') {

		int tokenLenght = buffer.indexOf(',');
		if (tokenLenght <= 0)
			tokenLenght = buffer.indexOf(')');
		tokenLenght -= tokenStart;
		return buffer.mid(tokenStart, tokenLenght);

	}
	else if (buffer[0].isLetter()) {

		int tokenLenght = buffer.indexOf(',');
		if (tokenLenght <= 0)
			tokenLenght = buffer.indexOf(')');
		tokenLenght -= tokenStart;
		return buffer.mid(tokenStart, tokenLenght);
	}

	return "!";
}


uint ignoreSpaces(QString text) {
	uint index = 0;
	while (index < text.size()) {
		if (text[index] != " ")
			return index;
		index++;
	}
}

bool atoboolean(QString buffer) {
	if (buffer == "true") return true;
	return false;
}

QVector<int> atoiArray(QString buf) {
	QVector<int> result;

	QString fBuf;
	for (uint i = 0; i < buf.size(); i++) {
		if (buf[i] == "[");
		else if (buf[i] == ',') {
			result.push_back(atof(fBuf.toStdString().c_str()));
			fBuf.clear();
		}
		else if (buf[i] == ']') {
			result.push_back(atof(fBuf.toStdString().c_str()));
			return result;
		}
		else {
			fBuf.append(buf[i]);
		}
	}
}

QVector<double> atofArray(QString buf) {
	QVector<double> result;

	QString fBuf;
	for (uint i = 0; i < buf.size(); i++) {
		if (buf[i] == "[");
		else if (buf[i] == ',') {
			result.push_back(atof(fBuf.toStdString().c_str()));
			fBuf.clear();
		}
		else if (buf[i] == ']') {
			result.push_back(atof(fBuf.toStdString().c_str()));
			return result;
		}
		else {
			fBuf.append(buf[i]);
		}
	}
}


