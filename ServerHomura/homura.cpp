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
        qInfo() << "3. Create&Excute Task";
        qInfo() << "4. View&Cancel Client Tasks";
        qInfo() << "5. Transfer File To Client";
        qInfo() << "6. Update Client Program";
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
                this->TaskProc();
            }
            else
            {
                qInfo() << "Server not start yet.";
            }
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
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

void Homura::TaskProc()
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
        in.flush();
        if(Buffer.toLower() == "all")
        {
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Input all, all clients will recv commands.");
            this->Commands.bAllClients = true;
            break;
        }
        else
        {
            quint8 id = Buffer.toUShort();
            if(id == 666)
            {
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Input 666, list creation over.");
                break;
            }
            else
            {
                this->Commands.ClientsIdList.append(id);
                HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Input host id: "+
                                     QString::number(id));
            }
        }
    }
    if(false == this->Commands.bAllClients && this->Commands.ClientsIdList.isEmpty())
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "No client recv command, return.");
        return;
    }

    qInfo() << "Input command type: [MSG, EXEC, UPDATE]";
    in >> Buffer;
    in.flush();
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command Type: "+Buffer);

    Buffer = Buffer.toUpper();
    if(Buffer == "MSG")
    {
        this->Commands.Type = COMMAND_MSG;
    }
    else if(Buffer == "EXEC")
    {
        this->Commands.Type = COMMAND_EXEC;
    }
    else if(Buffer == "EXECT")
    {
        this->Commands.Type = COMMAND_EXEC_WITHTIMER;
    }
    else if(Buffer == "UPDATE")
    {
        this->Commands.Type = COMMAND_SELFUPDATE;
    }
    else
    {
        qInfo() << "Command type error";
        return;
    }

    quint8 Count = 0;
    while(1)
    {
        qInfo() << "Input ###<cr> to end command creation";
        qInfo() << "Input " << Count+1 << " command:";
        in >> Buffer;
        in.flush();

        if(Buffer != "###")
        {
            this->Commands.CommandList.append(Buffer);
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Command " +
                                 QString::number(Count)+": " + Buffer);
            Count++;
        }
        else
        {
            HomuraLogSystem::Log(QtMsgType::QtInfoMsg,
                                 "Input ###, command creation over.");
            break;
        }
    }
    if(Commands.CommandList.isEmpty())
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "No command in list, return");
        return;
    }

    qInfo() << "Command creation complete!";
    qInfo() << "Input Y/y to send command...";
    in >> Buffer;
    in.flush();
    if(Buffer == "Y" || Buffer == "y")
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg,
                             "Input Y/y, now will send command to client!");
        this->WorkingThread->SendCommandsToClients(this->Commands);
    }
    else
    {
        HomuraLogSystem::Log(QtMsgType::QtInfoMsg,
                             "Input invalid, command will not send!");
    }
}

void Homura::ClearCommand()
{
    this->Commands.bAllClients = false;
    this->Commands.ClientsIdList.clear();
    this->Commands.ExecAfterMinutes = 0;
    this->Commands.Type = COMMAND_MSG;
    this->Commands.CommandList.clear();
    HomuraLogSystem::Log(QtMsgType::QtInfoMsg, "Cleared command structure.");
}
