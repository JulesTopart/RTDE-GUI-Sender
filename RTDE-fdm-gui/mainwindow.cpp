#include "mainwindow.h"
#include "stdafx.h"

#include "irtde.h"
#include "Program.h"
#include "console.h"

#include "loadingMsg.h"

#include <ur_rtde/dashboard_client.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    netTimer = new QTimer(this);

    connectSignal();
    loadSettings();

    ui.buttonConnect_rtde->setText("Connect");
    ui.status_rtde->setText("Offline");
    ui.status_rtde->setStyleSheet("color : red;");

    actionEnableConsoleTrigerred();
    actionEnableTemperatureTrigerred();

    ui.actionRun->setEnabled(false);
    ui.actionPause->setEnabled(false);
    ui.actionStop->setEnabled(false);
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

    connect(ui.actionEnableConsole, &QAction::triggered, this, &MainWindow::actionEnableConsoleTrigerred);
    connect(ui.actionEnableTemperature, &QAction::triggered, this, &MainWindow::actionEnableTemperatureTrigerred);

    connect(ui.startLineSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::spinBoxStartLineChanged);

    connect(netTimer, &QTimer::timeout, this, &MainWindow::updateGUI);
    netTimer->setInterval(1500);
    netTimer->start();

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

void MainWindow::updateGUI() {
    if(SharedData::state == State::RUNNING){
        while (SharedData::consoleQueue.size() > 0) {
            Console::printMessage(SharedData::consoleQueue.dequeue());
        }

        ui.labelCurrentLine->setText(QString::number(SharedData::currentLine));

        ui.progressBar->setValue(program.getProgress());

        //Past line to update
        do {
            if (SharedData::lineQueue.size() > 1) {
                LineState current = SharedData::lineQueue.front();

                if (current.line > CHUNK_SIZE * currentChunk + CHUNK_SIZE) {
                    currentChunk++;
                    if (currentChunk < chunks.size()) {
                        showChunk(currentChunk);
                    }
                }

                long lineToColor = current.line - currentChunk * CHUNK_SIZE;
                switch (current.lineState) {
                case State::RUNNING:
                    colorLine(lineToColor, 255, 125, 0);
                    break;

                case State::FINISHED:
                    colorLine(lineToColor, 21, 171, 0);
                    break;

                case State::FAILED:
                    colorLine(lineToColor, 255, 43, 0);
                    break;

                default:
                    //colorLine(lineToColor, 0, 0, 0);
                    break;
                }
            }
            if (SharedData::lineQueue.size() > 1) {
                SharedData::lineQueue.dequeue();
            }

        }while (SharedData::lineQueue.size() > 1);
    }
    else {
        if (!UrRobot::isConnected()) disconnect();
    }
}

void MainWindow::actionRunTriggered() {
    spinBoxStartLineChanged();
    if (SharedData::state != State::RUNNING) {

        long startLine = ui.startLineSpinBox->value();
        SharedData::firstLine = startLine;
        
        currentChunk = 0;
        showChunk(0);
        spinBoxStartLineChanged();

        if (program.isValid()) {
            if (program.hasError()) {
                Console::warn("Program has errors", "Program");
                int ret = QMessageBox::warning(this, "Attention",
                    "Le programme comporte peut-etre des erreurs (voir la console), \n"
                    "Etes vous sur de vouloir continuer ?",
                    QMessageBox::Apply | QMessageBox::Cancel,
                    QMessageBox::Apply);

                if (ret == QMessageBox::Apply) {
                    ui.actionRun->setEnabled(false);
                    ui.actionPause->setEnabled(true);
                    ui.actionStop->setEnabled(true);

                    

                    if (startLine >= program.size() || startLine < 0) {
                        Console::error("Invalid start line");
                        return;
                    }

                    //Ignored line to gray
                    for (int i(0); i < startLine; i++) {
                        colorLine(i, 80, 80, 80);
                    }

                    program.start(startLine);
                }
            }
            else {
                Console::log("Starting program", "Program");
                ui.actionRun->setEnabled(false);
                ui.actionPause->setEnabled(true);
                ui.actionStop->setEnabled(true);
                program.start(startLine);
            }
        }
        else {
            Console::error("Invalid program", "Program");
        }


    }
    else if(SharedData::state == State::PAUSED){
        program.resume();

        ui.actionRun->setEnabled(false);
        ui.actionPause->setEnabled(true);
        ui.actionStop->setEnabled(true);
    }

    qApp->processEvents(QEventLoop::ExcludeSocketNotifiers, 10);
}

