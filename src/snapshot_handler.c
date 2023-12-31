#include "snapshot_handler.h"
#include "riotee_stella.h"
#include "max2769.h"
#include "timestamping.h"
#include "string.h"
#include "riotee_timing.h"
#include "printf.h"

static riotee_stella_pkt_t tx_buf;
static riotee_stella_pkt_t rx_buf;

// Counts number of transmitted packets
static uint16_t stella_pkt_counter = 0;

int get_timestamped_snapshot(const max2769_cfg_t *max2769_cfg, uint8_t *snapshot_buf, timestamp_t *capture_timestamp)
{
    int result = 0;
    enable_max2769(max2769_cfg);
    riotee_sleep_ms(1);
    configure_max2769(max2769_cfg);
    riotee_sleep_ms(1);
    result += get_timestamp(capture_timestamp);
    max2769_capture_snapshot(max2769_cfg, snapshot_buf);
    disable_max2769(max2769_cfg);
    return result;
}

void init_snapshot_transmitter(uint32_t dev_id)
{
    riotee_stella_init();
    riotee_stella_set_id(dev_id);
}

int take_timestamp_and_send_first_frame(timestamp_t *capture_timestamp, timestamp_t *transmit_timestamp, uint16_t snapshot_id)
{
    //Prepare content of frame to be sent
    frame_0_t frame_0;
    frame_0.snapshot_id = snapshot_id;
    frame_0.frame_number = 0;
    frame_0.total_number_frames = TOTAL_NUMBER_FRAMES;
    frame_0.bytes_per_frame = SNAPSHOT_BYTES_PER_FRAME;
    frame_0.bytes_last_frame = SNAPSHOT_BYTES_LAST_FRAME;
    frame_0.capture_timestamp = *capture_timestamp;

    //Take transmit timestamp and insert it
    get_timestamp(transmit_timestamp);
    frame_0.transmit_timestamp = *transmit_timestamp;

    //Prepare parameters of stella packet and send it
    tx_buf.len = sizeof(riotee_stella_pkt_header_t) + LENGTH_FIRST_FRAME;
    tx_buf.hdr.pkt_id = stella_pkt_counter++;
    memcpy(tx_buf.data, &frame_0, LENGTH_FIRST_FRAME);
    return riotee_stella_verified_transmission(20, &rx_buf, &tx_buf);
}

int send_snapshot_data_frame(uint8_t *snapshot_buf, uint16_t frame_number, uint16_t snapshot_id)
{
    int frame_byte_offset = (int)(frame_number-1) * (int)SNAPSHOT_BYTES_PER_FRAME;
    //Check that this is NOT the last frame to be sent
    if(frame_number != (TOTAL_NUMBER_FRAMES-1))
    {
        //Set length of stella packet
        size_t frame_data_length = SNAPSHOT_BYTES_PER_FRAME + LENGTH_FRAME_HEADER;
        tx_buf.len = sizeof(riotee_stella_pkt_header_t) + frame_data_length;
        //Prepare stella packet content
        //insert pkt_id
        tx_buf.hdr.pkt_id = stella_pkt_counter++;
        //insert snapshot id
        memcpy((tx_buf.data + OFFSET_SNAPSHOT_ID), &snapshot_id, LENGTH_SNAPSHOT_ID);
        //insert frame number
        memcpy((tx_buf.data + OFFSET_FRAME_NUMBER), &frame_number, LENGTH_FRAME_NUMBER);
        //insert snapshot data
        memcpy((tx_buf.data + OFFSET_SNAPSHOT_SAMPLES), (snapshot_buf + frame_byte_offset), SNAPSHOT_BYTES_PER_FRAME);
        //Send stella packet
        return riotee_stella_verified_transmission(20, &rx_buf, &tx_buf);
    }
    else //Last frame
    {
        //Set length of stella packet
        size_t frame_data_length = SNAPSHOT_BYTES_LAST_FRAME + LENGTH_FRAME_HEADER;
        tx_buf.len = sizeof(riotee_stella_pkt_header_t) + frame_data_length;
        //Prepare stella packet content
        //insert pkt_id
        tx_buf.hdr.pkt_id = stella_pkt_counter++;
        //insert snapshot id
        memcpy((tx_buf.data + OFFSET_SNAPSHOT_ID), &snapshot_id, LENGTH_SNAPSHOT_ID);
        //insert frame number
        memcpy((tx_buf.data + OFFSET_FRAME_NUMBER), &frame_number, LENGTH_FRAME_NUMBER);
        //insert snapshot data
        memcpy((tx_buf.data + OFFSET_SNAPSHOT_SAMPLES), (snapshot_buf + frame_byte_offset), SNAPSHOT_BYTES_LAST_FRAME);
        //Send stella packet
        return riotee_stella_verified_transmission(20, &rx_buf, &tx_buf);
    }
}

int riotee_stella_verified_transmission(int max_retransmissions, riotee_stella_pkt_t *rx_pkt, riotee_stella_pkt_t *tx_pkt)
{
    int stella_return_value = STELLA_ERR_GENERIC;
    int retransmission_counter = 0;
    //Try to send packets. If we don't get an ACK, we retry sending for max_retransmissions times.
    while (retransmission_counter <= max_retransmissions )
    {
        stella_return_value = riotee_stella_transceive(rx_pkt, tx_pkt);
        if (stella_return_value == STELLA_ERR_OK)
        {
            printf_("No. of Retransmissions: %i\n", retransmission_counter);
            return STELLA_ERR_OK;
        }
        else
        {
            retransmission_counter++;
        }
    }
    //If we have not received an ACK after all retransmission have been performed, we return the error code.
    printf_("Transmission failed! \n");
    return stella_return_value;
}


