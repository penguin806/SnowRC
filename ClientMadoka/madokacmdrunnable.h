#ifndef MADOKACMDRUNNABLE_H
#define MADOKACMDRUNNABLE_H

#include <QRunnable>
#include <QEventLoop>
#include <QProcess>
#include "madokacustomtype.h"

class MadokaCmdRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MadokaCmdRunnable(SVRCOMMAND CommandStruct);
    void run();
    void ExecuteCommand(QString CommandString);

public slots:
    void ProcessFinished(int /*ExitCode*/);
    void ProcessReadyReadStdErr();
    void ProcessReadyReadStdOut();

private:
    QString ThreadIdString;
    SVRCOMMAND CommandStruct;
    QEventLoop *EventLoop;
    QProcess *ShellProcess;
};

#endif // MADOKACMDRUNNABLE_H