void MainWindow::actionPauseTriggered() {
    program.pause();

    if (UrRobot::isConnected()) {
        UrRobot::setAnalogOutputVoltage(0, 0);
        UrRobot::setStandardDigitalOut(0, false);
    }

    Console::log("Paused.");
    ui.actionRun->setEnabled(true);
    ui.actionPause->setEnabled(false);
    ui.actionStop->setEnabled(true);
}

void MainWindow::actionStopTriggered() {
    program.stop();

    if (UrRobot::isConnected()) {
        UrRobot::setAnalogOutputVoltage(0, 0);
        UrRobot::setStandardDigitalOut(0, false);
    }

    Console::log("Stopped.");
    
    ui.actionRun->setEnabled(true);
    ui.actionPause->setEnabled(false);
    ui.actionStop->setEnabled(false);
}

void MainWindow::actionOpenTriggered() {


    if (SharedData::state == State::RUNNING) {
        actionStopTriggered();
    }
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Script"), "%HOMEPATH%", tr("Scripts (*.urscript *.txt)"));

    if (filePath == "") return;
    LoadingMsg msgBox(this);
    msgBox.setLabel("Reading program...");
    msgBox.show();

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        long size = file.size();  //when file does open.
        long cursor = 0;

        while(!file.atEnd()) {
            QString buf = file.readLine();
            script.push_back(buf);
            cursor += buf.size();
            msgBox.updateProgress(float(float(cursor )/ float(size)) * 100.0);
            qApp->processEvents(QEventLoop::ExcludeSocketNotifiers, 10);
        }
        msgBox.updateProgress(100);

    }

    msgBox.setLabel("Buildings chunks...");
    msgBox.updateProgress(0);
    TextChunk tc;
    for (int i(0); i < script.size(); i++) {
        qApp->processEvents(QEventLoop::ExcludeSocketNotifiers, 10);
        if (tc.size() >= CHUNK_SIZE) {
            chunks.push_back(tc);
            tc.clear();
        }
        tc.push_back(script[i]);
        msgBox.updateProgress(float(float(i) / float(script.size())) * 100.0);
    }
    if (!tc.isEmpty()) chunks.push_back(tc);


    msgBox.close();
    showChunk(0);
    spinBoxStartLineChanged();
    compile();
}

void MainWindow::compile() {

    long lineCount = script.size();

    if (lineCount > 0) {
        Console::log("Compiling...", "Interpreter");
        QApplication::setOverrideCursor(Qt::WaitCursor);
        program.setScript(script);
        program.compile();
        QApplication::restoreOverrideCursor();

        if (program.isValid()) {
            if (program.hasError()) {
                Console::warn("Compilation finished with errors !", "Interpreter");
            }
            else {
                Console::success("Compilation successful !", "Interpreter");
            }

            if (UrRobot::isConnected())
                ui.actionRun->setEnabled(true);
            else
                ui.actionRun->setEnabled(false);

            ui.actionPause->setEnabled(false);
            ui.actionStop->setEnabled(false);

        }
        else {
            Console::error("Compilation failed !", "Interpreter");
        }
    }
    
}

