#ifndef MADOKACMDRUNNABLE_H
#define MADOKACMDRUNNABLE_H

#include <QRunnable>
#include "madokacustomtype.h"

class MadokaCmdRunnable : public QRunnable
{
public:
    MadokaCmdRunnable(SVRCOMMAND CommandStruct);
    void run();
    static void ExecuteCommand(QString CommandString);

private:
    SVRCOMMAND CommandStruct;
};

#endif // MADOKACMDRUNNABLE_H
