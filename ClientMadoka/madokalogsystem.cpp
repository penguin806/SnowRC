#include <QFile>
#include <QDateTime>
#include <QDebug>
#include "madokalogsystem.h"

MadokaLogSystem::MadokaLogSystem()
{

}

void MadokaLogSystem::Log(QtMsgType Level, QString Info)
{
    qInfo() << Info;

    const QString LogFilePath = "D:\\mlog.txt";
    QFile LogFile(LogFilePath);
    if(false == LogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "Cannot Open Log File.";
        return;
    }

    QString Buffer;
    switch (Level) {
    case QtMsgType::QtDebugMsg :
        Buffer = "[Debug]   \t";
        break;
    case QtMsgType::QtInfoMsg :
        Buffer = "[Info]    \t";
        break;
    case QtMsgType::QtWarningMsg :
        Buffer = "[Warning] \t";
        break;
    case QtMsgType::QtCriticalMsg :
        Buffer = "[Critical]\t";
        break;
    default:
        Buffer = "[Info]    \t";
        break;
    }

    Buffer += QDateTime::currentDateTime().toString(Qt::ISODate);
    Buffer += " ";
    Buffer += Info;
    Buffer += "\n";

    QTextStream out(&LogFile);
    out << Buffer;
    LogFile.flush();
    LogFile.close();
}
