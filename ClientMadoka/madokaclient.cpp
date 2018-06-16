#include "madokaclient.h"
#include "madokalogsystem.h"
#include "madokacustomtype.h"
#include <QDataStream>

MadokaClient::MadokaClient(QObject *parent) : QObject(parent)
{

}

void MadokaClient::StartConnectToServer()
{
    this->socket = new QTcpSocket(this);
    connect(this->socket,SIGNAL(hostFound()),this,SLOT(ServerHostFound()));
    connect(this->socket,SIGNAL(connected()),this,SLOT(ConnectedToServer()));
    connect(this->socket,SIGNAL(disconnected()),this,SLOT(DisconnectedFromServer()));
    connect(this->socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(SocketError(QAbstractSocket::SocketError)));
    connect(this->socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(SocketStateChanged(QAbstractSocket::SocketState)));
    connect(this->socket,SIGNAL(readyRead()),this,SLOT(ReceivedDataFromServer()));

    this->socket->connectToHost(this->ServerAddress,this->ServerPort);
}

// MessageFormat [v0.1.0]:
// Data << Packet size(int) <<  Command Type(int) << AfterMinutes(quint16)
// << Command String << EndTag(qint64)
void MadokaClient::ParseDataReceivedFromServer(QByteArray ReceivedDataBuffer, qint64 BytesAvailable)
{
    QDataStream dsOut_A(&ReceivedDataBuffer, QIODevice::ReadOnly);
    dsOut_A.setVersion(QDataStream::Qt_5_10);

    int PackageSize = 0;
    dsOut_A >> PackageSize;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field1-PackageSize: " + QString::number(PackageSize));
    if(PackageSize != BytesAvailable)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field1-PackageSize not equal to BytesAvailable, return.");
        return;
    }

    QByteArray ReceivedData;
    dsOut_A >> ReceivedData;
    QDataStream dsOut_B(&ReceivedData, QIODevice::ReadOnly);
    dsOut_B.setVersion(QDataStream::Qt_5_10);

    int CommandType = 0;
    quint16 AfterMinutes = 0;
    QString CommandString;
    qint64 CommandEndTag = 0;
    dsOut_B >> CommandType >> AfterMinutes >> CommandString >> CommandEndTag;

    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field2-CommandType: " + QString::number(CommandType));
    if(CommandType < COMMAND_MSG || CommandType > COMMAND_SELFUPDATE)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field2-CommandType invalid, return.");
        return;
    }

    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field3-AfterMinutes: " + QString::number(AfterMinutes));
    if(AfterMinutes > 525600 /*One year*/)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field3-AfterMinutes larger than 525600, return.");
        return;
    }

    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field4-CommandString: " + CommandString);
    if(CommandString.isEmpty())
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field4-CommandString empty, return.");
        return;
    }

    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field5-EndTag: " + QString::number(CommandEndTag));
    if(CommandEndTag != COMMAND_END_TAG)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field5-EndTag invalid, return.");
        return;
    }

    //Each part of recv packet is ok, now execute command!
    SVRCOMMAND CurrentCommand;
    CurrentCommand.Type = (ServerCommandType)CommandType;
    CurrentCommand.ExecAfterMinutes = AfterMinutes;
    CurrentCommand.Command = CommandString;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Packet correct, now execute command!");
    this->ExecuteCommand(CurrentCommand);
}

void MadokaClient::SendDataToServer(QByteArray DataToSend)
{

}

void MadokaClient::ExecuteCommand(SVRCOMMAND Command)
{
    switch (Command.Type) {
    case COMMAND_MSG:

        break;
    default:
        break;
    }
}

void MadokaClient::ServerHostFound()
{
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "ServerHostFound.");
}

void MadokaClient::ConnectedToServer()
{
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "ServerConnected: " + this->socket->peerName());
}

void MadokaClient::DisconnectedFromServer()
{
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "ServerDisconnected");
}

void MadokaClient::SocketError(QAbstractSocket::SocketError /*socketError*/)
{
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "SocketError: " + this->socket->errorString());
}

void MadokaClient::SocketStateChanged(QAbstractSocket::SocketState socketState)
{
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "SocketStateChanged: " + QString::number(socketState));
}

void MadokaClient::ReceivedDataFromServer()
{
    qint64 BytesAvailable = this->socket->bytesAvailable();
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "ReceivedBytes: " +
                  QString::number(BytesAvailable));

    QByteArray ReceivedData;
    ReceivedData = this->socket->readAll();

    this->ParseDataReceivedFromServer(ReceivedData, BytesAvailable);
}
