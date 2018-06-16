#ifndef HOMURATCPSERVER_H
#define HOMURATCPSERVER_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include "homuratcpsocket.h"

class HomuraTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HomuraTcpServer(QObject *parent = Q_NULLPTR);
    static void ParseReceivedData(QByteArray ReceivedData);
    void SendDataToClients(bool bAllClient, QList<quint8> ClientIdListToSend, QByteArray DataToSend);

    void PrintOnlineClients();

public slots:
    void OneClientDisconnected(qintptr socketDescriptor);
    void OneClientErrorOccured(qintptr socketDescriptor, QString ErrorReason);
    void ReceivedDataFromOneClient(qintptr socketDescriptor, qint64 BytesAvailable, QByteArray ReceivedData);

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

private:
    const quint16 ServerListenPort = 40500;
    QList<HomuraTcpSocket *> clientSocketList;
};

#endif // HOMURATCPSERVER_H
