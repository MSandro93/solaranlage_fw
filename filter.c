/**
 * @file fir_filter_moving_avg8_uint.h
 * @author stfwi
 *
 * FIR FILTER EXAMPLE, MOVING AVERAGE IMPLEMENTATION
 *
 *
**/

#include <stdint.h>
#include <math.h>
 
/**
 * The size of our filter. We specify it in bits
 */
#define FILTER_SIZE_IN_BITS (4)
#define FILTER_SIZE ( 1 << (FILTER_SIZE_IN_BITS) )


int16_t filter_buffer[2][FILTER_SIZE];
int16_t filter_sum[2];
int16_t *filter_position[2];
 

void filter_init(uint8_t filter)
{
  filter_sum[filter] = 0;
  filter_position[filter] = filter_buffer[filter];
  for(uint8_t i=0; i<FILTER_SIZE; i++)
  {
	  filter_buffer[filter][i] = 0;
  }
}
 

int16_t filter(int16_t new_value, uint8_t filter)
{
  filter_sum[filter] -= *filter_position[filter];
 
  *filter_position[filter] = new_value;
 
  filter_sum[filter] += new_value;
 
 
  if(++filter_position[filter] >= filter_buffer[filter] + FILTER_SIZE)
  {
    filter_position[filter] = filter_buffer[filter];
  }
 
  return (int16_t) round( filter_sum[filter]  / (FILTER_SIZE+0.0f) );
}