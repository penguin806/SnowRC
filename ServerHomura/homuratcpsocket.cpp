#include "homuratcpsocket.h"

HomuraTcpSocket::HomuraTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(ReadyReadReceivedData()));
    connect(this,SIGNAL(disconnected()),this,SLOT(Disconnected()));
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(SocketError(QAbstractSocket::SocketError)));
}

void HomuraTcpSocket::ReadyReadReceivedData()
{
    qint64 BytesAvailable = this->bytesAvailable();
    emit SocketReceivedData(this->socketDescriptor(), BytesAvailable, this->readAll());
}

void HomuraTcpSocket::Disconnected()
{
    emit SocketDisconnected(this->socketDescriptor());
}


void HomuraTcpSocket::SocketError(QAbstractSocket::SocketError)
{
    emit SocketErrorOccured(this->socketDescriptor(), this->errorString());
}
