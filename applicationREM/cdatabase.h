#ifndef CDATABASE_H
#define CDATABASE_H

#include <QString>
#include <QSqlDatabase>
class cdatabase
{
private:
     QSqlDatabase db;

public:
    cdatabase(QString IP, QString Db, QString username, QString password, QString Port);
};

#endif // CDATABASE_H
