#include "homuratcpserver.h"
#include "homuratcpsocket.h"
#include "homuralogsystem.h"
#include <QDebug>

HomuraTcpServer::HomuraTcpServer(QObject *parent) : QTcpServer(parent)
{
    bool bResult;
    bResult = this->listen(QHostAddress::Any, this->ServerListenPort);
    if(!bResult)
    {
        HomuraLogSystem::Log(QtMsgType::QtCriticalMsg, "Server listen port " +
                             QString::number(this->ServerListenPort) +
                             " Fail, HomuraTcpServer will delete later!");
        this->close();
        this->deleteLater();
    }

    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Server listen port " +
                         QString::number(this->ServerListenPort) + " Success.");
}

void HomuraTcpServer::ParseReceivedData(QByteArray ReceivedData)
{

}

void HomuraTcpServer::SendDataToClients(bool bAllClient, QList<quint8> ClientIdListToSend, QByteArray DataToSend)
{
    if(true == bAllClient)
    {
        for(int i = 0; i < this->clientSocketList.size(); i++)
        {
            HomuraTcpSocket *item = this->clientSocketList.at(i);
            item->write(DataToSend);
            if(false == item->waitForBytesWritten())
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Failed to send " +
                                     QString::number(DataToSend.size()) +
                                     " bytes to client " + QString::number(i) + " ["
                                     + item->peerAddress().toString() + ":" +
                                     QString::number(item->peerPort()) + "].");
            }
            else
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Send " +
                                     QString::number(DataToSend.size()) +
                                     " bytes to client " + QString::number(i) + " ["
                                     + item->peerAddress().toString() + ":" +
                                     QString::number(item->peerPort()) + "] Success.");
            }
        }
    }
    else
    {
        for(int i = 0; i < ClientIdListToSend.size(); i++)
        {
            quint8 idToSend = ClientIdListToSend.at(i);
            if(idToSend < 0 || idToSend >= clientSocketList.size())
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Invalid client id, will not send to it...");
                continue;
            }
            else
            {
                HomuraTcpSocket *item = this->clientSocketList.at(i);
                item->write(DataToSend);
                if(false == item->waitForBytesWritten())
                {
                    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Failed to send " +
                                         QString::number(DataToSend.size()) +
                                         " bytes to client " + QString::number(i) + " ["
                                         + item->peerAddress().toString() + ":" +
                                         QString::number(item->peerPort()) + "].");
                }
                else
                {
                    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Send " +
                                         QString::number(DataToSend.size()) +
                                         " bytes to client " + QString::number(i) + " ["
                                         + item->peerAddress().toString() + ":" +
                                         QString::number(item->peerPort()) + "] Success.");
                }
            }
        }
    }
}

void HomuraTcpServer::PrintOnlineClients()
{
    if(clientSocketList.isEmpty())
    {
        qInfo() << "No client connected...";
        return;
    }
    for(int i=0; i<this->clientSocketList.size(); i++)
    {
        HomuraTcpSocket *socket = this->clientSocketList.at(i);
        qInfo() << i << "\t" << socket->peerAddress().toString() + ":"
                + QString::number(socket->peerPort());
    }
}

void HomuraTcpServer::OneClientDisconnected(qintptr socketDescriptor)
{
    for(int i = 0; i < this->clientSocketList.size(); i++)
    {
        HomuraTcpSocket *item = clientSocketList.at(i);
        if(item->socketDescriptor() == socketDescriptor)
        {
            item->close();
            item->deleteLater();
            clientSocketList.removeAt(i);
            HomuraLogSystem::Log(QtMsgType::QtWarningMsg, "Client [" +
                                 item->peerAddress().toString() + ":" +
                                 QString::number(item->peerPort()) + "] disconnected.");
            break;
        }
    }
}

void HomuraTcpServer::OneClientErrorOccured(qintptr socketDescriptor, QString ErrorReason)
{
    for(int i = 0; i < this->clientSocketList.size(); i++)
    {
        HomuraTcpSocket *item = clientSocketList.at(i);
        if(item->socketDescriptor() == socketDescriptor)
        {
            HomuraLogSystem::Log(QtMsgType::QtCriticalMsg,
                                 "Client [" + item->peerAddress().toString()
                                 + ":" +QString::number(item->peerPort()) +
                                 "] error occured, reason: " + ErrorReason);

            break;
        }
    }
}

void HomuraTcpServer::ReceivedDataFromOneClient(qintptr socketDescriptor, qint64 BytesAvailable, QByteArray ReceivedData)
{
    //20180513 Unimplemented Parse Received Data From Client...

    for(int i = 0; i < this->clientSocketList.size(); i++)
    {
        HomuraTcpSocket *item = clientSocketList.at(i);
        if(item->socketDescriptor() == socketDescriptor)
        {
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg,
                                 "Receieved " + QString::number(BytesAvailable)
                                 + " from [" + item->peerAddress().toString() + ":" +
                                 QString::number(item->peerPort()) + "]: ");

            break;
        }
    }
}

void HomuraTcpServer::incomingConnection(qintptr socketDescriptor)
{
    HomuraTcpSocket *socket = new HomuraTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket,SIGNAL(SocketReceivedData(qintptr,qint64,QByteArray)),
            this,SLOT(ReceivedDataFromOneClient(qintptr,qint64,QByteArray)));
    connect(socket,SIGNAL(SocketDisconnected(qintptr)),
            this,SLOT(OneClientDisconnected(qintptr)));
    connect(socket,SIGNAL(SocketErrorOccured(qintptr,QString)),
            this,SLOT(OneClientErrorOccured(qintptr,QString)));

    this->clientSocketList.append(socket);

    HomuraLogSystem::Log(QtMsgType::QtWarningMsg, "One new client connected, descriptor: "
                         + QString::number(socketDescriptor) + " [" +
                         socket->peerAddress().toString() + ":" +
                         QString::number(socket->peerPort()) + "].");
}
