#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <windows.h>
#include <QMessageBox>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Cache la barre de progression au lancement du programme
        ui->progressBar->hide();
        updateLabelText();
        // Créer un QTimer pour déclencher la mise à jour toutes les 5 secondes
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
    if (!this->ping("192.168.0.1"))
    {
        QMessageBox::critical(this, "Erreur Réseau", "Vous n'êtes pas sur le réseau que la base de donnée locale!");
        return;
    }
}

void MainWindow::on_envoie_clicked()
{
    // Ping sur la base de donnée centrale
    if (!this->ping("192.168.21.254"))
    {
        // Message d'erreur si on ne se trouve pas dans le réseau de la Centrale
        QMessageBox::critical(this, "Erreur Réseau","Vous n'êtes pas sur le réseau de la base de donnée centrale!");
        return;
    }
}


// Fonction pour mettre à jour le texte du QLabel
void MainWindow::updateLabelText()
{
    // Générer un nombre aléatoire
    int randomNumber = rand() % 50;

    // Mettre à jour le texte avec le nombre aléatoire
    ui->label_2->setText("Envoie les données sur la base de donnée centrale \n\nFichier(s) présent: "+QString::number(randomNumber));
}

bool MainWindow::ping(const char* adresseIP) {

    HANDLE hIcmpFile;
      DWORD dwRetVal = 0;
      char SendData[32] = "PingTest";
      LPVOID ReplyBuffer = NULL;
      DWORD ReplySize = 0;

      sockaddr_in saGNI;
      ZeroMemory(&saGNI, sizeof(sockaddr_in));
      saGNI.sin_family = AF_INET;

      if (inet_pton(AF_INET, adresseIP, &(saGNI.sin_addr)) != 1) {
          std::cerr << "Adresse IP non valide : " << adresseIP << std::endl;
          return false;
      }

      hIcmpFile = IcmpCreateFile();
      if (hIcmpFile == INVALID_HANDLE_VALUE) {
          std::cerr << "Erreur lors de la création du handle ICMP." << std::endl;
          return false;
      }

      ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
      ReplyBuffer = (VOID*) malloc(ReplySize);
      if (ReplyBuffer == NULL) {
          std::cerr << "Erreur lors de l'allocation de mémoire pour la réponse ICMP." << std::endl;
          return false;
      }

      dwRetVal = IcmpSendEcho(hIcmpFile, saGNI.sin_addr.S_un.S_addr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
      if (dwRetVal != 0) {
          PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
          struct in_addr ReplyAddr;
          ReplyAddr.S_un.S_addr = pEchoReply->Address;
          std::cout << "Ping reussi. Temps de reponse : " << pEchoReply->RoundTripTime << "ms" << std::endl;
          return true;
      } else {
          std::cerr << "Erreur lors de l'envoi du ping. Code d'erreur : " << GetLastError() << std::endl;
          return false;
      }

      free(ReplyBuffer);
      IcmpCloseHandle(hIcmpFile);
  }




