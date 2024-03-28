#include <QCoreApplication>
#include "cdatabase.h"
#include <iostream>
#include <QString>
using namespace std;
int main()
{
    std::filesystem::path cheminFichierREM = std::filesystem::current_path()/ "dossierREM";

    // Créer le dossier si nécessaire
    if (!std::filesystem::exists(cheminFichierREM))
        std::filesystem::create_directory(cheminFichierREM);


    cdatabase *dbCentrale = new cdatabase("192.168.21.254", "centrale", "app", "appsql", "3306");
    QStringList str = dbCentrale->getListeRPIConnectes();

    delete dbCentrale;
    for (const QString &adresseIP : str)
    {
        qDebug() << "Connexion à " << adresseIP;
        cdatabase *dbLocale= new cdatabase(adresseIP, "locale", "app", "appsql", "3306");

        // Exportation de la BDD Locale
        if (!dbLocale->exporterDatabaseSql())
        {
            qDebug() <<"La récupération a échoué";
        }
        else
        {
            qDebug() << "La récupération de la base de données est terminée.";
        }
        delete dbLocale;

    }
    dbCentrale = new cdatabase("192.168.21.254", "centrale", "app", "appsql", "3306");

    // Importation de la BDD Locale sur centrale
    if (!dbCentrale->importerDatabaseSql())
    {
        qDebug() << "L'envoie des données a échoué";
    }
    else
    {
        qDebug() <<  "L'envoie des données sur la base de donnée est terminée.";
    }
    delete dbCentrale;

    return 0;
}
