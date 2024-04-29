#include "cdatabase.h"
#include "csauvegarde.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <filesystem>
#include <iostream>
using namespace std;

cdatabase::cdatabase(QString IP, QString Db, QString username, QString password, QString Port)
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
     {
         db = QSqlDatabase::database("qt_sql_default_connection");
     } else {
         db = QSqlDatabase::addDatabase("QODBC", "qt_sql_default_connection");
     }

    QString dsn = "DRIVER={MySQL ODBC 8.3 Unicode Driver};SERVER=" + IP + ";PORT=" + Port + ";DATABASE=" + Db + ";UID=" + username + ";PWD=" + password + ";TDS_Version=4.2;";
    db.setDatabaseName(dsn);
    this->ip= IP;
    if (!db.open())
    {
        qDebug() << "La connexion a la bdd a echoue...";
    }
    else
        qDebug() << "La connexion a reussi";

}

cdatabase::~cdatabase()
{
    qDebug() << "Fermeture de la connexion à"<< this->ip << "...";
    db.close();
}


QStringList cdatabase::getListeRPIConnectes()
{
    QStringList list;
    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return list;
    }
    if (this->ip == "192.168.21.254")
    {
        QStringList tables = db.tables();
        for (const QString &table : tables)
        {
            if (table == "moduleconnectee" )
            {
                QSqlQuery query("SELECT * FROM " + table, db);
                QSqlRecord rec = query.record();
                int fieldCount = rec.count();

                while (query.next())
                {
                    for (int i = 0; i < fieldCount; ++i)
                    {
                       QString nomField = rec.fieldName(i);

                       if (nomField == "AdresseIP")
                       {
                            list << query.value(i).toStringList();
                       }
                    }
               }

            }
        }
        return list;
    } else
        return list;
}



bool cdatabase::exporterDatabaseSql()
{

    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false;
    }
    csauvegarde save;
    QStringList tables = db.tables();
    QString fullQueryString;
    for (const QString &table : tables)
    {
        if (table == "porte" || table == "capteurconso")
        {
            QSqlQuery query("SELECT * FROM " + table, db);
            QSqlRecord rec = query.record();
            int fieldCount = rec.count();

            if (query.next())
            {

                QString insertQueryString = "INSERT INTO " + table + " VALUES ";
                QStringList valeur;

                while (query.next())
                {
                    QStringList rowValeur;
                    for (int i = 0; i < fieldCount; ++i)
                    {
                        if (i == 0)
                            rowValeur.append("NULL");
                        else
                            rowValeur.append("'" + query.value(i).toString() + "'");
                    }
                    valeur.append("(" + rowValeur.join(", ") + ")");
                }

                insertQueryString += valeur.join(",\n ") + ";";
                fullQueryString += insertQueryString+"\n";
            }
            QSqlQuery effacerTableQuery("TRUNCATE " + table, db);
            save.creerSauvegarde(fullQueryString);
        }
    }
    return true;
}
bool cdatabase::importerDatabaseSql()
{
    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false;
    }
    csauvegarde save;
    QStringList queries = save.obtenirSauvegardes();
    qDebug() << queries;
        // Exécute chaque requête SQL
        for (const QString &query : queries)
        {
            QString trimmedQuery = query.trimmed();
            if (!trimmedQuery.isEmpty())
            {
                QSqlQuery query(db);
                if (!query.exec(trimmedQuery))
                {
                    qDebug() << "Requête SQL :" << trimmedQuery;
                    return false; // Passe à la requête suivante
                }
            }
        }

    return true;
}


