#ifndef MADOKACLIENT_H
#define MADOKACLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QThreadPool>
#include "madokacustomtype.h"

class MadokaClient : public QObject
{
    Q_OBJECT
public:
    explicit MadokaClient(QObject *parent = nullptr);

    void StartConnectToServer();
    void ParseDataReceivedFromServer(QByteArray ReceivedData, qint64 BytesAvailable);
    void SendDataToServer(QByteArray DataToSend);

    void CommandProc(SVRCOMMAND CommandStruct);

signals:

private slots:
    void ServerHostFound();
    void ConnectedToServer();
    void DisconnectedFromServer();
    void SocketError(QAbstractSocket::SocketError /*socketError*/);
    void SocketStateChanged(QAbstractSocket::SocketState socketState);
    void ReceivedDataFromServer();

private:
    const QString ServerAddress = "127.0.0.1";
    const qint16 ServerPort = qint16(40500);
    QTcpSocket *socket;

    QThreadPool WorkingThreadPool;
};

#endif // MADOKACLIENT_H
