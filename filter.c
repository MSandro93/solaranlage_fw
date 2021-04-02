/**
 * @file fir_filter_moving_avg8_uint.h
 * @author stfwi
 *
 * FIR FILTER EXAMPLE, MOVING AVERAGE IMPLEMENTATION
 *
 *
**/

#include <stdint.h>
 
/**
 * The size of our filter. We specify it in bits
 */
#define FILTER_SIZE_IN_BITS (4)
#define FILTER_SIZE ( 1 << (FILTER_SIZE_IN_BITS) )
 
/**
 *
 */
uint8_t filter_buffer[2][FILTER_SIZE];
 
/**
 * Here we store the sum of all history values
 */
uint16_t filter_sum[2];
 
 
/**
 * This is the actual position of the ring buffer.
 */
uint8_t *filter_position[2];
 
/**
 * Here we initialise our filter. In this case we only set the
 * initial position and set all buffer values to zero.
 */
void filter_init(uint8_t filter)
{
  filter_sum[filter] = 0;
  filter_position[filter] = filter_buffer[filter];
  // alternatively memset(filter_buffer, 0, sizeof(filter_buffer) * sizeof(unsigned int))
  for(uint8_t i=0; i<FILTER_SIZE; i++)
  {
	  filter_buffer[filter][i] = 0;
  }
}
 
/**
 * This function is called every cycle. It adds substracts the oldest value from
 * the sum, adds the new value to the sum, overwrites the oldest value with the
 * new value and increments the ring buffer pointer (with rewind on overflow).
 *
 * @param double new_value
 * @return double
 */
uint8_t filter(uint8_t new_value, uint8_t filter)
{
  // Substract oldest value from the sum
  filter_sum[filter] -= *filter_position[filter];
 
  // Update ring buffer (overwrite oldest value with new one)
  *filter_position[filter] = new_value;
 
  // Add new value to the sum
  filter_sum[filter] += new_value;
 
  // Advance the buffer write position, rewind to the beginning if needed.
  if(++filter_position[filter] >= filter_buffer[filter] + FILTER_SIZE)
  {
    filter_position[filter] = filter_buffer[filter];
  }
 
  // Return sum divided by FILTER_SIZE, which is faster done by right shifting
  // The size of the ring buffer in bits. ( filter_sum / 2^bits ).
  return (uint8_t) (filter_sum[filter] >> FILTER_SIZE_IN_BITS);
}