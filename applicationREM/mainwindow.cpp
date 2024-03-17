#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <winsock2.h>
#include <windows.h>
#include <QMessageBox>
#include <iostream>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <QThread>
#include <cdatabase.h>
#include <QDir>
#include <QFileInfoList>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateLabelText();
    // QTimer pour déclencher la mise à jour toutes les 5 secondes
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateLabelText);
    timer->start(5000); // 5000 millisecondes = 5 secondes
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_close_clicked()
{
    close();
}

void MainWindow::on_recup_clicked()
{
    // Ping sur la base de données Locale
    if (!this->ping("10.3.141.1"))
    {
        // Affiche un message d'erreur si on n'est pas présent dans le réseau locale
        QMessageBox::critical(this, "Erreur Réseau", "Vous n'êtes pas sur le réseau que la base de donnée locale!");
        return;
    }

    // Initialise la connexion à la base de données locale
    cdatabase db("10.3.141.1", "locale", "app", "appsql", "3306");

    // Affiche une boîte de dialogue informant de la récupération en cours
    QMessageBox msgBox;
    msgBox.setWindowTitle("Récupération en cours...");
    msgBox.setText("Récupération de la base de données en cours...");
    msgBox.setStandardButtons(QMessageBox::NoButton); // Pas de boutons standard
    msgBox.show();

    // Exportation de la BDD Locale
    if (!db.exporterDatabaseSql())
    {
        QMessageBox::critical(this, "Erreur Récupération", "La récupération a échoué");
    }
    else
    {
        QMessageBox::information(this, "Récupération terminée", "La récupération de la base de données est terminée.");
    }
}

void MainWindow::on_envoie_clicked()
{
    // Ping sur la base de donnée centrale
    if (!this->ping("192.168.21.254"))
    {
        // Message d'erreur si on ne se trouve pas dans le réseau de la Centrale
        QMessageBox::critical(this, "Erreur Réseau", "Vous n'êtes pas sur le réseau de la base de donnée centrale!");
        return;
    }
    // Initialise la connexion à la base de données locale
    cdatabase db("192.168.21.254", "centrale", "app", "appsql", "3306");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Envoie des données en cours...");
    msgBox.setText("Envois sur la base de données en cours...");
    msgBox.setStandardButtons(QMessageBox::NoButton); // Pas de boutons standard
    msgBox.show();

    // Importation de la BDD Locale sur centrale
    if (!db.envoieDatabaseSql())
    {
        QMessageBox::critical(this, "Erreur Envoie", "L'envoie des données a échoué");
    }
    else
    {
        QMessageBox::information(this, "Envoie terminé", "L'envoie des données sur la base de donnée est terminée.");
    }
}

void MainWindow::updateLabelText()
{
    QString folderPath = "D:/ETUDIANT IR2/Julien RICHARD/REM/Projet code/fichierREM/";
    QDir directory(folderPath);

    QStringList filters;
    filters << "*.sql";

    QFileInfoList fileInfoList = directory.entryInfoList(filters, QDir::Files);

    int nombreFichiers = fileInfoList.size();

    if (nombreFichiers <= 1)
        ui->label_2->setText("Envoie les données sur la base de donnée centrale \n\nFichier présent: " + QString::number(nombreFichiers));
    else
        ui->label_2->setText("Envoie les données sur la base de donnée centrale \n\nFichiers présent: " + QString::number(nombreFichiers));
}



void MainWindow::on_pushButton_clicked()
{

    QMessageBox question;
    question.setText("Voulez-vous vraiment supprimer tout les fichiers de sauvegarde ?\n\nCette action sera irréversible.");
    QPushButton *ButtonOui = question.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton *ButtonNon = question.addButton(tr("Non"), QMessageBox::NoRole);

    question.exec();

    if (question.clickedButton() == ButtonOui)
    {
        QDir directory("D:/ETUDIANT IR2/Julien RICHARD/REM/Projet code/fichierREM/");
        if (!directory.exists())
        {
            qDebug() << "Le dossier spécifié n'existe pas.";
            return;
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
            }
        }
    }
    else if (question.clickedButton() == ButtonNon)
    {
        qDebug() << "Bouton non pressé";
    }
}

bool MainWindow::ping(const char *adresseIP)
{

    // Déclaration des variables locales
    HANDLE hIcmpFile;               // Handle pour l'accès ICMP
    DWORD dwRetVal = 0;             // Valeur de retour pour les fonctions ICMP
    char SendData[32] = "PingTest"; // Données à envoyer dans le paquet ping
    LPVOID ReplyBuffer = NULL;      // Tampon de réponse ICMP
    DWORD ReplySize = 0;            // Taille de la réponse

    sockaddr_in saGNI;                       // Structure pour stocker l'adresse IP à pinger
    ZeroMemory(&saGNI, sizeof(sockaddr_in)); // Initialisation de la structure à zéro
    saGNI.sin_family = AF_INET;              // Famille d'adresses internet

    // Convertir l'adresse IP au format binaire
    if (inet_pton(AF_INET, adresseIP, &(saGNI.sin_addr)) != 1)
    {
        std::cerr << "Adresse IP non valide : " << adresseIP << std::endl;
        return false; // Retourne false si l'adresse IP est invalide
    }

    // Créer un handle pour les opérations ICMP
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Erreur lors de la création du handle ICMP." << std::endl;
        return false; // Retourne false en cas d'échec de création du handle
    }

    // Calcule la taille du tampon de réponse
    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);

    // Alloue de la mémoire pour le tampon de réponse
    ReplyBuffer = (VOID *)malloc(ReplySize);
    if (ReplyBuffer == NULL)
    {
        std::cerr << "Erreur lors de l'allocation de mémoire pour la réponse ICMP." << std::endl;
        return false; 
    }

    // Envoi un paquet ping et recevoir la réponse
    dwRetVal = IcmpSendEcho(hIcmpFile, saGNI.sin_addr.S_un.S_addr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
    if (dwRetVal != 0)
    {
        // Extraction des informations de réponse
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        std::cout << "Ping réussi. Temps de réponse : " << pEchoReply->RoundTripTime << "ms" << std::endl;
        return true; // Retourne true si le ping est réussi
    }
    else
    {
        std::cerr << "Erreur lors de l'envoi du ping. Code d'erreur : " << GetLastError() << std::endl;
        return false; // Retourne false si l'envoi du ping échoue
    }

    // Libère la mémoire et ferme le handle
    free(ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
}