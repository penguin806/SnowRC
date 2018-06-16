#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QObject>
#include <QList>
#include <QThread>
#include "homuratcpserver.h"
#include "homuracustomtype.h"

class HomuraServerThread : public QThread
{
    Q_OBJECT
public:
    HomuraServerThread(bool &IsServerStarted);
    void run();
    void PrintOnlineClients();
    void BuildCommandsData(qint8 TotalCommandNum, quint8 WhichCommandId, COMMAND Command, QByteArray &DataToSend);
    void SendCommandsToClients(COMMAND Command);
public slots:
    void ThreadFinished();
private:
    HomuraTcpServer *server;
    bool &IsServerStarted;
};

#endif // SERVERTHREAD_H
