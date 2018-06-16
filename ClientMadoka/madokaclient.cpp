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


// MessageFormat [v0.0.1]:
// Data << Packet size(int) << Command Id(quint8 start from 0) << Command Type
// if(Command Type is MSG/EXEC) Data << Command String
// else if(Command Type is MSGT) Data << AfterMinutes(quint16) << Command String
// Data << EndSign(LAST_COMMAND_END_TAG / COMMAND_END_TAG)
void MadokaClient::ParseDataReceivedFromServer(QByteArray ReceivedDataBuffer, qint64 BytesAvailable)
{
    QDataStream RecvDataTempOutStream(&ReceivedDataBuffer, QIODevice::ReadOnly);
    RecvDataTempOutStream.setVersion(QDataStream::Qt_5_10);

    int PackageSize = 0;
    RecvDataTempOutStream >> PackageSize;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field1-PackageSize: " + QString::number(PackageSize));
    if(PackageSize != BytesAvailable)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field1-PackageSize not equal to BytesAvailable, return.");
        this->ClearTempTask();
        return;
    }

    QByteArray ReceivedData;
    RecvDataTempOutStream >> ReceivedData;
    QDataStream RecvDataOutStream(&ReceivedData, QIODevice::ReadOnly);
    RecvDataOutStream.setVersion(QDataStream::Qt_5_10);

    qint8 CommandId = 0;
    RecvDataOutStream >> CommandId;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field2-CommandId: " + QString::number(CommandId));
    if(CommandId != this->TempTask.CommandList.size())
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field2-CommandId not equal to CommandList.size(), return.");
        this->ClearTempTask();
        return;
    }

    int CommandType = 0;
    RecvDataOutStream >> CommandType;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field3-CommandType: " + QString::number(CommandType));
    if(this->TempTask.CommandList.isEmpty())
    {
        this->TempTask.Type = (ServerCommandType)CommandType;
    }
    else if(this->TempTask.Type != CommandType)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field3-CommandType not equal to prev recv CommandType, return.");
        this->ClearTempTask();
        return;
    }

    QString CommandString;
    quint16 AfterMinutes = 0;

    switch (CommandType) {
    case COMMAND_MSG:
    case COMMAND_EXEC:
        RecvDataOutStream >> CommandString;
        MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field4-CommandString: " + CommandString);
        if(CommandString.isEmpty())
        {
            MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field4-CommandString empty, return.");
            this->ClearTempTask();
            return;
        }
        this->TempTask.CommandList.append(CommandString);
        break;
    case COMMAND_EXEC_WITHTIMER:
        RecvDataOutStream >> AfterMinutes >> CommandString;
        MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field4-AfterMinutes: " + QString::number(AfterMinutes));
        this->TempTask.ExecAfterMinutes = AfterMinutes;
        MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Field5-CommandString: " + CommandString);
        if(CommandString.isEmpty())
        {
            MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field5-CommandString empty, return.");
            this->ClearTempTask();
            return;
        }
        this->TempTask.CommandList.append(CommandString);
        break;
    default:
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Field3-CommandType invalid, return.");
        this->ClearTempTask();
        return;
    }

    int EndSign;
    RecvDataOutStream >> EndSign;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Packet end sign: " + QString::number(EndSign));
    if(EndSign == LAST_COMMAND_END_TAG)
    {
        this->TaskList.append(this->TempTask);
        MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Last command of the task, append to task list.");
    }
    else if(EndSign != COMMAND_END_TAG)
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "EndSign invalid, return.");
        this->ClearTempTask();
    }
}

void MadokaClient::SendDataToServer(QByteArray DataToSend)
{

}

void MadokaClient::ClearTempTask()
{
    this->TempTask.CommandList.clear();
    this->TempTask.ExecAfterMinutes = 0;
    this->TempTask.Type = COMMAND_MSG;
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
