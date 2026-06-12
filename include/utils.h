#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

typedef enum log_level {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_DEBUG
} log_level_t;

void log_message(log_level_t level, const char *fmt, ...);
void log_socket_error(const char *context, int error_code);

uint16_t compute_internet_checksum(const void *data, size_t length);
uint32_t compute_crc32(const uint8_t *data, size_t length);
void hex_dump(const void *data, size_t length, size_t bytes_per_line);

#endif
