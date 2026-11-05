#include "database.h"

#include "sqlite3.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct database {
  sqlite3 *connection;
};

struct string_builder {
  char *data;
  size_t length;
  size_t capacity;
};

static void set_error(char *error, size_t error_size, const char *format, ...) {
  va_list args;

  if (error == NULL || error_size == 0) {
    return;
  }
  va_start(args, format);
  vsnprintf(error, error_size, format, args);
  va_end(args);
}

static int builder_reserve(struct string_builder *builder, size_t extra) {
  size_t required = builder->length + extra + 1;
  size_t capacity;
  char *data;

  if (required <= builder->capacity) {
    return 1;
  }
  capacity = builder->capacity == 0 ? 256 : builder->capacity;
  while (capacity < required) {
    if (capacity > (size_t)-1 / 2) {
      return 0;
    }
    capacity *= 2;
  }
  data = (char *)realloc(builder->data, capacity);
  if (data == NULL) {
    return 0;
  }
  builder->data = data;
  builder->capacity = capacity;
  return 1;
}

static int builder_append(struct string_builder *builder, const char *format,
                          ...) {
  va_list args;
  va_list copy;
  int length;

  va_start(args, format);
  va_copy(copy, args);
  length = vsnprintf(NULL, 0, format, copy);
  va_end(copy);
  if (length < 0 || !builder_reserve(builder, (size_t)length)) {
    va_end(args);
    return 0;
  }
  vsnprintf(builder->data + builder->length,
            builder->capacity - builder->length, format, args);
  va_end(args);
  builder->length += (size_t)length;
  return 1;
}

static int builder_append_json_string(struct string_builder *builder,
                                      const unsigned char *text) {
  if (!builder_append(builder, "\"")) {
    return 0;
  }
  while (*text != '\0') {
    unsigned char c = *text++;
    switch (c) {
    case '"':
    case '\\':
      if (!builder_append(builder, "\\%c", c)) {
        return 0;
      }
      break;
    case '\b':
      if (!builder_append(builder, "\\b")) {
        return 0;
      }
      break;
    case '\f':
      if (!builder_append(builder, "\\f")) {
        return 0;
      }
      break;
    case '\n':
      if (!builder_append(builder, "\\n")) {
        return 0;
      }
      break;
    case '\r':
      if (!builder_append(builder, "\\r")) {
        return 0;
      }
      break;
    case '\t':
      if (!builder_append(builder, "\\t")) {
        return 0;
      }
      break;
    default:
      if (c < 0x20) {
        if (!builder_append(builder, "\\u%04x", c)) {
          return 0;
        }
      } else if (!builder_reserve(builder, 1)) {
        return 0;
      } else {
        builder->data[builder->length++] = (char)c;
        builder->data[builder->length] = '\0';
      }
    }
  }
  return builder_append(builder, "\"");
}

static char *read_file(const char *path, char *error, size_t error_size) {
  FILE *file;
  long file_size;
  char *contents;
  size_t bytes_read;

#ifdef _MSC_VER
  if (fopen_s(&file, path, "rb") != 0) {
    file = NULL;
  }
#else
  file = fopen(path, "rb");
#endif
  if (file == NULL) {
    set_error(error, error_size, "Impossible d'ouvrir le script SQL: %s", path);
    return NULL;
  }
  if (fseek(file, 0, SEEK_END) != 0 || (file_size = ftell(file)) < 0 ||
      fseek(file, 0, SEEK_SET) != 0) {
    set_error(error, error_size, "Impossible de lire le script SQL: %s", path);
    fclose(file);
    return NULL;
  }
  contents = (char *)malloc((size_t)file_size + 1);
  if (contents == NULL) {
    set_error(error, error_size, "Mémoire insuffisante pour lire le script SQL.");
    fclose(file);
    return NULL;
  }
  bytes_read = fread(contents, 1, (size_t)file_size, file);
  if (bytes_read != (size_t)file_size || ferror(file)) {
    set_error(error, error_size, "Lecture incomplète du script SQL: %s", path);
    free(contents);
    fclose(file);
    return NULL;
  }
  fclose(file);
  contents[bytes_read] = '\0';
  return contents;
}

