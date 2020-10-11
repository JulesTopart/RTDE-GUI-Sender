#include "mainwindow.h"
#include "stdafx.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    qDebug() << QStyleFactory::keys();
    app.setStyle(QStyleFactory::create("Fusion"));


    w.show();
    return app.exec();
}
