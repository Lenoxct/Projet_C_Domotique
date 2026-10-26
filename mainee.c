#include "webview/webview.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

// Variable globale pour garder la connexion socket ouverte
SOCKET g_sock = INVALID_SOCKET;

// Initialiser la connexion au simulateur
int connecter_simdom(const char* ip, int port) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 0;

    g_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (g_sock == INVALID_SOCKET) {
        WSACleanup();
        return 0;
    }

    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = inet_addr(ip);

    if (connect(g_sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
        closesocket(g_sock);
        g_sock = INVALID_SOCKET;
        WSACleanup();
        return 0;
    }
    return 1;
}

// Fonction C appelée directement depuis le JavaScript de ta page HTML
// seq: identifiant de la requête JS, req: arguments envoyés depuis JS (au format JSON)
void cb_controler_equipement(const char *seq, const char *req, void *arg) {
    (void)arg;

    // Exemple : req recevra un argument JSON comme "[1]" pour allumer ou "[0]" pour éteindre
    int etat = 0;
    if (strstr(req, "1") != NULL) {
        etat = 1;
    }

    if (g_sock != INVALID_SOCKET) {
        // exemple d'une trame binaire identifiée sur Wireshark :
        unsigned char trame[8] = { 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, (unsigned char)etat };
        send(g_sock, (const char*)trame, sizeof(trame), 0);
    }

    // Réponse envoyée au JavaScript (promesse résolue)
    webview_return((webview_t)arg, seq, 0, "{\"status\": \"ok\"}");
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInst; (void)hPrevInst; (void)lpCmdLine; (void)nCmdShow;
#else
int main(void) {
#endif

    // 1. Connexion au simulateur SimDom PENSER A REMPLACER LE NUMERO DE PORT PAR CELUI DU SIM 
    connecter_simdom("127.0.0.1", 61759);

    // 2. Initialisation Webview
    webview_t w = webview_create(0, NULL);
    webview_set_title(w, "Logiciel domotique");
    webview_set_size(w, 1000, 600, WEBVIEW_HINT_NONE);

    // 3. Liaison de la fonction C au JavaScript : window.controlerEquipement(etat)
    webview_bind(w, "controlerEquipement", cb_controler_equipement, w);

    // 4. Chargement de l'IHM
    webview_navigate(w, "file:///C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/index.html");

    // 5. Boucle d'événements
    webview_run(w);

    // 6. Nettoyage
    if (g_sock != INVALID_SOCKET) {
        closesocket(g_sock);
        WSACleanup();
    }
    webview_destroy(w);
    return 0;
}