#include "datalink_sim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

#define DLINK_QUEUE_CAPACITY 32

struct datalink_simulator {
    datalink_mode_t mode;
    size_t window_size;
    unsigned int timeout_ms;
    double loss_rate;
    datalink_stats_t stats;
    datalink_frame_t queue[DLINK_QUEUE_CAPACITY];
    size_t queued_count;
    uint8_t next_sequence;
};

static double random_unit_value(void)
{
    return (double)rand() / (double)RAND_MAX;
}

static void prepare_frame(
    datalink_simulator_t *simulator,
    datalink_frame_t *frame,
    const uint8_t *payload,
    size_t payload_length
)
{
    memset(frame, 0, sizeof(*frame));
    frame->header.frame_type = (uint8_t)simulator->mode;
    frame->header.sequence_number = simulator->next_sequence++;
    frame->header.acknowledgement_number = 0;
    frame->header.payload_length = (uint8_t)payload_length;
    memcpy(frame->payload, payload, payload_length);
    frame->header.checksum = compute_crc32(frame->payload, payload_length);
}

datalink_simulator_t *datalink_simulator_create(
    datalink_mode_t mode,
    size_t window_size,
    unsigned int timeout_ms,
    double loss_rate
)
{
    datalink_simulator_t *simulator;

    simulator = (datalink_simulator_t *)calloc(1, sizeof(*simulator));
    if (simulator == NULL) {
        return NULL;
    }

    simulator->mode = mode;
    simulator->window_size = (window_size == 0) ? 1 : window_size;
    simulator->timeout_ms = timeout_ms;
    simulator->loss_rate = loss_rate;
    srand((unsigned int)time(NULL));
    return simulator;
}

void datalink_simulator_destroy(datalink_simulator_t *simulator)
{
    free(simulator);
}

int datalink_simulator_queue_payload(
    datalink_simulator_t *simulator,
    const uint8_t *payload,
    size_t payload_length
)
{
    if (simulator == NULL || payload == NULL || payload_length == 0 || payload_length > DLINK_MAX_PAYLOAD_SIZE) {
        return -1;
    }

    if (simulator->queued_count >= DLINK_QUEUE_CAPACITY) {
        return -1;
    }

    prepare_frame(
        simulator,
        &simulator->queue[simulator->queued_count],
        payload,
        payload_length
    );
    simulator->queued_count++;
    return 0;
}

int datalink_simulator_run(datalink_simulator_t *simulator)
{
    size_t index;

    if (simulator == NULL) {
        return -1;
    }

    for (index = 0; index < simulator->queued_count; ++index) {
        datalink_frame_t *frame = &simulator->queue[index];
        simulator->stats.sent_frames++;
        log_message(
            LOG_LEVEL_INFO,
            "send frame seq=%u len=%u mode=%s",
            (unsigned int)frame->header.sequence_number,
            (unsigned int)frame->header.payload_length,
            simulator->mode == DLINK_MODE_GBN ? "GBN" : "STOP_WAIT"
        );

        if (random_unit_value() < simulator->loss_rate) {
            simulator->stats.dropped_frames++;
            simulator->stats.resent_frames++;
            log_message(
                LOG_LEVEL_WARN,
                "frame seq=%u dropped, retransmitting after %u ms",
                (unsigned int)frame->header.sequence_number,
                simulator->timeout_ms
            );
            simulator->stats.sent_frames++;
        }

        if (compute_crc32(frame->payload, frame->header.payload_length) != frame->header.checksum) {
            simulator->stats.checksum_errors++;
            return -1;
        }

        simulator->stats.delivered_frames++;
        log_message(
            LOG_LEVEL_INFO,
            "ack frame seq=%u payload=\"%.*s\"",
            (unsigned int)frame->header.sequence_number,
            (int)frame->header.payload_length,
            (const char *)frame->payload
        );
    }

    return 0;
}

void datalink_simulator_print_stats(const datalink_simulator_t *simulator)
{
    const datalink_stats_t *stats;

    if (simulator == NULL) {
        return;
    }

    stats = &simulator->stats;
    printf("datalink stats: sent=%u resent=%u delivered=%u dropped=%u crc_error=%u\n",
        stats->sent_frames,
        stats->resent_frames,
        stats->delivered_frames,
        stats->dropped_frames,
        stats->checksum_errors);
}

const datalink_stats_t *datalink_simulator_get_stats(const datalink_simulator_t *simulator)
{
    return (simulator == NULL) ? NULL : &simulator->stats;
}
