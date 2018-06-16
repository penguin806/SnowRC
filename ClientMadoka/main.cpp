#include <QCoreApplication>
#include <madokacustomtype.h>
#include "madokaclient.h"

QVersionNumber MadokaAppVersion = QVersionNumber(0,0,1);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MadokaClient client;
    client.StartConnectToServer();

    return a.exec();
}
