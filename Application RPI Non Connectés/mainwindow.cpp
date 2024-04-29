#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

#include <QMessageBox>
#include <iostream>
#include <cdatabase.h>
#include <QDir>
#include "clog.h"
#include <csauvegarde.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
    , log("log") // Initialisation de CLog avec un nom de fichier
{
    ui->setupUi(this);
    updateLabelText();

    // Initialisation du journal avec un message
    log.Addlog("Initialisation", "La fenêtre principale a été initialisée avec succès.");

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
    log.Addlog("Action", "Fermeture de l'application");
    close();
}

void MainWindow::on_recup_clicked()
{
    log.Addlog("Action", "Clic sur le bouton 'Récupération'");
    // Ping sur la base de données Locale
    if (!ping.ping("10.3.141.1"))
    {
        // Affiche un message d'erreur si on n'est pas présent dans le réseau local
        QMessageBox::critical(this, "Erreur Réseau", "Vous n'êtes pas sur le réseau que la base de donnée locale!");
        log.Addlog("Erreur", "Échec du ping sur la base de données locale.");
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
        log.Addlog("Erreur", "Échec de la récupération de la base de données.");
    }
    else
    {
        QMessageBox::information(this, "Récupération terminée", "La récupération de la base de données est terminée.");
        log.Addlog("Succès", "La récupération de la base de données est terminée avec succès.");
    }
}

void MainWindow::on_envoie_clicked()
{
    log.Addlog("Action", "Clic sur le bouton 'Envoi'");

    if (save.getNombreFichiers() == 0)
    {
        // Message d'erreur si on ne se trouve pas dans le réseau de la Centrale
        QMessageBox::critical(this, "Erreur Fichier", "Aucun fichier de sauvegarde n'est présent");
        log.Addlog("Erreur", "Aucun fichier de sauvegarde n'est présent.");
        return;
    }
    // Ping sur la base de donnée centrale
    if (!ping.ping("192.168.21.254"))
    {
        // Message d'erreur si on ne se trouve pas dans le réseau de la Centrale
        QMessageBox::critical(this, "Erreur Réseau", "Vous n'êtes pas sur le réseau de la base de donnée centrale!");
        log.Addlog("Erreur", "Échec du ping sur la base de données centrale.");
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
        log.Addlog("Erreur", "Échec de l'envoi des données.");
    }
    else
    {
        QMessageBox::information(this, "Envoie terminé", "L'envoie des données sur la base de donnée est terminée.");
        log.Addlog("Succès", "L'envoi des données sur la base de donnée est terminée avec succès.");
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
    log.Addlog("Action", "Clic sur le bouton 'Effacer Sauvegarde'");

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
            log.Addlog("Erreur", "Échec de la suppression des fichiers de sauvegarde.");
            return;
        }
    }
    else if (question.clickedButton() == ButtonNon)
    {
        qDebug() << "Bouton non pressé";
    }
}
