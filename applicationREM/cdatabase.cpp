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

cdatabase::cdatabase(QString IP, QString Db, QString username, QString password, QString Port)
{
    db = QSqlDatabase::addDatabase("QODBC");

    QString dsn = "DRIVER={MySQL ODBC 8.3 Unicode Driver};SERVER=" + IP + ";PORT=" + Port + ";DATABASE=" + Db + ";UID=" + username + ";PWD=" + password + ";TDS_Version=4.2;";
    db.setDatabaseName(dsn);

    if (!db.open())
    {
        qDebug() << "La connexion a la bdd a echoue...";
    }
    else
        qDebug() << "La connexion a reussi";
}

cdatabase::~cdatabase()
{
    qDebug() << "Fermeture de la connexion...";
    db.close();
    QSqlDatabase::removeDatabase("QODBC")
}

bool cdatabase::exporterDatabaseSql()
{

    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString outputPath = "recup_" + timestamp + ".sql";
    QFile outputFile("D:/ETUDIANT IR2/Julien RICHARD/REM/Projet code/fichierREM/" + outputPath);

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

bool cdatabase::envoieDatabaseSql()
{
    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false;
    }
    QDir directory("D:/ETUDIANT IR2/Julien RICHARD/REM/Projet code/fichierREM/");
    if (!directory.exists())
    {
        qDebug() << "Le dossier spécifié n'existe pas.";
        return false;
    }

    // Filtre les fichiers dans le dossier
    QStringList filters;
    filters << "*.sql";

    // Lister les fichiers dans le dossier qui correspondent aux filtres
    QStringList fileList = directory.entryList(filters, QDir::Files);

    if (fileList.size() == 0)
    {
        qDebug() << "Aucun fichier n'est présent dans le dossier de sauvegarde";
        return false;
    }
    // Parcourir tous les fichiers
    for (const QString &fileName : fileList)
    {
        QFile file(directory.filePath(fileName));

        // Ouvrir le fichier en mode lecture
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Impossible d'ouvrir le fichier :" << file.fileName();
            return false; // Passer au fichier suivant
        }

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        // Séparer les requêtes SQL
        QStringList queries = fileContent.split(QRegularExpression(";\\s*(?:\n|\r\n|\r)?"));

        // Exécuter chaque requête SQL
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
                    return false; // Passer à la requête suivante
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
