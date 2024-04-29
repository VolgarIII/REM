#ifndef CLOG_H
#define CLOG_H

#include <QString>
#include <filesystem>
#include <QFileInfoList>
#include <QDir>
using namespace std;

class CLog
{
    QString nomFichier;
    filesystem::path dossierLog = filesystem::current_path()/ "log";
    public:
        CLog(QString FileName);
        CLog();
        ~CLog();
        void Addlog(QString Event, QString Detail);
};

#endif // CLOG_H
