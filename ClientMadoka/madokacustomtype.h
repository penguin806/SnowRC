#ifndef MADOKACUSTOMTYPE_H
#define MADOKACUSTOMTYPE_H
#include <QVersionNumber>

extern QVersionNumber MadokaAppVersion;

enum ServerCommandType      //v0.1.0
{
    COMMAND_MSG = 0,        //MSG
    COMMAND_EXEC,           //EXEC
    COMMAND_EXEC_WITHTIMER, //EXECT
    COMMAND_SELFUPDATE      //UPDATE
};

struct ServerCommandInfo
{
    ServerCommandType Type;
    QString Command;
    quint16 ExecAfterMinutes;   //work with CommandType EXECT
};
typedef ServerCommandInfo SVRCOMMAND;

#define COMMAND_END_TAG 0x102420484096

#endif // MADOKACUSTOMTYPE_H
