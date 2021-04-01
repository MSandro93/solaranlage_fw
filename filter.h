/*
 * filter.h
 *
 * Created: 01.04.2021 11:53:06
 *  @author stfwi
 */ 


#ifndef FILTER_H_
#define FILTER_H_


/**
 * Here we initialise our filter. In this case we only set the
 * initial position and set all buffer values to zero.
 */
void filter_init();
 
/**
 * This function is called every cycle. It adds substracts the oldest value from
 * the sum, adds the new value to the sum, overwrites the oldest value with the
 * new value and increments the ring buffer pointer (with rewind on overflow).
 *
 * @param double new_value
 * @return double
 */
uint8_t filter(uint8_t new_value);


#endif /* FILTER_H_ */