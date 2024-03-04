#include "cdatabase.h"
#include <QDebug>

cdatabase::cdatabase(QString IP, QString Db, QString username, QString password, QString Port)  {
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    QString dsn ="DRIVER={MySQL ODBC 8.3 Unicode Driver};SERVER="+IP+";PORT="+Port+";DATABASE="+Db+";UID="+username+";PWD="+password+";TDS_Version=4.2;";
    db.setDatabaseName(dsn);
    this->db = db;
    if (!db.open())
    {
        qDebug() << "La connexion a la bdd a echoue...";
    }
    else qDebug() << "La connexion a reussi";
}
