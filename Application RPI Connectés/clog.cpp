#include "clog.h"

#include <QString>
#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>


CLog::CLog(QString FileName)
{
    this->nomFichier = FileName;

    // Créer le dossier si nécessaire
    if (!std::filesystem::exists(path))
        std::filesystem::create_directory(path);

    QDateTime heureActuel = QDateTime::currentDateTime();
    QString heureFormate = heureActuel.toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString dossierSauvegarde = QString::fromStdString(this->path.generic_string());

    QString outputPath = dossierSauvegarde + "/" + nomFichier+".log";

    QFile file(outputPath);
        if(file.open(QIODevice::Append | QIODevice::Text))
        {
             QTextStream out(&file);
             out << "\r\n";
             out << "================================================================================";
             out << "\r\n\r\n";
             out << heureFormate << " | Application | Démarré\r\n";
        }
        file.close();

}
CLog::~CLog()
{

    this->Addlog("Application", "Arrêté");
}

void CLog::Addlog(QString Event, QString Detail)
{

    QDateTime heureActuel = QDateTime::currentDateTime();
    QString heureFormate = heureActuel.toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString dossierSauvegarde = QString::fromStdString(this->path.generic_string());
    QString outpath = dossierSauvegarde + "/" + nomFichier+".log";
    QFile file(outpath);

    if(file.open(QIODevice::Append | QIODevice::Text)) {
         QTextStream out(&file);
         out << heureFormate << " | " << Event << " | " << Detail << "\r\n";
    }

    file.close();


}
