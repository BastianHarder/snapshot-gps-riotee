#ifndef __TIMESTAMPING_H_
#define __TIMESTAMPING_H_

#include <stdint.h>
#include <time.h>

int get_timestamp(struct tm* timestamp);
void get_time_difference(time_t time2, time_t time1);

#endif /* __TIMESTAMPING_H_ */