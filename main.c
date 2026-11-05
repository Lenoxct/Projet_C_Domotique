#include "webview/webview.h"
#include "database.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif
#include <limits.h>

typedef struct {
  webview_t window;
  database_t *database;
  int server_connected;
  char server_host[16];
  int server_port;
} app_state_t;

static const char *const device_tables[] = {"climatisations", "luminaires",
                                             "prises", "volets"};

static int make_path(char *path, size_t path_size, const char *directory,
                     const char *filename) {
  int length = snprintf(path, path_size, "%s\\%s", directory, filename);
  return length >= 0 && (size_t)length < path_size;
}

static void report_binding_error(webview_t window, const char *id,
                                 const char *message) {
  char result[1100];
  size_t used = 0;

  fprintf(stderr, "%s\n", message);
  result[used++] = '"';
  while (*message != '\0' && used + 7 < sizeof(result)) {
    unsigned char c = (unsigned char)*message++;
    if (c == '"' || c == '\\') {
      result[used++] = '\\';
      result[used++] = (char)c;
    } else if (c == '\n') {
      result[used++] = '\\';
      result[used++] = 'n';
    } else if (c == '\r') {
      result[used++] = '\\';
      result[used++] = 'r';
    } else if (c == '\t') {
      result[used++] = '\\';
      result[used++] = 't';
    } else if (c < 0x20) {
      int count = snprintf(result + used, sizeof(result) - used, "\\u%04x", c);
      if (count < 0 || (size_t)count >= sizeof(result) - used) {
        return;
      }
      used += (size_t)count;
    } else {
      result[used++] = (char)c;
    }
  }
  result[used++] = '"';
  result[used] = '\0';
  if (webview_return(window, id, 1, result) != WEBVIEW_ERROR_OK) {
    fprintf(stderr, "Impossible de transmettre l'erreur à l'interface.\n");
  }
}

static const char *skip_json_whitespace(const char *text) {
  while (isspace((unsigned char)*text)) {
    text++;
  }
  return text;
}

static int parse_json_integer(const char **text, int *value) {
  char *end;
  long parsed;

  *text = skip_json_whitespace(*text);
  errno = 0;
  parsed = strtol(*text, &end, 10);
  if (end == *text || errno == ERANGE || parsed < INT_MIN ||
      parsed > INT_MAX) {
    return 0;
  }
  *text = end;
  *value = (int)parsed;
  return 1;
}

static int parse_device_request(const char *request, int *type, int *device_id,
                                int *is_on) {
  const char *text = skip_json_whitespace(request);

  if (*text++ != '[' || !parse_json_integer(&text, type)) {
    return 0;
  }
  text = skip_json_whitespace(text);
  if (*text++ != ',' || !parse_json_integer(&text, device_id)) {
    return 0;
  }
  text = skip_json_whitespace(text);
  if (*text++ != ',' || !parse_json_integer(&text, is_on)) {
    return 0;
  }
  text = skip_json_whitespace(text);
  return *text++ == ']' && *skip_json_whitespace(text) == '\0';
}

static int parse_server_request(const char *request, char *host,
                               size_t host_size, int *port) {
  const char *text = skip_json_whitespace(request);
  size_t host_length = 0;

  if (*text++ != '[') {
    return 0;
  }
  text = skip_json_whitespace(text);
  if (*text++ != '"') {
    return 0;
  }
  while (*text != '"' && *text != '\0') {
    if ((!isdigit((unsigned char)*text) && *text != '.') ||
        host_length + 1 >= host_size) {
      return 0;
    }
    host[host_length++] = *text++;
  }
  if (*text++ != '"' || host_length == 0) {
    return 0;
  }
  host[host_length] = '\0';
  text = skip_json_whitespace(text);
  if (*text++ != ',' || !parse_json_integer(&text, port)) {
    return 0;
  }
  text = skip_json_whitespace(text);
  return *text++ == ']' && *skip_json_whitespace(text) == '\0';
}

