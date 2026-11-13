#include "simdom_protocol.h"

#include <string.h>

static int parse_ipv4(const char *text, unsigned char address[4]) {
  int octet_index;
  const char *cursor = text;

  if (text == NULL || *text == '\0') {
    return 0;
  }
  for (octet_index = 0; octet_index < 4; octet_index++) {
    unsigned int octet = 0;
    int digits = 0;

    while (*cursor >= '0' && *cursor <= '9') {
      octet = octet * 10U + (unsigned int)(*cursor - '0');
      if (octet > 255U) {
        return 0;
      }
      cursor++;
      digits++;
    }
    if (digits == 0) {
      return 0;
    }
    address[octet_index] = (unsigned char)octet;
    if (octet_index < 3) {
      if (*cursor++ != '.') {
        return 0;
      }
    } else if (*cursor != '\0') {
      return 0;
    }
  }
  return 1;
}

int simdom_build_payload(const char *actuator_ip, const char *input,
                         int device_type, int is_on, int mode,
                         unsigned char payload[SIMDOM_PAYLOAD_MAX_SIZE],
                         size_t *payload_size) {
  int i;

  if (payload == NULL || payload_size == NULL || input == NULL ||
      strlen(input) != 8 ||
      device_type < 0 || device_type > 3 || (is_on != 0 && is_on != 1) ||
      (device_type == 0 && mode != 0 && mode != 1) ||
      (device_type != 0 && mode != -1)) {
    return 0;
  }
  if (!parse_ipv4(actuator_ip, payload)) {
    return 0;
  }
  for (i = 0; i < 8; i++) {
    if (input[i] != '0' && input[i] != '1') {
      return 0;
    }
    payload[4 + i] = (unsigned char)(input[i] - '0');
  }
  payload[12] = (unsigned char)is_on;
  if (device_type == 0) {
    payload[13] = (unsigned char)mode;
    *payload_size = 14;
  } else {
    *payload_size = 13;
  }
  return 1;
}
