#include "console.h"

QTextEdit* Console::element = nullptr;
bool Console::isLinked = false;

void Console::link(QTextEdit* el){
	Console::element = el;
	Console::isLinked = true;
}

void Console::log(QString m, QString origin){
	print(Level::INFO, m, origin);
}

void Console::warn(QString m, QString origin){
	print(Level::WARNING, m, origin);
}

void Console::error(QString m, QString origin){
	print(Level::FATAL, m, origin);
}

void Console::success(QString m, QString origin){
	print(Level::SUCCESS, m, origin);
}

void Console::printMessage(Message msg) {
	print(msg.level, msg.msg, msg.origin);
}

void Console::print(Level level, QString m, QString origin) {

	QColor errorColor = QColor(255,  50,  50);
	QColor warnColor  = QColor(255, 195,  15);
	QColor infoColor = QColor(200, 200, 200);
	QColor successColor = QColor(30, 200, 80);

	QString header = "[INFO]";

	try {
		if (element != nullptr) {
			switch (level)
			{
			case Level::FATAL:
				element->setTextColor(errorColor);
				header = "[ERROR]";
				if (origin != "Global") header += " [" + origin + ']';
				m = header + ": " + m;
				break;
			case Level::WARNING :
				element->setTextColor(warnColor);
				header = "[WARNING]";
				if (origin != "Global") header += " [" + origin + ']';
				m = header + ": " + m;
				break;
			case Level::INFO:
				element->setTextColor(infoColor);
				header = "[INFO]";
				if (origin != "Global") header += " [" + origin + ']';
				m = header + ": " + m;
				break;
			case Level::SUCCESS:
				element->setTextColor(successColor);
				header = "[INFO]";
				if (origin != "Global") header += " [" + origin + ']';
				m = header + ": " + m;
				break;
			default:
				element->setTextColor(infoColor);
				header = "[INFO]";
				if (origin != "Global") header += " [" + origin + ']';
				m = header + ": " + m;
				break;
			}

			element->append(m);
		}
	}
	catch (const std::exception&){

	}
	
}