static SOCKET open_server_socket(const char *host, int port, char *error,
                                 size_t error_size) {
  struct sockaddr_in address;
  fd_set writable;
  struct timeval connect_timeout;
  SOCKET server_socket;
  DWORD timeout = 3000;
  u_long nonblocking = 1;
  int connect_result;
  int socket_error = 0;
  int socket_error_size = sizeof(socket_error);

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons((u_short)port);
  if (InetPtonA(AF_INET, host, &address.sin_addr) != 1) {
    snprintf(error, error_size, "L'adresse IPv4 saisie n'est pas valide.");
    return INVALID_SOCKET;
  }
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket == INVALID_SOCKET) {
    snprintf(error, error_size, "Création de la connexion TCP impossible (%d).",
             WSAGetLastError());
    return INVALID_SOCKET;
  }
  if (ioctlsocket(server_socket, FIONBIO, &nonblocking) == SOCKET_ERROR) {
    socket_error = WSAGetLastError();
    closesocket(server_socket);
    snprintf(error, error_size,
             "Configuration de la connexion TCP impossible (%d).", socket_error);
    return INVALID_SOCKET;
  }
  connect_result =
      connect(server_socket, (struct sockaddr *)&address, sizeof(address));
  if (connect_result == SOCKET_ERROR) {
    socket_error = WSAGetLastError();
    if (socket_error != WSAEWOULDBLOCK && socket_error != WSAEINPROGRESS) {
      closesocket(server_socket);
      snprintf(error, error_size,
               "Connexion à %s:%d impossible (erreur réseau %d).", host, port,
               socket_error);
      return INVALID_SOCKET;
    }
    FD_ZERO(&writable);
    FD_SET(server_socket, &writable);
    connect_timeout.tv_sec = 3;
    connect_timeout.tv_usec = 0;
    connect_result = select(0, NULL, &writable, NULL, &connect_timeout);
    if (connect_result <= 0) {
      if (connect_result == 0) {
        socket_error = WSAETIMEDOUT;
      } else {
        socket_error = WSAGetLastError();
      }
      closesocket(server_socket);
      snprintf(error, error_size,
               "Connexion à %s:%d impossible (erreur réseau %d).", host, port,
               socket_error);
      return INVALID_SOCKET;
    }
    if (getsockopt(server_socket, SOL_SOCKET, SO_ERROR, (char *)&socket_error,
                   &socket_error_size) == SOCKET_ERROR) {
      socket_error = WSAGetLastError();
    }
    if (socket_error != 0) {
      closesocket(server_socket);
      snprintf(error, error_size,
               "Connexion à %s:%d impossible (erreur réseau %d).", host, port,
               socket_error);
      return INVALID_SOCKET;
    }
  }
  nonblocking = 0;
  if (ioctlsocket(server_socket, FIONBIO, &nonblocking) == SOCKET_ERROR) {
    socket_error = WSAGetLastError();
    closesocket(server_socket);
    snprintf(error, error_size,
             "Configuration de la connexion TCP impossible (%d).",
             socket_error);
    return INVALID_SOCKET;
  }
  if (setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout,
                 sizeof(timeout)) == SOCKET_ERROR) {
    int socket_error = WSAGetLastError();
    closesocket(server_socket);
    snprintf(error, error_size,
             "Configuration de la connexion TCP impossible (%d).",
             socket_error);
    return INVALID_SOCKET;
  }
  return server_socket;
}

static int connect_to_server(app_state_t *state, const char *host, int port,
                             char *error, size_t error_size) {
  SOCKET server_socket = open_server_socket(host, port, error, error_size);

  if (server_socket == INVALID_SOCKET) {
    return 0;
  }
  closesocket(server_socket);
  snprintf(state->server_host, sizeof(state->server_host), "%s", host);
  state->server_port = port;
  state->server_connected = 1;
  return 1;
}

static void connect_server(const char *id, const char *request,
                           void *argument) {
  app_state_t *state = (app_state_t *)argument;
  char host[16];
  char error[512] = {0};
  int port;

  if (!parse_server_request(request, host, sizeof(host), &port) ||
      port < 1 || port > 65535) {
    report_binding_error(state->window, id,
                         "Saisis une adresse IPv4 et un port entre 1 et 65535.");
    return;
  }
  state->server_connected = 0;
  if (!connect_to_server(state, host, port, error, sizeof(error))) {
    state->server_connected = 0;
    report_binding_error(state->window, id, error);
    return;
  }
  if (!database_save_server_config(state->database, host, port, error,
                                   sizeof(error))) {
    state->server_connected = 0;
    report_binding_error(state->window, id, error);
    return;
  }
  if (webview_return(state->window, id, 0, "true") != WEBVIEW_ERROR_OK) {
    fprintf(stderr, "Impossible de confirmer la connexion au serveur.\n");
  }
}

