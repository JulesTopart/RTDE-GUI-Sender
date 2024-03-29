#include "socket.h"
/*

Socket::Socket(QObject* parent) : QObject(parent)  {
}

void Socket::doConnect(){
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    qDebug() << "connecting...";

    // this is not blocking call
    socket->connectToHost("google.com", 80);

    // we need to wait...
    if (!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void Socket::connected(){
    qDebug() << "connected...";

    // Hey server, tell me about you.
    socket->write("HEAD / HTTP/1.0\r\n\r\n\r\n\r\n");
}


void Socket::disconnected(){
    qDebug() << "disconnected...";
}

void Socket::bytesWritten(qint64 bytes){
    qDebug() << bytes << " bytes written...";
}

void Socket::readyRead(){
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << socket->readAll();
}

*/