static int count_user_tables(sqlite3 *connection, int *count) {
  sqlite3_stmt *statement = NULL;
  int result = sqlite3_prepare_v2(
      connection,
      "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' "
      "AND name NOT LIKE 'sqlite_%'",
      -1, &statement, NULL);

  if (result != SQLITE_OK) {
    return result;
  }
  result = sqlite3_step(statement);
  if (result == SQLITE_ROW) {
    *count = sqlite3_column_int(statement, 0);
    result = SQLITE_OK;
  }
  sqlite3_finalize(statement);
  return result;
}

static int has_expected_schema(sqlite3 *connection) {
  sqlite3_stmt *statement = NULL;
  int result = sqlite3_prepare_v2(
      connection,
      "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name IN "
      "('localisations', 'climatisations', 'luminaires', 'prises', 'volets')",
      -1, &statement, NULL);

  if (result != SQLITE_OK) {
    return 0;
  }
  result = sqlite3_step(statement);
  if (result == SQLITE_ROW) {
    result = sqlite3_column_int(statement, 0) == 5;
  } else {
    result = 0;
  }
  sqlite3_finalize(statement);
  return result;
}

int database_open(database_t **out, const char *database_path,
                  const char *seed_path, char *error, size_t error_size) {
  database_t *database;
  int result;
  int table_count = 0;

  if (out == NULL || database_path == NULL || seed_path == NULL) {
    set_error(error, error_size, "Chemin de base de données invalide.");
    return 0;
  }
  *out = NULL;
  database = (database_t *)calloc(1, sizeof(*database));
  if (database == NULL) {
    set_error(error, error_size, "Mémoire insuffisante pour ouvrir la base.");
    return 0;
  }
  result = sqlite3_open_v2(database_path, &database->connection,
                           SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (result != SQLITE_OK) {
    set_error(error, error_size, "Ouverture de la base impossible: %s",
              sqlite3_errmsg(database->connection));
    database_close(database);
    return 0;
  }
  result = sqlite3_exec(database->connection, "PRAGMA foreign_keys = ON;",
                        NULL, NULL, NULL);
  if (result != SQLITE_OK) {
    set_error(error, error_size, "Configuration SQLite impossible: %s",
              sqlite3_errmsg(database->connection));
    database_close(database);
    return 0;
  }
  result = count_user_tables(database->connection, &table_count);
  if (result != SQLITE_OK) {
    set_error(error, error_size, "Lecture de la base impossible: %s",
              sqlite3_errmsg(database->connection));
    database_close(database);
    return 0;
  }
  if (table_count == 0) {
    char *script;

    script = read_file(seed_path, error, error_size);
    if (script == NULL) {
      database_close(database);
      return 0;
    }
    result = sqlite3_exec(database->connection, script, NULL, NULL, NULL);
    free(script);
    if (result != SQLITE_OK) {
      set_error(error, error_size, "Initialisation de la base impossible: %s",
                sqlite3_errmsg(database->connection));
      database_close(database);
      return 0;
    }
  } else if (!has_expected_schema(database->connection)) {
    set_error(error, error_size,
              "La base contient des tables, mais son schéma ne correspond pas "
              "aux équipements attendus.");
    database_close(database);
    return 0;
  }
  result = sqlite3_exec(
      database->connection,
      "CREATE TABLE IF NOT EXISTS server_config ("
      "id INTEGER PRIMARY KEY CHECK (id = 1), "
      "host TEXT NOT NULL, "
      "port INTEGER NOT NULL CHECK (port BETWEEN 1 AND 65535));",
      NULL, NULL, NULL);
  if (result != SQLITE_OK) {
    set_error(error, error_size, "Configuration de la base impossible: %s",
              sqlite3_errmsg(database->connection));
    database_close(database);
    return 0;
  }
  *out = database;
  return 1;
}

char *database_list_devices(database_t *database, char *error,
                            size_t error_size) {
  static const char query[] =
      "SELECT type, id, description, etat, localisation FROM ("
      "SELECT 0 AS type, d.id, d.description, d.etat, l.description AS "
      "localisation FROM climatisations d JOIN localisations l ON "
      "l.id = d.localisation_id "
      "UNION ALL "
      "SELECT 1, d.id, d.description, d.etat, l.description FROM luminaires d "
      "JOIN localisations l ON l.id = d.localisation_id "
      "UNION ALL "
      "SELECT 2, d.id, d.description, d.etat, l.description FROM prises d "
      "JOIN localisations l ON l.id = d.localisation_id "
      "UNION ALL "
      "SELECT 3, d.id, d.description, d.etat, l.description FROM volets d "
      "JOIN localisations l ON l.id = d.localisation_id) "
      "ORDER BY localisation, type, description";
  struct string_builder builder = {0};
  sqlite3_stmt *statement = NULL;
  int result;
  int first = 1;

  result = sqlite3_prepare_v2(database->connection, query, -1, &statement, NULL);
  if (result != SQLITE_OK) {
    set_error(error, error_size, "Lecture des équipements impossible: %s",
              sqlite3_errmsg(database->connection));
    return NULL;
  }
  if (!builder_append(&builder, "[")) {
    set_error(error, error_size, "Mémoire insuffisante pour les équipements.");
    sqlite3_finalize(statement);
    return NULL;
  }
  while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
    if ((!first && !builder_append(&builder, ",")) ||
        !builder_append(&builder, "{\"type\":%d,\"id\":%d,\"description\":",
                        sqlite3_column_int(statement, 0),
                        sqlite3_column_int(statement, 1)) ||
        !builder_append_json_string(
            &builder, sqlite3_column_text(statement, 2)) ||
        !builder_append(&builder, ",\"state\":") ||
        !builder_append_json_string(
            &builder, sqlite3_column_text(statement, 3)) ||
        !builder_append(&builder, ",\"location\":") ||
        !builder_append_json_string(
            &builder, sqlite3_column_text(statement, 4)) ||
        !builder_append(&builder, "}")) {
      set_error(error, error_size, "Mémoire insuffisante pour les équipements.");
      sqlite3_finalize(statement);
      free(builder.data);
      return NULL;
    }
    first = 0;
  }
  if (result != SQLITE_DONE) {
    set_error(error, error_size, "Lecture des équipements impossible: %s",
              sqlite3_errmsg(database->connection));
    sqlite3_finalize(statement);
    free(builder.data);
    return NULL;
  }
  sqlite3_finalize(statement);
  if (!builder_append(&builder, "]")) {
    set_error(error, error_size, "Mémoire insuffisante pour les équipements.");
    free(builder.data);
    return NULL;
  }
  return builder.data;
}

