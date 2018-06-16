#include "homuraserverthread.h"
#include "homuralogsystem.h"
#include <QDataStream>

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

// MessageFormat [v0.1.0]:
// Data << Packet size(int) <<  Command Type(int) << AfterMinutes(quint16)
// << Command String << EndTag(qint64)
void HomuraServerThread::BuildCommandData(/*In*/COMMAND Command, /*Out*/QByteArray &DataToSend)
{
    DataToSend.clear();

    QByteArray SendDataBuffer;
    QDataStream dsIn_A(&SendDataBuffer,QIODevice::WriteOnly);
    dsIn_A.setVersion(QDataStream::Qt_5_10);
    dsIn_A << (int)Command.Type << Command.ExecAfterMinutes
                 << Command.CommandString << (qint64)COMMAND_END_TAG;

    QDataStream dsIn_B(&DataToSend, QIODevice::WriteOnly);
    dsIn_B.setVersion(QDataStream::Qt_5_10);
    int PacketSize = SendDataBuffer.size() + sizeof(int)*2; //Actually this needs + 4 bytes
    dsIn_B << PacketSize << SendDataBuffer;
}

void HomuraServerThread::SendCommandToClients(COMMAND Command)
{
    QByteArray DataToSend;

    this->BuildCommandData(Command, DataToSend);

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
