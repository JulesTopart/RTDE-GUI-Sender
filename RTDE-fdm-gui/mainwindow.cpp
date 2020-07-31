#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "stdafx.h"

#include "irtde.h"
#include "console.h"

#include <ur_rtde/dashboard_client.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connectSignal();
    loadSettings();
}

void MainWindow::closeEvent(QCloseEvent* event){

        saveSettings();
        event->accept();
}

void MainWindow::connectSignal() {
    Console::link(ui.console);

    connect(ui.actionOuvrir, &QAction::triggered, this, &MainWindow::actionOpenTriggered);
    connect(ui.actionRun, &QAction::triggered, this, &MainWindow::actionRunTriggered);
    connect(ui.actionPause, &QAction::triggered, this, &MainWindow::actionPauseTriggered);
    connect(ui.actionStop, &QAction::triggered, this, &MainWindow::actionStopTriggered);

    connect(ui.buttonConnect_rtde, &QPushButton::pressed, this, &MainWindow::buttonConnectRTDEPressed);
    connect(ui.buttonSend_rtde, &QPushButton::pressed, this, &MainWindow::buttonSendRTDEPressed);

    connect(ui.buttonConnect_temp, &QPushButton::pressed, this, &MainWindow::buttonConnectTemperaturePressed);
    connect(ui.buttonSend_temp, &QPushButton::pressed, this, &MainWindow::buttonSendTemperaturePressed);

    connect(ui.scriptText->verticalScrollBar(), &QScrollBar::valueChanged, ui.lineText->verticalScrollBar(), &QScrollBar::setValue);
    connect(ui.lineText->verticalScrollBar(), &QScrollBar::valueChanged, ui.scriptText->verticalScrollBar(), &QScrollBar::setValue);
}

void MainWindow::saveSettings() {
    QSettings settings("LaMachinerie", "UR-GCode");

    settings.setValue("rtde/ip", currentRTDEIP);

    settings.setValue("temperature/ip", currentTempIP);
    settings.setValue("temperature/port", currentTempPort);
}

void MainWindow::resetSettings() {
    QSettings settings("LaMachinerie", "UR-GCode");
    settings.setValue("rtde/ip", "192.168.1.1");

    settings.setValue("temperature/ip", "192.168.1.1");
    settings.setValue("temperature/port", "3000");

    loadSettings();
}

void MainWindow::loadSettings() {
    QSettings settings("LaMachinerie", "UR-GCode");
    QString ip = settings.value("rtde/ip").toString();

    QString Tport = settings.value("temperature/port").toString();
    QString Tip = settings.value("temperature/ip").toString();

    ui.comboBoxIP_rtde->clear();
    ui.comboBoxIP_rtde->addItem(ip);
    ui.comboBoxIP_rtde->setCurrentIndex(0);
    currentRTDEIP = ip;

    ui.comboBoxIP_temp->clear();
    ui.comboBoxIP_temp->addItem(Tip);
    ui.comboBoxIP_temp->setCurrentIndex(0);
    currentTempIP = Tip;

    ui.comboBoxPort_temp->clear();
    ui.comboBoxPort_temp->addItem(Tport);
    ui.comboBoxPort_temp->setCurrentIndex(0);
    currentTempPort = Tport;
}



void MainWindow::actionRunTriggered() {
    if (!running) {
        running = true;
        Interpreter intr;
        intr.setText(currentProgram);
        if (currentProgram.length() > 5 && robot.isConnected()) {

            QVector<Operation> program = intr.getProgram();
            Console::log("Program size :");
            Console::log(QString::number(program.size()));

            for each (Operation op in program) {
                colorLine(op.getLine());
                QFuture<void> future = QtConcurrent::run(this, &MainWindow::executeLine, op);

                while(!future.isFinished() || paused){
                    QCoreApplication::processEvents();
                    if (paused) {
                        future.pause();
                    }
                    else if (future.isPaused() && !paused) {
                        future.resume();
                    }

                    if (stopTrigerred) {
                        stopTrigerred = false;
                        running = false;
                        future.cancel();
                        return;
                    }
                }
            }
        }
    }
    else if (paused) {
        paused = false;
    }
}

void MainWindow::executeLine(Operation op) {
    if(!op.isEnded())
    if (op.getType() == FunctionType::COMMENT)
        Console::log(op.getComment());
    else {
        QString key = op.getCommand();
        Arguments args = op.getArguments();

        if (      key == "setStandardDigitalOut") {
            int  pin   = args[0].getInteger();
            bool state = args[1].getBoolean();

            robot.setStandardDigitalOut(pin, state);

        }else if (key == "setAnalogOutputCurrent") {
            int  pin = args[0].getInteger();
            double value = args[1].getDouble();

            robot.setAnalogOutputCurrent(pin, value);
        }else if (key == "moveJ") {
            std::vector<double> axis = args[0].getDoubleArray().toStdVector();

            double accel = args[1].getDouble();
            double speed = args[2].getDouble();
            
            robot.moveJ(axis, accel, speed);
        }else if (key == "moveL") {
            std::vector<double> axis = args[0].getDoubleArray().toStdVector();

            double accel = args[1].getDouble();
            double speed = args[2].getDouble();

            robot.moveL(axis, accel, speed);
        }

    }
    op.setEnded();
}

void MainWindow::colorLine(long n){

    if (n > 0) {
        QTextCursor tc(ui.scriptText->document()->findBlockByLineNumber(n-1));
        QTextBlockFormat blockFormat = ui.scriptText->document()->findBlockByLineNumber(n-1).blockFormat();
        blockFormat.setBackground(Qt::gray);
        tc.setBlockFormat(blockFormat);
    }

    QTextCursor tc(ui.scriptText->document()->findBlockByLineNumber(n));
    QTextBlockFormat blockFormat = ui.scriptText->document()->findBlockByLineNumber(n).blockFormat();
    blockFormat.setBackground(Qt::darkGray);
    tc.setBlockFormat(blockFormat);
}

void MainWindow::generateLines(long count) {
    QString result;
    for (long i = 0; i < count; i++) result.append(QString::number(i) + "\n");
    ui.lineText->setText(result);
}

void MainWindow::actionOpenTriggered() {
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Script"), "%HOMEPATH%", tr("Scripts (*.urscript *.txt)"));

    QFile fichier(filePath);
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    currentProgram = flux.readAll();
    ui.scriptText->setText(currentProgram);

    long lineCount = ui.scriptText->document()->blockCount();

    generateLines(lineCount);
}

void MainWindow::actionPauseTriggered() {
    if (running && !paused) {
        paused = true;
        Console::log("Paused.");
    }
}

void MainWindow::actionStopTriggered() {
    if (running && !stopTrigerred) {
        stopTrigerred = true;
        Console::log("Stopped.");
        ui.scriptText->document()->clear();
        ui.scriptText->append(currentProgram);
    }
}

void MainWindow::buttonConnectRTDEPressed() {
    currentRTDEIP = ui.comboBoxIP_rtde->currentText();
    Console::log("Connecting... Please wait.");
    robot.connect(currentRTDEIP.toStdString());
}

void MainWindow::buttonSendRTDEPressed() {
    Console::log("Connecting to dashboard...");
    DashboardClient dash(currentRTDEIP.toStdString());

    if (dash.isConnected()) {
        Console::log("Connected. Sending : " + ui.comboBoxCmd_rtde->currentText());
        dash.send(ui.comboBoxCmd_rtde->currentText().toStdString());
    }else
        Console::log("Connection failed");
}

void MainWindow::buttonConnectTemperaturePressed() {

}

void MainWindow::buttonSendTemperaturePressed() {

}



