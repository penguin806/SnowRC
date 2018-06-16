#ifndef HOMURA_H
#define HOMURA_H

#include <QObject>
#include <QList>
#include "homuraserverthread.h"
#include "homuracustomtype.h"

class Homura : public QObject
{
    Q_OBJECT
public:
    explicit Homura(QObject *parent = nullptr);
    void StartThisServer();
    void CommandUserInterfaceProc();
    void SendCommandProc();
    void ClearCommand();

private:
    HomuraServerThread *WorkingThread;
    COMMAND Command;            //20180616 v0.1 Cancel Task System -> Single Comand
    bool bIsServerStarted;
};

#endif // HOMURA_H
