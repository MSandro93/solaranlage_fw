/*
 * regulator.h
 *
 * Created: 09.11.2020 20:44:57
 *  Author: stoff
 */ 


#ifndef REGULATOR_H_
#define REGULATOR_H_

void regulator_init(void);
void enable_show_temps(void);
void disable_show_temps(void);
void inc_delta();
void dec_delta();
uint8_t get_delta();
uint8_t get_temp(uint8_t sensor);

#endif /* REGULATOR_H_ */