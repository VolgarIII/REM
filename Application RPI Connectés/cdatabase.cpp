#include "cdatabase.h"
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

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString nomFichier = "recup_" + timestamp + ".sql";
    // Convertir cheminDossierSauvegarde en QString
    QString dossierSauvegarde = QString::fromStdString(this->cheminDossierSauvegarde.generic_string());

    // Créer le chemin complet du fichier de sortie
    QString outputPath = dossierSauvegarde + "/" + nomFichier;

    QFile outputFile(outputPath);

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Impossible d'ouvrir le fichier de sortie.";
        return false;
    }

    QTextStream out(&outputFile);

    QStringList tables = db.tables();
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
                out << insertQueryString << "\n";
            }
            QSqlQuery effacerTableQuery("TRUNCATE " + table, db);
        }
    }
    outputFile.close();
    return true;
}

bool cdatabase::importerDatabaseSql()
{
    if (!db.isOpen())
    {
        qDebug() << db;
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false;
    }
    QDir directory(this->cheminDossierSauvegarde);
    if (!directory.exists())
    {
        qDebug() << "Le dossier spécifié n'existe pas.";
        return false;
    }

    // Filtre les fichiers dans le dossier
    QStringList filters;
    filters << "*.sql";

    // Liste les fichiers dans le dossier qui correspondent aux filtres
    QStringList fileList = directory.entryList(filters, QDir::Files);

    if (fileList.size() == 0)
    {
        qDebug() << "Aucun fichier n'est présent dans le dossier de sauvegarde";
        return false;
    }
    // Parcoure tous les fichiers
    for (const QString &fileName : fileList)
    {
        QFile file(directory.filePath(fileName));

        // Ouvre le fichier en mode lecture
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Impossible d'ouvrir le fichier :" << file.fileName();
            return false; // Passe au fichier suivant
        }

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        // Sépare les requêtes SQL
        QStringList queries = fileContent.split(QRegularExpression(";\\s*(?:\n|\r\n|\r)?"));

        // Exécute chaque requête SQL
        for (const QString &query : queries)
        {
            QString trimmedQuery = query.trimmed();
            if (!trimmedQuery.isEmpty())
            {
                QSqlQuery query(db);
                if (!query.exec(trimmedQuery))
                {
                    qDebug() << "Erreur lors de l'exécution de la requête SQL dans le fichier :" << file.fileName();
                    qDebug() << "Requête SQL :" << trimmedQuery;
                    return false; // Passe à la requête suivante
                }
            }
        }

        if (!file.remove())
        {
            qDebug() << "Impossible de supprimer le fichier :" << file.fileName();
        }
    }

    return true;
}