char *database_get_server_config(database_t *database, char *error,
                                 size_t error_size) {
  sqlite3_stmt *statement = NULL;
  struct string_builder builder = {0};
  const unsigned char *host = (const unsigned char *)"";
  int port = 53084;
  int result = sqlite3_prepare_v2(
      database->connection, "SELECT host, port FROM server_config WHERE id = 1",
      -1, &statement, NULL);

  if (result != SQLITE_OK) {
    set_error(error, error_size, "Lecture de la connexion impossible: %s",
              sqlite3_errmsg(database->connection));
    return NULL;
  }
  result = sqlite3_step(statement);
  if (result == SQLITE_ROW) {
    host = sqlite3_column_text(statement, 0);
    port = sqlite3_column_int(statement, 1);
  } else if (result != SQLITE_DONE) {
    set_error(error, error_size, "Lecture de la connexion impossible: %s",
              sqlite3_errmsg(database->connection));
    sqlite3_finalize(statement);
    return NULL;
  }
  if (host == NULL) {
    host = (const unsigned char *)"";
  }
  if (!builder_append(&builder, "{\"host\":") ||
      !builder_append_json_string(&builder, host) ||
      !builder_append(&builder, ",\"port\":%d}", port)) {
    set_error(error, error_size,
              "Mémoire insuffisante pour lire la connexion.");
    free(builder.data);
    sqlite3_finalize(statement);
    return NULL;
  }
  sqlite3_finalize(statement);
  return builder.data;
}

int database_save_server_config(database_t *database, const char *host, int port,
                                char *error, size_t error_size) {
  sqlite3_stmt *statement = NULL;
  int result = sqlite3_prepare_v2(
      database->connection,
      "INSERT INTO server_config (id, host, port) VALUES (1, ?, ?) "
      "ON CONFLICT(id) DO UPDATE SET host = excluded.host, "
      "port = excluded.port",
      -1, &statement, NULL);

  if (result == SQLITE_OK) {
    result = sqlite3_bind_text(statement, 1, host, -1, SQLITE_TRANSIENT);
  }
  if (result == SQLITE_OK) {
    result = sqlite3_bind_int(statement, 2, port);
  }
  if (result == SQLITE_OK) {
    result = sqlite3_step(statement);
  }
  if (result != SQLITE_DONE) {
    set_error(error, error_size, "Sauvegarde de la connexion impossible: %s",
              sqlite3_errmsg(database->connection));
    sqlite3_finalize(statement);
    return 0;
  }
  sqlite3_finalize(statement);
  return 1;
}

