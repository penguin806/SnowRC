#ifndef HOMURACUSTOMTYPE_H
#define HOMURACUSTOMTYPE_H
#include <QVersionNumber>

extern QVersionNumber HomuraAppVersion;

enum CommandType            //v0.0.1
{
    COMMAND_MSG = 0,        //MSG
    COMMAND_EXEC,           //EXEC
    COMMAND_EXEC_WITHTIMER, //EXECT
    COMMAND_VIEWCLIENTTASKS,//VIEWTASK   20180513 Unimplemented
    COMMAND_CANCELTASK,     //CANCELTASK 20180513 Unimplemented
    COMMAND_SELFUPDATE      //UPDATE
};

struct CommandInfo
{
    bool bAllClients;
    QList<quint8> ClientsIdList;
    CommandType Type;
    QList<QString> CommandList;
    quint16 ExecAfterMinutes;   //work with CommandType EXECT
};
typedef CommandInfo COMMAND;

#define COMMAND_END_TAG -1
#define LAST_COMMAND_END_TAG -2

#endif // HOMURACUSTOMTYPE_H