static void get_server_config(const char *id, const char *request,
                              void *argument) {
  app_state_t *state = (app_state_t *)argument;
  char error[512] = {0};
  char *config;
  (void)request;

  config = database_get_server_config(state->database, error, sizeof(error));
  if (config == NULL) {
    report_binding_error(state->window, id, error);
    return;
  }
  if (webview_return(state->window, id, 0, config) != WEBVIEW_ERROR_OK) {
    fprintf(stderr, "Impossible de transmettre la configuration du serveur.\n");
  }
  free(config);
}

static void get_server_status(const char *id, const char *request,
                              void *argument) {
  app_state_t *state = (app_state_t *)argument;
  (void)request;

  if (webview_return(state->window, id, 0,
                     state->server_connected ? "true" : "false") !=
      WEBVIEW_ERROR_OK) {
    fprintf(stderr, "Impossible de transmettre l'état de la connexion.\n");
  }
}

static void list_devices(const char *id, const char *request, void *argument) {
  app_state_t *state = (app_state_t *)argument;
  char error[512] = {0};
  char *devices;
  (void)request;

  devices = database_list_devices(state->database, error, sizeof(error));
  if (devices == NULL) {
    report_binding_error(state->window, id, error);
    return;
  }
  if (webview_return(state->window, id, 0, devices) != WEBVIEW_ERROR_OK) {
    fprintf(stderr, "Impossible de transmettre les équipements à l'interface.\n");
  }
  free(devices);
}

