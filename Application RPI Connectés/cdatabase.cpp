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
    // Vérifie si la base de données est ouverte
    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false; // Retourne false si la base de données n'est pas ouverte
    }
    
    // Instancie un objet de la classe csauvegarde
    csauvegarde save;
    
    // Récupère la liste des tables dans la base de données
    QStringList tables = db.tables();
    
    // Initialise une chaîne de caractères pour stocker la requête SQL complète
    QString fullQueryString;
    
    // Parcourt chaque table dans la base de données
    for (const QString &table : tables)
    {
        // Vérifie si la table est "porte" ou "capteurconso"
        if (table == "porte" || table == "capteurconso")
        {
            // Exécute une requête SQL pour sélectionner toutes les lignes de la table
            QSqlQuery query("SELECT * FROM " + table, db);
            
            // Récupère les métadonnées de la requête
            QSqlRecord rec = query.record();
            int fieldCount = rec.count();

            // Vérifie s'il y a au moins une ligne dans la table
            if (query.next())
            {
                // Initialise une chaîne de caractères pour la requête d'insertion
                QString insertQueryString = "INSERT INTO " + table + " VALUES ";
                QStringList valeur;

                // Parcourt chaque ligne de résultat de la requête
                while (query.next())
                {
                    QStringList rowValeur;
                    
                    // Parcourt chaque colonne de la ligne
                    for (int i = 0; i < fieldCount; ++i)
                    {
                        // Si c'est la première colonne, ajoute NULL à la liste des valeurs
                        if (i == 0)
                            rowValeur.append("NULL");
                        else
                            // Sinon, ajoute la valeur de la colonne entourée de guillemets simples à la liste des valeurs
                            rowValeur.append("'" + query.value(i).toString() + "'");
                    }
                    
                    // Ajoute la liste des valeurs de la ligne à la liste des valeurs
                    valeur.append("(" + rowValeur.join(", ") + ")");
                }
                
                // Construit la requête d'insertion complète avec toutes les valeurs
                insertQueryString += valeur.join(",\n ") + ";";
                
                // Ajoute la requête d'insertion à la chaîne de caractères complète
                fullQueryString += insertQueryString+"\n";
            }
            
            // Exécute une requête pour vider la table
            QSqlQuery effacerTableQuery("TRUNCATE " + table, db);
            
            // Appelle la méthode creerSauvegarde de l'objet save pour sauvegarder les données
            save.creerSauvegarde(fullQueryString);
        }
    }
    
    return true; // Retourne true si l'exportation est réussie
}

bool cdatabase::importerDatabaseSql()
{
    // Vérifie si la base de données est ouverte
    if (!db.isOpen())
    {
        qDebug() << "La connexion à la base de données n'est pas ouverte.";
        return false; // Retourne false si la base de données n'est pas ouverte
    }
    
    // Instancie un objet de la classe csauvegarde
    csauvegarde save;
    
    // Récupère la liste des requêtes SQL à partir de la sauvegarde
    QStringList queries = save.obtenirSauvegardes();
    
    // Affiche les requêtes récupérées dans la console pour le débogage
    qDebug() << queries;
    
    // Exécute chaque requête SQL
    for (const QString &query : queries)
    {
        // Supprime les espaces inutiles au début et à la fin de la requête
        QString trimmedQuery = query.trimmed();
        
        // Vérifie si la requête n'est pas vide après suppression des espaces
        if (!trimmedQuery.isEmpty())
        {
            // Instancie un objet QSqlQuery pour exécuter la requête SQL
            QSqlQuery query(db);
            
            // Exécute la requête SQL
            if (!query.exec(trimmedQuery))
            {
                // Affiche la requête SQL en cas d'erreur pour le débogage
                qDebug() << "Requête SQL :" << trimmedQuery;
                return false; // Retourne false en cas d'échec d'exécution de la requête
            }
        }
    }

    return true; // Retourne true si l'importation est réussie
}


