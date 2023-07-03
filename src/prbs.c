//PRBS Code adapted from:
// https://gist.github.com/Btremaine/c3a48e0baab539e0cb562635f0e6ecc6#file-prbs-c

#include <stdint.h>

void prbs_gen(uint8_t *buffer, int buffer_length, uint8_t seed)
{
    uint8_t a = seed;  
    for (int i = 0;i<buffer_length; i++) 
    {
        int newbit = (((a >> 6) ^ (a >> 5)) & 1);
        a = ((a << 1) | newbit) & 0x7f;
        buffer[i] = a;
    }
}

void increment_gen(uint8_t *buffer, int buffer_length)
{
    for(int k=0;k<buffer_length;k++)
    {
      int packet_idx = k/243;
      int number = k%243;
      if((number==0) | (number==242) | (number==62))
      {
        buffer[k] = (uint8_t) packet_idx;
      }
      else
      {
        buffer[k] = (uint8_t) number;
      }
    }
}