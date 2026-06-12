#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static const char *log_level_name(log_level_t level)
{
    switch (level) {
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_WARN: return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_DEBUG: return "DEBUG";
        default: return "LOG";
    }
}

void log_message(log_level_t level, const char *fmt, ...)
{
    va_list args;
    time_t now;
    struct tm *local_time_ptr;
    char time_buffer[32];

    now = time(NULL);
    local_time_ptr = NULL;

#if defined(_MSC_VER)
    struct tm local_time_storage;
    localtime_s(&local_time_storage, &now);
    local_time_ptr = &local_time_storage;
#elif defined(__MINGW32__)
    local_time_ptr = localtime(&now);
#else
    struct tm local_time_storage;
    localtime_r(&now, &local_time_storage);
    local_time_ptr = &local_time_storage;
#endif

    if (local_time_ptr != NULL) {
        strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", local_time_ptr);
    } else {
        snprintf(time_buffer, sizeof(time_buffer), "00:00:00");
    }

    fprintf(stderr, "[%s] [%s] ", time_buffer, log_level_name(level));

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}

void log_socket_error(const char *context, int error_code)
{
    log_message(LOG_LEVEL_ERROR, "%s failed, error=%d", context, error_code);
}

uint16_t compute_internet_checksum(const void *data, size_t length)
{
    const uint8_t *bytes;
    uint32_t sum;
    size_t index;

    bytes = (const uint8_t *)data;
    sum = 0;

    for (index = 0; index + 1 < length; index += 2) {
        sum += (uint32_t)((bytes[index] << 8) | bytes[index + 1]);
    }

    if ((length & 1U) != 0U) {
        sum += (uint32_t)(bytes[length - 1] << 8);
    }

    while ((sum >> 16) != 0U) {
        sum = (sum & 0xFFFFU) + (sum >> 16);
    }

    return (uint16_t)(~sum & 0xFFFFU);
}

uint32_t compute_crc32(const uint8_t *data, size_t length)
{
    uint32_t crc;
    size_t index;
    int bit;

    crc = 0xFFFFFFFFU;
    for (index = 0; index < length; ++index) {
        crc ^= data[index];
        for (bit = 0; bit < 8; ++bit) {
            if ((crc & 1U) != 0U) {
                crc = (crc >> 1) ^ 0xEDB88320U;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc ^ 0xFFFFFFFFU;
}

void hex_dump(const void *data, size_t length, size_t bytes_per_line)
{
    const uint8_t *bytes;
    size_t index;

    if (data == NULL || bytes_per_line == 0) {
        return;
    }

    bytes = (const uint8_t *)data;
    for (index = 0; index < length; ++index) {
        if ((index % bytes_per_line) == 0) {
            printf("%08lX  ", (unsigned long)index);
        }
        printf("%02X ", bytes[index]);
        if (((index + 1U) % bytes_per_line) == 0U || index + 1U == length) {
            printf("\n");
        }
    }
}
