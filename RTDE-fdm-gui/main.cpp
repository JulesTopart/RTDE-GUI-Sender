#include "mainwindow.h"
#include "stdafx.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    QFile File(":/MainWindow/qss/VisualScript.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    //app.setStyleSheet(StyleSheet);


    w.show();
    return app.exec();
}
