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
#include <cdatabase.h>
#include <QDir>
#include <csauvegarde.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    updateLabelText();

    // QTimer pour déclencher la mise à jour toutes les 5 secondes
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateLabelText);
    timer->start(5 * 1000); // 1000 millisecondes = 1 seconde
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
    if (!ping.ping("10.3.141.1"))
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
    if (!ping.ping("192.168.21.254"))
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
    if (!db.importerDatabaseSql())
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
    csauvegarde save;

    int nombreFichiers = save.getNombreFichiers();

    if (nombreFichiers <= 1)
        ui->fichierPresent->setText("Fichier présent: " + QString::number(nombreFichiers));
    else
        ui->fichierPresent->setText("Fichiers présents: " + QString::number(nombreFichiers));
}



void MainWindow::on_effacerSauvegarde_clicked()
{

    QMessageBox question;
    question.setText("Voulez-vous vraiment supprimer tout les fichiers de sauvegarde ?\n\nCette action sera irréversible.");
    QPushButton *ButtonOui = question.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton *ButtonNon = question.addButton(tr("Non"), QMessageBox::NoRole);

    question.exec();

    if (question.clickedButton() == ButtonOui)
    {

      if (!save.supprimerSauvegarde())
      {
          QMessageBox::critical(this, "Erreur Suppression Sauvegarde", "La suppression des fichiers de sauvegardes a échoué");
          return;

      }

    }
    else if (question.clickedButton() == ButtonNon)
    {
        qDebug() << "Bouton non pressé";
    }
}



