#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

    // Mets bien le port ACTUEL de SimDom (regarde la barre du bas)
    int port = 61759; 

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("1. Tentative de connexion sur 127.0.0.1:%d...\n", port);
    if (connect(s, (struct sockaddr*)&target, sizeof(target)) != 0) {
        printf("-> ERREUR: Impossible de joindre le port %d. Code erreur: %d\n", port, WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    printf("2. Connecte ! Envoi de la trame binaire...\n");
    unsigned char trame[] = { 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01 };
    int res = send(s, (const char*)trame, sizeof(trame), 0);

    printf("3. Resultat send: %d octets emis.\n", res);

    // On garde la socket ouverte 5 secondes pour laisser le temps au serveur
    Sleep(5000);

    closesocket(s);
    WSACleanup();
    printf("4. Fin du test.\n");
    return 0;
}