#include "madokacmdrunnable.h"
#include <QThread>
#include <QProcess>

MadokaCmdRunnable::MadokaCmdRunnable(SVRCOMMAND CommandStruct) : QRunnable()
{
    this->CommandStruct = CommandStruct;
}

void MadokaCmdRunnable::run()
{
    if(this->CommandStruct.Type == COMMAND_EXEC_WITHTIMER)
    {
        QThread::sleep(this->CommandStruct.ExecAfterMinutes * 60 * 1000);
        MadokaCmdRunnable::ExecuteCommand(this->CommandStruct.Command);
    }
    else if(this->CommandStruct.Type == COMMAND_EXEC)
    {
        MadokaCmdRunnable::ExecuteCommand(this->CommandStruct.Command);
    }
}

void MadokaCmdRunnable::ExecuteCommand(QString CommandString)
{

}
