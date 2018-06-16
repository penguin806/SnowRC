#include <QCoreApplication>
#include "homuracustomtype.h"
#include "homura.h"

QVersionNumber HomuraAppVersion = QVersionNumber(0,0,1);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Homura hom;
    hom.CommandUserInterfaceProc();

    return 0;
}
