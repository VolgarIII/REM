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

public:
    cdatabase(QString IP, QString Db, QString username, QString password, QString Port);
    ~cdatabase();
    bool exporterDatabaseSql(); // Récupération des données
    bool importerDatabaseSql(); // Envoie des données



};

#endif // CDATABASE_H
