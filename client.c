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
    if (iResult != 0) {
        printf("Echec de l'initialisation WSAStartup : %d\n", iResult);
        return 1;
    }

    int sock_fd;
    struct sockaddr_in server_addr;

    // Les 13 octets observés dans Wireshark (en hexadécimal) pour fermer un volet
    unsigned char payload[] = {
        0xc0, 0xa8, 0x00, 0x71, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x01, 0x01, 0x01, 0x00
    };

    //Recuperation du port du serveur avec une commande powershell
    char portStr[16] = {0};
    char cmd[] =
        "powershell -NoProfile -Command \""
        "netstat -ano | Select-String 'ESTABLISHED' | "
        "Where-Object { $_.Line -match '^\\s*TCP\\s+127\\.0\\.0\\.1' } | "
        "Select-Object -First 1 | "
        "ForEach-Object { ($_.Line -split '\\s+')[2] -replace '.*:','' }"
        "\"";

    FILE *fp = _popen(cmd, "r");

    if (fgets(portStr, sizeof(portStr), fp) != NULL) {
        /* Suppression du saut de ligne final */
        portStr[strcspn(portStr, "\r\n")] = '\0';
    } else {
        printf("Aucun resultat\n");
    }

    //Je défini la variable port de type integer cette fois-ci
    char *end;
    long port = strtol(portStr, &end, 10);
    printf("%ld\n", port);
    _pclose(fp);
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
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.28");

    // 3. Connexion au serveur
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Echec de la connexion au serveur");
        close(sock_fd);
        return 1;
    }
    printf("Connection au serveur avec succes !\n");

    // 4. Envoi des 13 octets de la trame
    ssize_t bytes_sent = send(sock_fd, payload, sizeof(payload), 0);
    if (bytes_sent < 0) {
        perror("Erreur lors de l'envoi des donnees");
    } else {
        printf("Succes : %ld octets envoyes (correspondant a la trame Wireshark).\n", bytes_sent);
    }

    // 5. Fermeture de la socket
    close(sock_fd);
    return 0;
}