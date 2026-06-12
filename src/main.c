#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datalink_sim.h"
#include "ftp_app.h"
#include "utils.h"

static void print_usage(const char *program_name)
{
    printf("Usage:\n");
    printf("  %s help\n", program_name);
    printf("  %s ftp-server <port>\n", program_name);
    printf("  %s ftp-client <host> <port>\n", program_name);
    printf("  %s datalink-demo\n", program_name);
    printf("  %s ping <host> [-t]\n", program_name);
    printf("  %s capture <local-ip>\n", program_name);
}

static int run_datalink_demo(void)
{
    static const uint8_t sample1[] = "frame-one";
    static const uint8_t sample2[] = "frame-two";
    datalink_simulator_t *simulator;
    int status;

    simulator = datalink_simulator_create(DLINK_MODE_GBN, DLINK_DEFAULT_WINDOW, 250, 0.20);
    if (simulator == NULL) {
        log_message(LOG_LEVEL_ERROR, "failed to create datalink simulator");
        return 1;
    }

    datalink_simulator_queue_payload(simulator, sample1, sizeof(sample1) - 1U);
    datalink_simulator_queue_payload(simulator, sample2, sizeof(sample2) - 1U);

    status = datalink_simulator_run(simulator);
    datalink_simulator_print_stats(simulator);
    datalink_simulator_destroy(simulator);
    return status;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "ftp-server") == 0) {
        if (argc != 3) {
            print_usage(argv[0]);
            return 1;
        }
        return ftp_server_run((unsigned short)atoi(argv[2]));
    }

    if (strcmp(argv[1], "ftp-client") == 0) {
        if (argc != 4) {
            print_usage(argv[0]);
            return 1;
        }
        return ftp_client_run(argv[2], (unsigned short)atoi(argv[3]));
    }

    if (strcmp(argv[1], "datalink-demo") == 0) {
        return run_datalink_demo();
    }

    if (strcmp(argv[1], "ping") == 0) {
        log_message(LOG_LEVEL_WARN, "ping module is reserved for the next implementation step");
        return 0;
    }

    if (strcmp(argv[1], "capture") == 0) {
        log_message(LOG_LEVEL_WARN, "packet capture module is reserved for the next implementation step");
        return 0;
    }

    print_usage(argv[0]);
    return 1;
}
