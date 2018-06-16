#ifndef HOMURATCPSOCKET_H
#define HOMURATCPSOCKET_H

#include <QObject>
#include <QTcpSocket>

//struct ClientInfo
//{
//    QString HostName;
//    QHostAddress Address;
//    quint16 Port;
//};
//typedef struct ClientInfo CLIENTINFO;

class HomuraTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit HomuraTcpSocket(QObject *parent = Q_NULLPTR);

signals:
    void SocketReceivedData(qintptr , qint64 , QByteArray );
    void SocketDisconnected(qintptr );
    void SocketErrorOccured(qintptr , QString );

//    void setClientInfo();
//    CLIENTINFO getClientInfo() const;

private slots:
    void ReadyReadReceivedData();
    void Disconnected();
    void SocketError(QAbstractSocket::SocketError /*socketError*/);
};

#endif // HOMURATCPSOCKET_H
