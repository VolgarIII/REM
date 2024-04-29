#include "qdir.h"
#include "qregularexpression.h"
#include "qsqlquery.h"

int main()
{
    QStringList allQueries;
    QDir directory(std::filesystem::current_path()/ "dossierREM");
    if (!directory.exists())
    {
        qDebug() << "Le dossier spécifié n'existe pas.";
        return -1;
    }

    // Filtre les fichiers dans le dossier
    QStringList filters;
    filters << "*.sql";

    // Liste les fichiers dans le dossier qui correspondent aux filtres
    QStringList fileList = directory.entryList(filters, QDir::Files);

    if (fileList.size() == 0)
    {
        qDebug() << "Aucun fichier n'est présent dans le dossier de sauvegarde";
        return -1;
    }
    // Parcoure tous les fichiers
    for (const QString &fileName : fileList)
    {
        QFile file(directory.filePath(fileName));

        // Ouvre le fichier en mode lecture
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Impossible d'ouvrir le fichier :" << file.fileName();
            return -1; // Passe au fichier suivant
        }

        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        // Sépare les requêtes SQL
        QStringList queries = fileContent.split(QRegularExpression(";\\s*(?:\n|\r\n|\r)?"));
        allQueries << queries;

    }
    qDebug() << allQueries;
    qDebug() << allQueries.length();
    return 0;
}
