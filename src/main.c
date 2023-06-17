
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_spic.h"
#include "riotee_spis.h"
#include "max2769.h"

//Define the size of snapshots to be received from max2769 in bytes
//Snapshot size depends on sampling frequency, snapshot duration and adc resolution
#define SNAPSHOT_SIZE_BYTES 6138    // 4092000 Hz x 0.012s / 8 = 6138 Bytes

//Global buffer to store snapshot
uint8_t snapshot_buf[SNAPSHOT_SIZE_BYTES] __VOLATILE_UNINITIALIZED;


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




/* This gets called after every reset */
void reset_callback(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);
  //Initialize spi master to configure max2769
  spic_init(&spic_cfg);
  //Initialize spi slave for receiving serial data from max2769
  spis_init(&spis_cfg);
  //Initialize for max2769 usage
  max2769_init(&max2769_cfg);
}

/* This gets called when capacitor voltage gets low */
void turnoff_callback(void) {
  riotee_gpio_clear(PIN_LED_CTRL);
  disable_max2769(&max2769_cfg);
}

int main(void) {
  for (;;) {
    riotee_wait_cap_charged();
    //riotee_gpio_set(PIN_LED_CTRL);
    enable_max2769(&max2769_cfg);
    configure_max2769(&max2769_cfg);
    max2769_capture_snapshot(&max2769_cfg, snapshot_buf);
    disable_max2769(&max2769_cfg);
  }
}
