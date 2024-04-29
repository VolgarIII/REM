#ifndef CDATABASE_H
#define CDATABASE_H


#include "csauvegarde.h"
#include <QString>
#include <QSqlDatabase>
#include <filesystem>
using namespace std;

class cdatabase
{
    QSqlDatabase db;
    QString ip;

public:
    cdatabase(QString IP, QString Db, QString username, QString password, QString Port);
    ~cdatabase();
    QStringList getListeRPIConnectes();
    bool exporterDatabaseSql();
    bool importerDatabaseSql();

};

#endif // CDATABASE_H
