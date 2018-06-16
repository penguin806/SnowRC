#ifndef HOMURACUSTOMTYPE_H
#define HOMURACUSTOMTYPE_H
#include <QVersionNumber>

extern QVersionNumber HomuraAppVersion;

enum CommandType            //v0.1.0
{
    COMMAND_MSG = 0,        //MSG
    COMMAND_EXEC,           //EXEC
    COMMAND_EXEC_WITHTIMER, //EXECT
    COMMAND_SELFUPDATE      //UPDATE
};

struct CommandInfo          //v0.1.0
{
    bool bAllClients;
    QList<quint8> ClientsIdList;
    CommandType Type;
    QString CommandString;
    quint16 ExecAfterMinutes;   //work with CommandType EXECT
};
typedef CommandInfo COMMAND;

#define COMMAND_END_TAG 0x102420484096;

#endif // HOMURACUSTOMTYPE_H
