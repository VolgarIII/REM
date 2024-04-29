#include "cping.h"
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


#pragma comment(lib, "iphlpapi.lib");
#pragma comment(lib, "ws2_32.lib");
using namespace std;

bool cping::ping(const char *adresseIP)
{

    // Déclaration des variables locales
    HANDLE hIcmpFile;                   // Handle pour l'accès ICMP
    DWORD dwRetVal = 0;                 // Valeur de retour pour les fonctions ICMP
    char SendData[32] = "Test de Ping"; // Données à envoyer dans le paquet ping
    LPVOID ReplyBuffer = NULL;          // Tampon de réponse ICMP
    DWORD ReplySize = 0;                // Taille de la réponse

    sockaddr_in saGNI;                       // Structure pour stocker l'adresse IP à pinger
    ZeroMemory(&saGNI, sizeof(sockaddr_in)); // Initialisation de la structure à zéro
    saGNI.sin_family = AF_INET;              // Famille d'adresses internet

    // Convertir l'adresse IP au format binaire
    if (inet_pton(AF_INET, adresseIP, &(saGNI.sin_addr)) != 1)
    {
       cerr << "Adresse IP non valide : " << adresseIP <<endl;
        return false; // Retourne false si l'adresse IP est invalide
    }

    // Créer un handle pour les opérations ICMP
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
    {
       cerr << "Erreur lors de la création du handle ICMP." <<endl;
        return false; // Retourne false en cas d'échec de création du handle
    }

    // Calcule la taille du tampon de réponse
    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);

    // Alloue de la mémoire pour le tampon de réponse
    ReplyBuffer = (VOID *)malloc(ReplySize);
    if (ReplyBuffer == NULL)
    {
       cerr << "Erreur lors de l'allocation de mémoire pour la réponse ICMP." <<endl;
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
       cout << "Ping reussi. Temps de reponse : " << pEchoReply->RoundTripTime << "ms" <<endl;
        return true; // Retourne true si le ping est réussi
    }
    else
    {
        qDebug() << GetLastError();
       cerr << "Erreur lors de l'envoi du ping. Code d'erreur : " << GetLastError() <<endl;
        return false; // Retourne false si l'envoi du ping échoue
    }

    // Libère la mémoire et ferme le handle
    free(ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
}
