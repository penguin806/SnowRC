#ifndef HOMURALOGGINGSYSTEM_H
#define HOMURALOGGINGSYSTEM_H

#include <QObject>

class HomuraLogSystem
{
public:
    HomuraLogSystem();
    static void Log(QtMsgType Level = QtMsgType::QtInfoMsg, QString Info = "NoInfo");
};

#endif // HOMURALOGGINGSYSTEM_H
