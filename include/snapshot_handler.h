#ifndef __SNAPSHOT_HANDLER_H_
#define __SNAPSHOT_HANDLER_H_

#include <stdint.h>
#include "timestamping.h"
#include "max2769.h"

//Define the size of snapshots to be received from max2769 in bytes
//Snapshot size depends on sampling frequency, snapshot duration and adc resolution
#define SNAPSHOT_SIZE_BYTES 6138    // 4092000 Hz x 0.012s / 8 = 6138 Bytes

#define TOTAL_NUMBER_FRAMES 27

#define LENGTH_FIRST_FRAME 26
#define SNAPSHOT_BYTES_PER_FRAME 243
#define SNAPSHOT_BYTES_LAST_FRAME 63

//Byte offset for frame fields
#define OFFSET_SNAPSHOT_ID 0
#define OFFSET_FRAME_NUMBER 2
#define OFFSET_SNAPSHOT_SAMPLES 4

//length of fields frame header
#define LENGTH_FRAME_HEADER 4
#define LENGTH_SNAPSHOT_ID 2
#define LENGTH_FRAME_NUMBER 2

typedef struct {
    uint16_t snapshot_id;
    uint16_t frame_number;
    uint16_t total_number_frames;
    uint16_t bytes_per_frame;
    uint16_t bytes_last_frame;
    timestamp_t capture_timestamp;
    timestamp_t transmit_timestamp;
} frame_0_t;

// typedef struct {
//     uint16_t snapshot_id;
//     uint16_t frame_number;
//     uint8_t data[BYTES_PER_FRAME];
// } middle_frames_t;

// typedef struct {
//     uint16_t snapshot_id;
//     uint16_t frame_number;
//     uint8_t data[BYTES_LAST_FRAME];
// } last_frame_t;

int get_timestamped_snapshot(const max2769_cfg_t *max2769_cfg, uint8_t *snapshot_buf, timestamp_t *capture_timestamp);
void init_snapshot_transmitter();
int take_timestamp_and_send_first_frame(timestamp_t *capture_timestamp, timestamp_t *transmit_timestamp, uint16_t snapshot_id);
int send_snapshot_data_frame(uint8_t *snapshot_buf, uint16_t frame_number, uint16_t snapshot_id);

#endif /* __SNAPSHOT_HANDLER_H_ */