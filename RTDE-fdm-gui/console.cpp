#include "console.h"

QTextEdit* Console::element = nullptr;

void Console::link(QTextEdit* el){
	Console::element = el;
}

void Console::log(QString m, QString origin){
	print(lINFO, m, origin);
}

void Console::warn(QString m, QString origin){
	print(lWARN, m, origin);
}

void Console::error(QString m, QString origin){
	print(lERROR, m, origin);
}

void Console::print(WarnLevel level, QString m, QString origin) {

	QColor errorColor = QColor(255,  27,  15);
	QColor warnColor  = QColor(255, 195,  15);
	QColor infoColor  = QColor(200, 200, 200);

	QString header = "[INFO]";

	switch (level)
	{
		case lERROR: 
			element->setTextColor(errorColor); 
			header = "[ERROR]";
			if (origin != "Global") header += " [" + origin + ']';
			m = header + ": " + m;
			break;
		case lWARN:	
			element->setTextColor(warnColor); 
			header = "[WARNING]";
			if (origin != "Global") header += " [" + origin + ']';
			m = header + ": " + m;
			break;
		case lINFO:	
			element->setTextColor(infoColor); 
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

	if (element != nullptr) element->append(m);
}
