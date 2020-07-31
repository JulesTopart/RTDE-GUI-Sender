#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "console.h"
#include "irtde.h"
#include "interpreter.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

    void connectSignal();
    
    void saveSettings();
    void resetSettings();
    void loadSettings();

    void colorLine(long count);
    void generateLines(long count);
    void executeLine(Operation op);

    void closeEvent(QCloseEvent* e);

public slots :
    void actionOpenTriggered();
    void actionRunTriggered();
    void actionPauseTriggered();
    void actionStopTriggered();
    
    void buttonConnectRTDEPressed();
    void buttonSendRTDEPressed();

    void buttonConnectTemperaturePressed();
    void buttonSendTemperaturePressed();

private:


    QString currentProgram;
    QString currentRTDEIP;

    QString currentTempPort;
    QString currentTempIP;

    bool running = false;
    bool paused = false;
    bool stopTrigerred = false;

    Ui::MainWindowClass ui;

    UrRobot robot;
};
