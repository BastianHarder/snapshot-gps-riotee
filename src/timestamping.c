#include "timestamping.h"
#include <stdint.h>
#include <time.h>
#include "riotee_am1805.h"

int get_timestamp(struct tm* timestamp)
{
    int res = 0;
    res = am1805_init();
    res += am1805_get_datetime(timestamp);
    return res;
}

//Function that calculate time1 - time2
// https://www.tutorialspoint.com/c_standard_library/c_function_difftime.htm
void get_time_difference(time_t time2, time_t time1)
{
    difftime (time2, time1);
}

