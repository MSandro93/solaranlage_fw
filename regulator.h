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
void inc_delta(uint8_t i_);
void dec_delta(uint8_t i_);
uint8_t get_delta(uint8_t i_);
int16_t get_temp(uint8_t sensor);
int16_t measure_temp(uint8_t sensor);
void set_PWM(uint8_t duty);
uint8_t get_PWM();
float get_k(void);
void set_k(float f);
uint8_t get_current_mode(void);
void set_current_mode(uint8_t m);
uint8_t get_openLoad(uint8_t seg);

#endif /* REGULATOR_H_ */