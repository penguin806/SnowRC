#ifndef MADOKALOGSYSTEM_H
#define MADOKALOGSYSTEM_H

#include <QObject>

class MadokaLogSystem
{
public:
    MadokaLogSystem();
    static void Log(QtMsgType Level = QtMsgType::QtInfoMsg, QString Info = "NoInfo");
};

#endif // MADOKALOGSYSTEM_H
