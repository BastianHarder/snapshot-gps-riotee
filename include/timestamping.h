#ifndef __TIMESTAMPING_H_
#define __TIMESTAMPING_H_

#include <stdint.h>
#include <time.h>

//structure definition for timestamping snapshots
typedef struct {
  uint8_t hundredths;   
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
  uint8_t wday; 
} timestamp_t;

int get_timestamp(timestamp_t *timestamp);
int reset_rtc();
int rtc_init();

#endif /* __TIMESTAMPING_H_ */