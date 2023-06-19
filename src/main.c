#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_spic.h"
#include "riotee_spis.h"
#include "max2769.h"
#include "snapshot_handler.h"
#include "printf.h"

//Global buffer to store gnss snapshot
uint8_t snapshot_buf[SNAPSHOT_SIZE_BYTES] __VOLATILE_UNINITIALIZED;
uint16_t snapshot_id = 0;

//Global structures to store timestamps
timestamp_t capture_timestamp;
timestamp_t transmit_timestamp;

//Define Device ID for communication with base station
const uint32_t dev_id = 0x00000065;

const static riotee_spic_cfg_t spic_cfg = {.mode = SPIC_MODE0_CPOL0_CPHA0,
                                           .frequency = SPIC_FREQUENCY_K500,
                                           .pin_cs = PIN_D8,
                                           .pin_sck = PIN_D10,
                                           .pin_copi = PIN_D9,
                                           .pin_cipo = SPIC_PIN_UNUSED};

const static riotee_spis_cfg_t spis_cfg = {.mode = SPIC_MODE1_CPOL0_CPHA1,
                                           .pin_cs_out = PIN_D4,
                                           .pin_cs_in = PIN_D5,
                                           .pin_sck = PIN_D2,
                                           .pin_mosi = PIN_D3};

const static max2769_cfg_t max2769_cfg = {.snapshot_size_bytes = SNAPSHOT_SIZE_BYTES,
                                          .sampling_frequency = MAX2769_SAMPLING_FREQUENCY_M4,
                                          .adc_resolution = MAX2769_ADC_RESOLUTION_1B,
                                          .min_power_option = MAX2769_MIN_POWER_OPTION_DISABLE,
                                          .pin_pe = PIN_D7};

/* This gets called one time after flashing new firmware */
void bootstrap_callback(void) {
  reset_rtc();
}

/* This gets called after every reset */
void reset_callback(void) {
  //Initialize spi master to configure max2769
  spic_init(&spic_cfg);
  //Initialize spi slave for receiving serial data from max2769
  spis_init(&spis_cfg);
  //Initialize global variables and pins for max2769 usage
  max2769_init(&max2769_cfg);
  //Initialize BLE Frontend for communication with base station
  init_snapshot_transmitter(dev_id);
  //Check that RTC is available
  rtc_init();
}

/* This gets called when capacitor voltage gets low */
void turnoff_callback(void) {
  disable_max2769(&max2769_cfg);
}

int main(void) {
  for (;;) {
    //Capture a GNSS snapshot and take a timestamp
    // riotee_wait_cap_charged();
    // get_timestamped_snapshot(&max2769_cfg, snapshot_buf, &capture_timestamp);
    // riotee_wait_cap_charged();
    // for(int k = 0; k < max2769_cfg.snapshot_size_bytes; k++)
	  // {
		//   printf_("%02X\n", snapshot_buf[k]);
	  // }

    //For Testpurpose write incrementing numbers in snapshot buffer and take timestamp
    riotee_wait_cap_charged();
    get_timestamp(&capture_timestamp);
    for(int k=0;k<SNAPSHOT_SIZE_BYTES;k++)
    {
      snapshot_buf[k] = (uint8_t) k%256;
    }
    //Take another timestamp and send both timestamps to base station to allow recalculation of snapshot caputre time
    riotee_wait_cap_charged();
    take_timestamp_and_send_first_frame(&capture_timestamp, &transmit_timestamp, snapshot_id);
    //Divide snapshot into several frames and send them one after another
    for(uint16_t frame_number=1;frame_number<TOTAL_NUMBER_FRAMES;frame_number++)
    {
      riotee_wait_cap_charged();
      send_snapshot_data_frame(snapshot_buf, frame_number, snapshot_id);
    }
    //Next snapshot gets incremented ID
    snapshot_id++;
  }
}
