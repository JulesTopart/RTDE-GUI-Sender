#pragma once
#include <QString>
#include <QVector>

#include "console.h"

enum class FunctionType {
	COMMAND,
	COMMENT
};

enum class ParamType {
	INTEGER,
	INTEGER_ARRAY,
	DOUBLE,
	DOUBLE_ARRAY,
	BOOLEAN
};


QVector<int>	atoiArray	(QString buf);
QVector<double> atofArray	(QString buf);
bool			atoboolean	(QString buffer);

uint			ignoreSpaces	(QString text);
QString			readToken		(QString buffer);
void			chop			(QString& buf, uint start, uint end = -1);

class Token {
public:
	Token(QString buf) { setText(buf); }

	void	setText(QString txt) 
		{ buffer = txt; parseText(); }

	void parseText();

	ParamType getType();

	int getInteger() 
		{ return atoi(buffer.toStdString().c_str()); }

	QVector<int> getIntArray() 
		{ return atoiArray(buffer); }

	double getDouble()
		{ return atof(buffer.toStdString().c_str()); }

	QVector<double> getDoubleArray()
		{ return atofArray(buffer); }

	bool getBoolean()
		{ return atoboolean(buffer); }


private :

	QString buffer;
};



typedef QVector<Token> Arguments;

class Operation {
public:
	Operation(QString txt, long number) {
		setText(txt);
		lineNumber = number;
	}

	inline void	setText(QString txt) { buffer = txt; parseText(); }
	inline void	setType(FunctionType t) { type	 = t;	}

	void parseText();
	void parseCommand();
	void parseArguments();

	inline long	getLine() { return lineNumber; }
	inline FunctionType	getType() { return type; }

	inline Arguments getArguments() { return content; }

	QString getCommand() { return command; }

	QString getComment() { return buffer; }

	bool isEnded() { return ended; }
	void setEnded() { ended = true; }

private:

	QString command;
	Arguments content;

	QString buffer;
	FunctionType type;

	long lineNumber = 0;

	bool ended = false;
};



class Interpreter{
public:
	Interpreter() {};

	inline void setText(QString txt) { buffer = txt.trimmed(); buffer.replace(" ", ""); parseText(); }

	QVector<Operation> getProgram() { return program; }



private:

	void parseText();



	QString buffer;
	QVector<Operation> program;
};

