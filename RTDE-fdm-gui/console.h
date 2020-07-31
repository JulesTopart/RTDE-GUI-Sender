#pragma once
#include "QtWidgets/QTextEdit"

enum WarnLevel{
	lINFO,
	lWARN,
	lERROR,
	lFATAL
};

class Console{
public:
	static void link(QTextEdit* el);

	static void log	(QString m, QString origin = "Global");
	static void warn	(QString m, QString origin = "Global");
	static void error	(QString m, QString origin = "Global");

	static void print(WarnLevel level, QString m, QString origin = "Global");
private:

	static QTextEdit* element;

	Console() {}
};

