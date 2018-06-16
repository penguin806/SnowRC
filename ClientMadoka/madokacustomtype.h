#ifndef MADOKACUSTOMTYPE_H
#define MADOKACUSTOMTYPE_H
#include <QVersionNumber>

extern QVersionNumber MadokaAppVersion;

enum ServerCommandType            //v0.0.1
{
    COMMAND_MSG = 0,        //MSG
    COMMAND_EXEC,           //EXEC
    COMMAND_EXEC_WITHTIMER, //EXECT
    COMMAND_VIEWCLIENTTASKS,//VIEWTASK   20180513 Unimplemented
    COMMAND_CANCELTASK,     //CANCELTASK 20180513 Unimplemented
    COMMAND_SELFUPDATE      //UPDATE
};

struct ServerCommandInfo
{
    ServerCommandType Type;
    QList<QString> CommandList;
    quint16 ExecAfterMinutes;   //work with CommandType EXECT
};
typedef ServerCommandInfo SVRCOMMAND;

#define COMMAND_END_TAG -1
#define LAST_COMMAND_END_TAG -2

#endif // MADOKACUSTOMTYPE_H
