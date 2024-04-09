#include <QCoreApplication>
#include "cdatabase.h"
#include "clog.h"
#include <iostream>
#include <QString>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CLog log("log");

    std::filesystem::path cheminFichierREM = std::filesystem::current_path()/ "dossierREM";

    // Créer le dossier si nécessaire
    if (!std::filesystem::exists(cheminFichierREM))
        std::filesystem::create_directory(cheminFichierREM);

    // Crée une nouvelle instance de cdatabase pour la base de données centrale.
    cdatabase *dbCentrale = new cdatabase("192.168.21.254", "centrale", "app", "appsql", "3306");
    log.Addlog("Centrale","Connexion à 192.168.21.254...");

    QStringList str = dbCentrale->getListeRPIConnectes(); // Récupère une liste d'adresses IP des Raspberry Pi connectés à la base de données centrale.
    log.Addlog("Centrale","Trouvé "+QString::number(str.count())+" Raspberry dans la BDD");

    log.Addlog("Centrale","Déconnexion de 192.168.21.254...");
    delete dbCentrale; // Libère la mémoire allouée pour l'objet dbCentrale.

    /*  Cette boucle parcourt chaque adresse IP dans la liste 'str'
     *
     *  Qui contient les adresses IP des Raspberry Pi connectés à la base de données centrale.
     *
     */
    for (const QString &adresseIP : str)
    {
        qDebug() << "Connexion à " << adresseIP;
        log.Addlog("Récupération","Connexion à "+adresseIP+"...");

        // Crée une nouvelle instance de cdatabase pour la base de données locale correspondant à une adresse IP spécifique.
        cdatabase *dbLocale = new cdatabase(adresseIP, "locale", "app", "appsql", "3306");

        // Exportation de la BDD Locale
        if (!dbLocale->exporterDatabaseSql()) // Vérifie si l'exportation de la base de données locale a échoué.
        {
            qDebug() <<"La récupération a échoué";
            log.Addlog("Récupération","Connexion à "+adresseIP+"...");
        }
        else
        {
            qDebug() << "La récupération de la base de donnée est terminée.";
            log.Addlog("Récupération","La récupération de la base de donnée est terminée");
        }
        log.Addlog("Récupération","Déconnexion de "+adresseIP+"...");
        delete dbLocale; // Libère la mémoire allouée pour l'objet dbLocale.

    }
    dbCentrale = new cdatabase("192.168.21.254", "centrale", "app", "appsql", "3306");
    log.Addlog("Centrale","Reconnexion à 192.168.21.254...");

    // Importation de la BDD Locale sur centrale
    if (!dbCentrale->importerDatabaseSql()) // Vérifie si l'importation des données dans la base de données centrale a échoué.
    {
        qDebug() << "L'envoie des données a échoué"; // Affiche un message d'erreur si l'importation a échoué.
        log.Addlog("Envoie","L'envoie des données a échoué");
    }
    else
    {
        qDebug() << "L'envoie des données sur la base de donnée est terminée."; // Affiche un message de succès si l'importation est réussie.
        log.Addlog("Envoie Centrale", "L'envoie des données sur la base de donnée est terminée.");
    }

    log.Addlog("Centrale","Déconnexion de 192.168.21.254...");
    delete dbCentrale; // Libère la mémoire allouée pour l'objet dbCentrale.

    return 0;// a.exec();
}


