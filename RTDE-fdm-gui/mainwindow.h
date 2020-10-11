#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_mainwindow.h"
#include "console.h"
#include "irtde.h"
#include "Program.h"

constexpr auto CHUNK_SIZE = 100;

typedef QVector<QString> TextChunk;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

    void connectSignal();
    
    void updateGUI();

    void saveSettings();
    void resetSettings();
    void loadSettings();
    void compile();

    void showChunk(long lineNumber);
    void colorLine(long n, int r, int g, int b);
    void generateLines(long start, long end);

    void closeEvent(QCloseEvent* e);

    void resetRobot();
    void connectRobot();
    void disconnectRobot();

public slots :
    void actionOpenTriggered();
    void actionRunTriggered();
    void actionPauseTriggered();
    void actionStopTriggered();

    void actionEnableTemperatureTrigerred();
    void actionEnableConsoleTrigerred();

    void buttonConnectRTDEPressed();
    void buttonSendRTDEPressed();

    void buttonConnectTemperaturePressed();
    void buttonSendTemperaturePressed();

    void spinBoxStartLineChanged();

private:

    //Executer

    QTimer* netTimer;

    QVector<QString> script;
    Program program;

    long currentChunk = 0;
    QVector<TextChunk> chunks;

    //Network

    QString currentRTDEIP;

    QString currentTempPort;
    QString currentTempIP;

    Ui::MainWindowClass ui;

};
