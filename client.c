#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {
    // 1. INITIALISATION DE WINSOCK (OBLIGATOIRE SOUS WINDOWS)
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    int sock_fd;
    struct sockaddr_in server_addr;

    // Les 13 octets pour fermer un volet
    unsigned char payload[] = {
        0xc0, 0xa8, 0x00, 0x71, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x01, 0x01, 0x01, 0x00
    };

    
    // Commande PowerShell combinée pour récupérer le port d'écoute de javaw
    char portStr[16] = {0};
    char cmd[] =
        "powershell -NoProfile -Command \""
        "$p = (Get-Process javaw -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Id -First 1); "
        "if ($p) { (Get-NetTCPConnection -State Listen -OwningProcess $p -ErrorAction SilentlyContinue | Select-Object -ExpandProperty LocalPort -First 1) }"
        "\"";

    FILE *fp = _popen(cmd, "r");

    if (fgets(portStr, sizeof(portStr), fp) != NULL) {
        /* Suppression du saut de ligne final renvoyé par PowerShell */
        portStr[strcspn(portStr, "\r\n")] = '\0';
    }else {
        printf("Aucun resultat\n");
    }
    _pclose(fp);

    // Conversion de la chaîne de caractères en variable de type 'int' (via long)
    char *end;
    long port_long = strtol(portStr, &end, 10);

    int port = (int)port_long;
    // Conversion de la chaîne IP en entier 32-bit réseau
    unsigned long ip = inet_addr("127.0.0.1");

    ////////////////////////////
    // 1. Création de la socket TCP
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Erreur creation socket");
        return 1;
    }

    // 2. Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connexion au serveur
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Echec de la connexion au serveur");
        closesocket(sock_fd);
        return 1;
    }
    printf("Connection au serveur avec succes !\n");

    // 4. Envoi des 13 octets de la trame
    ssize_t bytes_sent = send(sock_fd, payload, sizeof(payload), 0);
    if (bytes_sent < 0) {
        perror("Erreur lors de l'envoi des donnees");
    } else {
        printf("Succes : %ld octets envoyes.\n", bytes_sent);
    }

    // 5. Fermeture de la socket
    closesocket(sock_fd);
    WSACleanup();
    return 0;
}