#ifndef CDATABASE_H
#define CDATABASE_H


#include <QString>
#include <QSqlDatabase>

class cdatabase
{
    QSqlDatabase db;
public:
    cdatabase(QString IP, QString Db, QString username, QString password, QString Port);
    ~cdatabase();
    bool exporterDatabaseSql();
    bool envoieDatabaseSql();



};

#endif // CDATABASE_H