static void set_device_state(const char *id, const char *request,
                             void *argument) {
  app_state_t *state = (app_state_t *)argument;
  char error[512] = {0};
  char database_error[512] = {0};
  char actuator_ip[16];
  char actuator_input[9];
  struct in_addr actuator_address;
  SOCKET command_socket;
  unsigned char payload[13];
  int type;
  int device_id;
  int is_on;
  int i;
  int sent = 0;

  if (!parse_device_request(request, &type, &device_id, &is_on) ||
      type < 0 ||
      type >= (int)(sizeof(device_tables) / sizeof(device_tables[0])) ||
      (is_on != 0 && is_on != 1)) {
    report_binding_error(state->window, id, "Paramètres d'équipement invalides.");
    return;
  }
  if (!state->server_connected) {
    report_binding_error(state->window, id,
                         "Connecte-toi au serveur SimDom avant de piloter un équipement.");
    return;
  }
  if (!database_get_device_endpoint(state->database, type, device_id,
                                    actuator_ip, sizeof(actuator_ip),
                                    actuator_input, sizeof(actuator_input),
                                    error, sizeof(error))) {
    report_binding_error(state->window, id, error);
    return;
  }
  if (InetPtonA(AF_INET, actuator_ip, &actuator_address) != 1) {
    report_binding_error(state->window, id,
                         "L'adresse de l'automate est invalide dans la base.");
    return;
  }
  memcpy(payload, &actuator_address.s_addr, 4);
  for (i = 0; i < 8; i++) {
    if (actuator_input[i] != '0' && actuator_input[i] != '1') {
      report_binding_error(state->window, id,
                           "L'entrée de l'automate est invalide dans la base.");
      return;
    }
    payload[4 + i] = (unsigned char)(actuator_input[i] - '0');
  }
  payload[12] = (unsigned char)is_on;
  command_socket = open_server_socket(state->server_host, state->server_port,
                                      error, sizeof(error));
  if (command_socket == INVALID_SOCKET) {
    state->server_connected = 0;
    report_binding_error(state->window, id, error);
    return;
  }
  while (sent < (int)sizeof(payload)) {
    int result = send(command_socket, (const char *)payload + sent,
                      (int)sizeof(payload) - sent, 0);
    if (result == SOCKET_ERROR || result == 0) {
      int socket_error = WSAGetLastError();
      closesocket(command_socket);
      state->server_connected = 0;
      snprintf(error, sizeof(error),
               "Envoi de la commande à SimDom impossible (%d).", socket_error);
      report_binding_error(state->window, id, error);
      return;
    }
    sent += result;
  }
  closesocket(command_socket);
  if (!database_set_device_state(state->database, type, device_id, is_on,
                                 database_error, sizeof(database_error))) {
    snprintf(error, sizeof(error),
             "Commande envoyée à SimDom, mais la base locale n'a pas pu être "
             "mise à jour: %.400s",
             database_error);
    report_binding_error(state->window, id, error);
    return;
  }
  if (webview_return(state->window, id, 0, "true") != WEBVIEW_ERROR_OK) {
    fprintf(stderr, "Commande envoyée, mais impossible de confirmer l'opération.\n");
  }
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main(void) {
#endif
  app_state_t state = {0};
  char exePath[MAX_PATH];
  char databasePath[MAX_PATH];
  char seedPath[MAX_PATH];
  char fileUrl[MAX_PATH + 16];
  char error[512] = {0};
  DWORD exePathLength;
  char *lastSlash;
  WSADATA winsock_data;

  exePathLength = GetModuleFileNameA(NULL, exePath, MAX_PATH);
  if (exePathLength == 0 || exePathLength >= MAX_PATH) {
    MessageBoxA(NULL, "Impossible de déterminer le dossier de l'application.",
                "Erreur", MB_ICONERROR | MB_OK);
    return 1;
  }
  lastSlash = strrchr(exePath, '\\');
  if (lastSlash == NULL) {
    MessageBoxA(NULL, "Chemin de l'application invalide.", "Erreur",
                MB_ICONERROR | MB_OK);
    return 1;
  }
  *lastSlash = '\0';
  if (!make_path(databasePath, sizeof(databasePath), exePath, "Database.db") ||
      !make_path(seedPath, sizeof(seedPath), exePath, "Database.sql")) {
    MessageBoxA(NULL, "Chemin de la base de données trop long.", "Erreur",
                MB_ICONERROR | MB_OK);
    return 1;
  }
  if (!database_open(&state.database, databasePath, seedPath, error,
                     sizeof(error))) {
    MessageBoxA(NULL, error, "Erreur de base de données",
                MB_ICONERROR | MB_OK);
    return 1;
  }

  if (WSAStartup(MAKEWORD(2, 2), &winsock_data) != 0) {
    database_close(state.database);
    MessageBoxA(NULL, "Initialisation du réseau TCP impossible.", "Erreur",
                MB_ICONERROR | MB_OK);
    return 1;
  }

  webview_t w = webview_create(0, NULL);
  if (w == NULL) {
    WSACleanup();
    database_close(state.database);
    MessageBoxA(NULL, "Impossible de créer la fenêtre de l'application.",
                "Erreur", MB_ICONERROR | MB_OK);
    return 1;
  }
  state.window = w;
  webview_set_title(w, "Logiciel domotique");
  webview_set_size(w, 1500, 650, WEBVIEW_HINT_NONE);

  if (webview_bind(w, "listDevices", list_devices, &state) != WEBVIEW_ERROR_OK ||
      webview_bind(w, "setDeviceState", set_device_state, &state) !=
          WEBVIEW_ERROR_OK ||
      webview_bind(w, "connectServer", connect_server, &state) !=
          WEBVIEW_ERROR_OK ||
      webview_bind(w, "getServerConfig", get_server_config, &state) !=
          WEBVIEW_ERROR_OK ||
      webview_bind(w, "getServerStatus", get_server_status, &state) !=
          WEBVIEW_ERROR_OK) {
    database_close(state.database);
    webview_destroy(w);
    WSACleanup();
    MessageBoxA(NULL, "Impossible de connecter l'interface à la base de données.",
                "Erreur", MB_ICONERROR | MB_OK);
    return 1;
  }
  {
    int url_length =
        snprintf(fileUrl, sizeof(fileUrl), "file:///%s/index.html", exePath);
    if (url_length < 0 || (size_t)url_length >= sizeof(fileUrl)) {
      database_close(state.database);
      webview_destroy(w);
      WSACleanup();
      MessageBoxA(NULL, "Impossible de construire l'adresse de l'interface.",
                  "Erreur", MB_ICONERROR | MB_OK);
      return 1;
    }
  }

  webview_navigate(w, fileUrl);
  webview_run(w);
  webview_destroy(w);
  database_close(state.database);
  WSACleanup();
  return 0;
}