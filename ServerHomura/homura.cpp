#include "homura.h"
#include "homuralogsystem.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QTimer>

Homura::Homura(QObject *parent) : QObject(parent)
{
    this->bIsServerStarted = false;
}

void Homura::StartThisServer()
{
    if(true == this->bIsServerStarted)
    {
        HomuraLogSystem::Log(QtMsgType::QtCriticalMsg, "Server already started, return.");
        return;
    }
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Starting Server...");
    this->WorkingThread = new HomuraServerThread(this->bIsServerStarted);
    this->WorkingThread->start();
}

void Homura::CommandUserInterfaceProc()
{
    while(1)
    {
        qInfo("==================================");
        qInfo() << "1. Start Server";
        qInfo() << "2. Display Online Clients";
        qInfo() << "3. Send Command To Clients";
        qInfo() << "4. Update Client Program";  //Unimplemented
        qInfo() << "0. Exit" << endl;

        QTextStream in(stdin, QIODevice::ReadOnly);
        QString ch;
        in >> ch;
        switch (ch.toInt()) {
        case 1:
            this->StartThisServer();
            break;
        case 2:
            if(true == this->bIsServerStarted)
            {
                this->WorkingThread->PrintOnlineClients();
            }
            else
            {
                qInfo() << "Server not start yet.";
            }
            break;
        case 3:
            if(true == this->bIsServerStarted)
            {
                this->SendCommandProc();
            }
            else
            {
                qInfo() << "Server not start yet.";
            }
            break;
        case 4:
            qInfo("Update Client Program (Unimplemented)");
            break;
        case 0:
            if(true == this->bIsServerStarted)
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Quit menu choosed, wait for end working thread...");
                this->WorkingThread->quit();
                this->WorkingThread->wait();
            }
            else
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Server not started.");
            }
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Now quit application...");
            //qApp->exit(0);
            return;
        default:
            qInfo("Invalid option");
            break;
        }
        qInfo("\n\n\n");
        //qInfo("Press any key to continue...");
        //in >> ch;
    }
}

void Homura::SendCommandProc()
{
    if(false == this->bIsServerStarted)
    {
        qInfo() << "Server not start yet.";
        return;
    }

    this->ClearCommand();

    QTextStream in(stdin,QIODevice::ReadOnly);
    QString Buffer;

    qInfo() << "Input clients-id list which recv command: [eg: '0 1'/'ALL'], input 666 to end.";
    while(1)
    {
        in >> Buffer;
        if(Buffer.toLower() == "all")
        {
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Input all, all clients will recv commands.");
            this->Command.bAllClients = true;
            break;
        }
        else
        {
            quint16 id = Buffer.toUShort();
            if(id == 666)
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Input 666, list creation over.");
                break;
            }
            else
            {
                this->Command.ClientsIdList.append(id);
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Input host id: "+
                                     QString::number(id));
            }
        }
    }
    if(false == this->Command.bAllClients && this->Command.ClientsIdList.isEmpty())
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "No client recv command, return.");
        return;
    }

    qInfo() << "Input command type: [MSG, EXEC, EXECT, UPDATE]";
    in >> Buffer;
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command Type: "+Buffer);

    Buffer = Buffer.toUpper();
    if(Buffer == "MSG")
    {
        this->Command.Type = COMMAND_MSG;
    }
    else if(Buffer == "EXEC")
    {
        this->Command.Type = COMMAND_EXEC;
    }
    else if(Buffer == "EXECT")
    {
        this->Command.Type = COMMAND_EXEC_WITHTIMER;
        qInfo() << "Input Timer Value: (Command will execute after x minutes)";

        in >> Buffer;
        this->Command.ExecAfterMinutes = Buffer.toUShort();
        if(this->Command.ExecAfterMinutes == 0)
        {
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command Timer Value Invalid, return");
            return;
        }
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command Timer Value:"  + this->Command.ExecAfterMinutes);
    }
    else if(Buffer == "UPDATE")
    {
        this->Command.Type = COMMAND_SELFUPDATE;
    }
    else
    {
        qInfo() << "Command type error";
        return;
    }

    qInfo() << "Input command:";
    in.skipWhiteSpace();
    Buffer = in.readLine();
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command info:"  + Buffer);

    if(Buffer.isEmpty())
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command empty, return");
        return;
    }
    this->Command.CommandString = Buffer;

    qInfo() << "Input n/N to cancel, or anything else to send!";
    in >> Buffer;
    if(Buffer == "n" || Buffer == "N")
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg,
                             "Input n/N, cancel command send process, return");
    }
    else
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg,
                             "Now will send command to client!");
        this->WorkingThread->SendCommandToClients(this->Command);
    }
}

void Homura::ClearCommand()
{
    this->Command.bAllClients = false;
    this->Command.ClientsIdList.clear();
    this->Command.ExecAfterMinutes = 0;
    this->Command.Type = COMMAND_MSG;
    this->Command.CommandString.clear();
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Cleared command structure.");
}
