#pragma once
#include "interpreter.h"
#include <QVector>
#include <QQueue>
#include <QThread>

enum class State {
	COMPILING, RUNNING, FINISHED, FAILED, PAUSED, STOPPED
};

class Worker;

class Program {
public:
	Program(){
		_hasError = false;
		_valid = 0;
	};

	void start(long start = 0);
	void resume();
	void stop();
	void pause();

	long	getCurrentLine();
	int		getProgress();
	State	getCurrentState();

	void compile();

//Inlines
	inline QVector<Operation>	getProgram()	{ return program; }
	inline QVector<QString>		getText()		{ return script; }
	inline bool					isValid()		{ return _valid;  }
	inline bool					hasError()		{ return _hasError; }
	inline int					size()			{ return program.size(); };

	inline bool					isEmpty() { return script.size() <= 0; }
	inline void					setScript(QVector<QString> src) { script = src; }

private:

	QVector<QString> script;
	QVector<Operation> program;
	State programState = State::STOPPED;
	
	bool _valid;
	bool _hasError;


	QThread* thread = nullptr;
	Worker* worker = nullptr;

};

struct LineState {
	long line;
	State lineState;
};

//------------ Executer -----------------

struct SharedData {

	static State state; //1 Command success, 2 Comment, 0 Not running, -1 error.
	
	static long currentLine;
	static long firstLine;

	static QQueue<Console::Message>  consoleQueue;
	static QQueue<LineState> lineQueue;

	static void log(QString msg);
	static void error(QString msg);
	static void warning(QString msg);


};

class Worker : public QObject {
	Q_OBJECT
public:
	Worker(QVector<Operation> prgm);
	~Worker(){};

	bool executeLine(Operation op);

public slots:
	void process();
signals:
	void finished();
private:
	QVector<Operation> program;

	// add your variables here
};