int database_get_device_endpoint(database_t *database, int type, int id,
                                 char *ip, size_t ip_size, char *input,
                                 size_t input_size, char *error,
                                 size_t error_size) {
  static const char *const tables[] = {"climatisations", "luminaires", "prises",
                                       "volets"};
  char query[192];
  sqlite3_stmt *statement = NULL;
  const unsigned char *stored_ip;
  const unsigned char *stored_input;
  int result;
  int ip_length;
  int input_length;

  if (type < 0 || type >= (int)(sizeof(tables) / sizeof(tables[0])) || id <= 0 ||
      ip == NULL || ip_size == 0 || input == NULL || input_size < 9) {
    set_error(error, error_size, "Équipement ou destination invalide.");
    return 0;
  }
  snprintf(query, sizeof(query),
           "SELECT ip_automate, entree_automate FROM %s WHERE id = ?",
           tables[type]);
  result = sqlite3_prepare_v2(database->connection, query, -1, &statement, NULL);
  if (result == SQLITE_OK) {
    result = sqlite3_bind_int(statement, 1, id);
  }
  if (result == SQLITE_OK) {
    result = sqlite3_step(statement);
  }
  if (result != SQLITE_ROW) {
    if (result == SQLITE_DONE) {
      set_error(error, error_size, "Équipement introuvable dans la base.");
    } else {
      set_error(error, error_size, "Lecture de l'équipement impossible: %s",
                sqlite3_errmsg(database->connection));
    }
    sqlite3_finalize(statement);
    return 0;
  }
  stored_ip = sqlite3_column_text(statement, 0);
  stored_input = sqlite3_column_text(statement, 1);
  ip_length = sqlite3_column_bytes(statement, 0);
  input_length = sqlite3_column_bytes(statement, 1);
  if (stored_ip == NULL || stored_input == NULL || ip_length <= 0 ||
      (size_t)ip_length >= ip_size || input_length != 8) {
    set_error(error, error_size, "Adresse automate invalide dans la base.");
    sqlite3_finalize(statement);
    return 0;
  }
  memcpy(ip, stored_ip, (size_t)ip_length);
  ip[ip_length] = '\0';
  memcpy(input, stored_input, (size_t)input_length);
  input[input_length] = '\0';
  sqlite3_finalize(statement);
  return 1;
}

int database_set_device_state(database_t *database, int type, int id,
                              int is_on, char *error, size_t error_size) {
  static const char *const tables[] = {"climatisations", "luminaires", "prises",
                                       "volets"};
  char query[192];
  sqlite3_stmt *statement = NULL;
  int result;

  if (type < 0 || type >= (int)(sizeof(tables) / sizeof(tables[0])) || id <= 0 ||
      (is_on != 0 && is_on != 1)) {
    set_error(error, error_size, "Commande d'équipement invalide.");
    return 0;
  }
  snprintf(query, sizeof(query),
           "UPDATE %s SET etat = ? WHERE id = ? "
           "AND etat IN ('allumé', 'éteint')",
           tables[type]);
  result = sqlite3_prepare_v2(database->connection, query, -1, &statement, NULL);
  if (result == SQLITE_OK) {
    result = sqlite3_bind_text(statement, 1, is_on ? "allumé" : "éteint", -1,
                               SQLITE_TRANSIENT);
  }
  if (result == SQLITE_OK) {
    result = sqlite3_bind_int(statement, 2, id);
  }
  if (result == SQLITE_OK) {
    result = sqlite3_step(statement);
  }
  if (result != SQLITE_DONE) {
    set_error(error, error_size, "Mise à jour de l'équipement impossible: %s",
              sqlite3_errmsg(database->connection));
    sqlite3_finalize(statement);
    return 0;
  }
  sqlite3_finalize(statement);
  if (sqlite3_changes(database->connection) != 1) {
    set_error(error, error_size,
              "Équipement introuvable ou déclaré en panne.");
    return 0;
  }
  return 1;
}

void database_close(database_t *database) {
  if (database == NULL) {
    return;
  }
  if (database->connection != NULL) {
    sqlite3_close(database->connection);
  }
  free(database);
}
