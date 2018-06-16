#include "homuraserverthread.h"
#include "homuralogsystem.h"
#include <QDataStream>
#include <QTimer>

HomuraServerThread::HomuraServerThread(bool &IsServerStarted) : IsServerStarted(IsServerStarted)
{
    connect(this,SIGNAL(finished()),this,SLOT(ThreadFinished()),Qt::DirectConnection);
}

void HomuraServerThread::run()
{
    this->server = new HomuraTcpServer();
    if(!this->server->isListening())
    {
        HomuraLogSystem::Log(QtMsgType::QtCriticalMsg, "Server listen failed, Working thread will end!");
        this->IsServerStarted = false;
        return;
    }
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Server started success!");
    IsServerStarted = true;
    this->exec();
}

void HomuraServerThread::PrintOnlineClients()
{
    this->server->PrintOnlineClients();
}

// MessageFormat [v0.0.1]:
// Data << Packet size(int) << Command Id(quint8 start from 0) << Command Type
// if(Command Type is MSG/EXEC) Data << Command String
// else if(Command Type is MSGT) Data << AfterMinutes(quint16) << Command String
// Data << EndSign
void HomuraServerThread::BuildCommandsData(qint8 TotalCommandNum, quint8 WhichCommandId, COMMAND Command, QByteArray &DataToSend)
{
    QByteArray SendDataBuffer;
    QDataStream dsIntoBuffer(&SendDataBuffer,QIODevice::WriteOnly);
    dsIntoBuffer.setVersion(QDataStream::Qt_5_10);

    dsIntoBuffer << WhichCommandId << (int)Command.Type;
    if(COMMAND_MSG == Command.Type || COMMAND_EXEC == Command.Type)
    {
        dsIntoBuffer << Command.CommandList.at(WhichCommandId);
    }
    else
    {
        dsIntoBuffer << Command.ExecAfterMinutes
                     << Command.CommandList.at(WhichCommandId);
    }

    if(WhichCommandId == TotalCommandNum)
    {
        dsIntoBuffer << LAST_COMMAND_END_TAG;
    }
    else
    {
        dsIntoBuffer << COMMAND_END_TAG;
    }
    DataToSend.clear();

    QDataStream dsIn(&DataToSend, QIODevice::WriteOnly);
    dsIn.setVersion(QDataStream::Qt_5_10);
    int PacketSize = SendDataBuffer.size() + sizeof(int)*2;
    dsIn << PacketSize << SendDataBuffer;
}

void HomuraServerThread::SendCommandsToClients(COMMAND Command)
{
    QByteArray DataToSend;
    qint8 TotalCommandNum = Command.CommandList.size() - 1; // start from 0

    for(int i= 0; i < TotalCommandNum; i++)
    {
        this->BuildCommandsData(TotalCommandNum, i, Command, DataToSend);
    }

    if(true == Command.bAllClients)
    {
        this->server->SendDataToClients(true, Command.ClientsIdList, DataToSend);
    }
    else
    {
        this->server->SendDataToClients(false, Command.ClientsIdList, DataToSend);
    }
}

void HomuraServerThread::ThreadFinished()
{
    this->IsServerStarted = false;
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Working thread ended.");
    this->deleteLater();
}
