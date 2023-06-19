#include "timestamping.h"
#include <stdint.h>
#include <time.h>
#include "riotee_am1805.h"

int get_timestamp(timestamp_t *timestamp)
{
    int result = 0;
    struct tm t;
    result += am1805_get_datetime_and_hundredths(&t);
    timestamp->hundredths = t.tm_isdst;
    timestamp->second = t.tm_sec;
    timestamp->minute = t.tm_min;
    timestamp->hour = t.tm_hour;
    timestamp->day = t.tm_mday;
    timestamp->month = t.tm_mon;
    timestamp->year = t.tm_year;
    timestamp->wday = t.tm_wday;
    return result;
}

int reset_rtc()
{
    int result = 0;
    struct tm init_time =  {.tm_sec = 0,
                            .tm_min = 0,
                            .tm_hour = 0,
                            .tm_mday = 0,
                            .tm_mon = 0,
                            .tm_year = 0,
                            .tm_wday = 0,
                            .tm_yday = 0,
                            .tm_isdst = 0};
    //Establish connection to RTC
    result += am1805_init();
    //Initialize RTC with all zeros
    result += am1805_set_datetime(&init_time);
    return result;
}

int rtc_init()
{
    return am1805_init();
}

