//PRBS Code adapted from:
// https://gist.github.com/Btremaine/c3a48e0baab539e0cb562635f0e6ecc6#file-prbs-c

#ifndef __PRBS_H_
#define __PRBS_H_

#include <stdint.h>

void prbs_gen(uint8_t *buffer, int buffer_length, uint8_t seed);
void increment_gen(uint8_t *buffer, int buffer_length);

#endif /* __PRBS_H_ */