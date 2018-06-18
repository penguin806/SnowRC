#include "madokacmdrunnable.h"
#include "madokalogsystem.h"
#include <QThread>

MadokaCmdRunnable::MadokaCmdRunnable(SVRCOMMAND CommandStruct) : QRunnable()
{
    this->CommandStruct = CommandStruct;
}

void MadokaCmdRunnable::run()
{
    this->ThreadIdString = QString::number(quintptr(QThread::currentThreadId()));
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Thread started! [Id: " +
                         ThreadIdString + "]");

    if(this->CommandStruct.Type == COMMAND_EXEC_WITHTIMER)
    {
        MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Thread will sleep " +
                             QString::number(this->CommandStruct.ExecAfterMinutes) +
                             " minutes! [Id: " + ThreadIdString + "]");
        QThread::sleep(this->CommandStruct.ExecAfterMinutes * 60);
        this->ExecuteCommand(this->CommandStruct.Command);
    }
    else if(this->CommandStruct.Type == COMMAND_EXEC)
    {
        this->ExecuteCommand(this->CommandStruct.Command);
    }
    else if(this->CommandStruct.Type == COMMAND_SELFUPDATE)
    {

    }
}

void MadokaCmdRunnable::ExecuteCommand(QString CommandString)
{
#ifdef Q_OS_WIN
    this->EventLoop = new QEventLoop();
    this->ShellProcess = new QProcess();
    QObject::connect(this->ShellProcess, SIGNAL(finished(int)), this, SLOT(ProcessFinished(int)));
    QObject::connect(this->ShellProcess, SIGNAL(readyReadStandardOutput()),this,SLOT(ProcessReadyReadStdOut()));
    QObject::connect(this->ShellProcess, SIGNAL(readyReadStandardError()),this,SLOT(ProcessReadyReadStdErr()));

    QStringList Arguments;
    Arguments << "/c" << CommandString;
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Thread starting shell process... [Id: " +
                         ThreadIdString + "]");
    this->ShellProcess->start("cmd", Arguments);
    if(false == this->ShellProcess->waitForStarted())
    {
        MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Thread start shell process FAIL, return. [Id: " +
                             ThreadIdString + "]");
        return;
    }
    else
    {
        MadokaLogSystem::Log(QtMsgType::QtCriticalMsg, "Thread start shell process SUCCESS. [Id: " +
                             ThreadIdString + "]");
    }

    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Thread start eventLoop. [Id: " +
                         this->ThreadIdString + "]");
    this->EventLoop->exec();
#endif
}

void MadokaCmdRunnable::ProcessFinished(int)
{
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, "Thread task finished, exit eventloop. [Id: " +
                         this->ThreadIdString + "]");
    this->EventLoop->exit();
    this->ShellProcess->deleteLater();
    this->EventLoop->deleteLater();
}

void MadokaCmdRunnable::ProcessReadyReadStdErr()
{
    QString StdErrString = QString::fromLocal8Bit(this->ShellProcess->readAllStandardError());
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, StdErrString + " [Stderr from thread id: " +
                         this->ThreadIdString + "]");
}

void MadokaCmdRunnable::ProcessReadyReadStdOut()
{
    QString StdOutString = QString::fromLocal8Bit(this->ShellProcess->readAllStandardOutput());
    MadokaLogSystem::Log(QtMsgType::QtInfoMsg, StdOutString + " [Stdout from thread id: " +
                         this->ThreadIdString + "]");
}
