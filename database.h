#ifndef DOMOTIQUE_DATABASE_H
#define DOMOTIQUE_DATABASE_H

#include <stddef.h>

typedef struct database database_t;

int database_open(database_t **out, const char *database_path,
                  const char *seed_path, char *error, size_t error_size);
char *database_list_devices(database_t *database, char *error,
                            size_t error_size);
char *database_get_server_config(database_t *database, char *error,
                                 size_t error_size);
int database_save_server_config(database_t *database, const char *host, int port,
                                char *error, size_t error_size);
int database_get_device_endpoint(database_t *database, int type, int id,
                                 char *ip, size_t ip_size, char *input,
                                 size_t input_size, char *error,
                                 size_t error_size);
int database_set_device_state(database_t *database, int type, int id, int is_on,
                              int mode, char *error, size_t error_size);
void database_close(database_t *database);

#endif
