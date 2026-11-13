#ifndef SIMDOM_PROTOCOL_H
#define SIMDOM_PROTOCOL_H

#include <stddef.h>

#define SIMDOM_PAYLOAD_MAX_SIZE 14

int simdom_build_payload(const char *actuator_ip, const char *input,
                         int device_type, int is_on, int mode,
                         unsigned char payload[SIMDOM_PAYLOAD_MAX_SIZE],
                         size_t *payload_size);

#endif
