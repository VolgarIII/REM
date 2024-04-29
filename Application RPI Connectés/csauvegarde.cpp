#include "csauvegarde.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "qregularexpression.h"
using namespace std;

csauvegarde::csauvegarde()
{
    // Créer le dossier si nécessaire pour la sauvegarde des fichiers
    if (!std::filesystem::exists(cheminDossierREM))
        std::filesystem::create_directory(cheminDossierREM);


}

int  csauvegarde::getNombreFichiers()
{
    QDir directory(this->cheminDossierREM);

    QStringList filters;
    filters << "*.sql";

    QFileInfoList fileInfoList = directory.entryInfoList(filters, QDir::Files);

    return fileInfoList.size();
}

bool csauvegarde::creerSauvegarde(QString ligne)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString nomFichier = "recup_" + timestamp + ".sql";
    // Convertir cheminDossierSauvegarde en QString
    QString dossierSauvegarde = QString::fromStdString(this->cheminDossierREM.generic_string());

    // Créer le chemin complet du fichier de sortie
    QString outputPath = dossierSauvegarde + "/" + nomFichier;

    QFile outputFile(outputPath);

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Impossible d'ouvrir le fichier de sortie.";
        return false;
    }

    QTextStream out(&outputFile);

    out << ligne << "\n";
    outputFile.close();

    return true;
}
QStringList csauvegarde::obtenirSauvegardes()
{
    QStringList allQueries;
    QDir directory(std::filesystem::current_path()/ "dossierREM");
    if (!directory.exists())
    {
        qDebug() << "Le dossier spécifié n'existe pas.";
        return allQueries;
    }

    // Filtre les fichiers dans le dossier
    QStringList filters;
    filters << "*.sql";

    // Liste les fichiers dans le dossier qui correspondent aux filtres
    QStringList fileList = directory.entryList(filters, QDir::Files);

    if (fileList.size() == 0)
    {
        qDebug() << "Aucun fichier n'est présent dans le dossier de sauvegarde";
        return allQueries;
    }
    qDebug() << fileList.size();
    // Parcoure tous les fichiers
    for (const QString &fileName : fileList)
    {
        QFile file(directory.filePath(fileName));

        // Ouvre le fichier en mode lecture
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Impossible d'ouvrir le fichier :" << file.fileName();
            return allQueries; // Passe au fichier suivant
        }

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        // Sépare les requêtes SQL
        QStringList queries = fileContent.split(QRegularExpression(";\\s*(?:\n|\r\n|\r)?"));
        allQueries << queries;

    }
    return allQueries;
}
bool csauvegarde::supprimerSauvegarde()
{
    QDir directory(this->cheminDossierREM);
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

           // Parcourir tous les fichiers
           for (const QString &fileName : fileList)
           {
               QFile file(directory.filePath(fileName));
               if (!file.remove())
               {
                   qDebug() << "Impossible de supprimer le fichier :" << file.fileName();
                   return false;
               }
           }
           return true;
}

std::filesystem::path csauvegarde::getDossierREM()
{
    return this->cheminDossierREM;
}
