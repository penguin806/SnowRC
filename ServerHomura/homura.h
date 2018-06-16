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
    void TaskProc();
    void ClearCommand();

private:
    HomuraServerThread *WorkingThread;
    COMMAND Commands;
    bool bIsServerStarted;
};

#endif // HOMURA_H
