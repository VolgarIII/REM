#ifndef CSAUVEGARDE_H
#define CSAUVEGARDE_H

#include <filesystem>
#include <QString>

class csauvegarde
{
    std::filesystem::path cheminDossierREM = std::filesystem::current_path()/ "dossierREM";

public:
    csauvegarde();

    int  getNombreFichiers();
    bool creerSauvegarde(QString ligne);
    QStringList obtenirSauvegardes();

    bool supprimerSauvegarde();

    std::filesystem::path getDossierREM();

};

#endif // CSAUVEGARDE_H
