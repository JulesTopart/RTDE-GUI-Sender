#pragma once
#include "QtWidgets/QTextEdit"


class Console{
public:
	enum class Level {
		INFO,
		SUCCESS,
		WARNING,
		FATAL
	};

	struct Message {
		QString msg;
		Level level;
		QString origin = "Global";
	};

	static void link(QTextEdit* el);

	static void log		(QString m, QString origin = "Global");
	static void warn	(QString m, QString origin = "Global");
	static void error	(QString m, QString origin = "Global");
	static void success	(QString m, QString origin = "Global");

	static void printMessage(Message msg);
	static void print(Level level, QString m, QString origin = "Global");

private:
	static bool isLinked;
	static QTextEdit* element;

	Console() {}
};