void MainWindow::showChunk(long chunkNumber) {
    ui.scriptText->clear();
    for (int i(0); i < chunks[chunkNumber].size(); i++) {
        ui.scriptText->append(chunks[chunkNumber][i].trimmed());
    }

    long begin = chunkNumber * CHUNK_SIZE;
    generateLines(chunkNumber * CHUNK_SIZE, begin + chunks[chunkNumber].size());
}

void MainWindow::colorLine(long n, int r, int g , int b){
    QTextCursor tc(ui.scriptText->document()->findBlockByLineNumber(n));
    QTextBlockFormat blockFormat = ui.scriptText->document()->findBlockByLineNumber(n).blockFormat();
    blockFormat.setBackground(QColor(r,g,b,255));
    tc.setBlockFormat(blockFormat);
}

void MainWindow::generateLines(long start, long end) {
    QString result;
    for (long i = start; i < end; i++) result.append(QString::number(i) + "\n");
    ui.lineText->setText(result);
}


void MainWindow::buttonConnectRTDEPressed() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!UrRobot::isConnected()) {
        Console::log("Connecting... Please wait.");
        connectRobot();
    }else {
        Console::log("Disconnecting... Please wait.");
        disconnectRobot();
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::resetRobot() {
    if (UrRobot::isConnected()) {
        try{
            disconnectRobot();
            connectRobot();
        }
        catch (const std::exception&)
        {
            Console::error("error catched");
        }
    }
}

void MainWindow::connectRobot() {
    if (!UrRobot::isConnected()) {
        currentRTDEIP = ui.comboBoxIP_rtde->currentText();
        UrRobot::connect(currentRTDEIP.toStdString());
        if (UrRobot::isConnected()) {
            ui.buttonConnect_rtde->setText("Disconnect");
            ui.status_rtde->setText("Online");
            ui.status_rtde->setStyleSheet("color : green;");

            if(!program.isEmpty()) ui.actionRun->setEnabled(true);

            saveSettings();
        }else {
            ui.buttonConnect_rtde->setText("Connect");
            ui.status_rtde->setText("Offline");
            ui.status_rtde->setStyleSheet("color : red;");

            ui.actionRun->setEnabled(false);
        }
    }
}

void MainWindow::disconnectRobot() {
    if (UrRobot::isConnected()) {
        UrRobot::disconnect();
        currentRTDEIP = ui.comboBoxIP_rtde->currentText();
        ui.buttonConnect_rtde->setText("Connect");
        ui.status_rtde->setText("Offline");
        ui.status_rtde->setStyleSheet("color : red;");

        ui.actionRun->setEnabled(false);
    }
}


void MainWindow::buttonSendRTDEPressed() {
    Console::log("Connecting to dashboard...");
    DashboardClient dash(currentRTDEIP.toStdString());
    dash.connect();

    if (dash.isConnected()) {
        Console::log("Connected. Sending : " + ui.comboBoxCmd_rtde->currentText());
        dash.send(ui.comboBoxCmd_rtde->currentText().toStdString());
    }else
        Console::log("Connection failed");
}


void MainWindow::actionEnableTemperatureTrigerred() {
    if (ui.actionEnableTemperature->isChecked()) ui.groupBox_temp->setVisible(true);
    else ui.groupBox_temp->setVisible(false);

}

void MainWindow::actionEnableConsoleTrigerred() {
    if (ui.actionEnableConsole->isChecked()) ui.groupBox_console->setVisible(true);
    else ui.groupBox_console->setVisible(false);

}


void MainWindow::buttonConnectTemperaturePressed() {

}

void MainWindow::buttonSendTemperaturePressed() {

}

void MainWindow::spinBoxStartLineChanged() {
    if (script.size() > 0) {
        long startLine = SharedData::firstLine = ui.startLineSpinBox->value();

        if (int(float(startLine) / float(CHUNK_SIZE)) != currentChunk) {
            currentChunk = (startLine / CHUNK_SIZE);
            if (currentChunk > chunks.size()) currentChunk = chunks.size() - 1;
            showChunk(currentChunk);
        }
    }
}


