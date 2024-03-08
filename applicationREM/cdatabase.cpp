#include "cdatabase.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

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

cdatabase::~cdatabase()
{
    qDebug() << "Fermeture de la connexion...";
    db.close();
}

bool cdatabase::exporterDatabaseSql()
{

    if (!db.isOpen()) {
           qDebug() << "La connexion à la base de données n'est pas ouverte.";
           return false;
       }

        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        QString outputPath = "recup_" + timestamp + ".sql";
        QFile outputFile(outputPath);

        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
           qDebug() << "Impossible d'ouvrir le fichier de sortie.";
           return false;
       }

       QTextStream out(&outputFile);

       QStringList tables = db.tables();
       for (const QString& table : tables) {
           QSqlQuery query("SELECT * FROM " + table, db);
           QSqlRecord rec = query.record();
           int fieldCount = rec.count();


           while (query.next()) {
               out << "INSERT INTO " << table << " VALUES (";
               for (int i = 0; i < fieldCount; ++i) {
                   out << "'" << query.value(i).toString() << "'";
                   if (i < fieldCount - 1) {
                       out << ", ";
                   }
               }
               out << ");\n";
           }
           out << "\n";
       }

       outputFile.close();
       return true;
   }


