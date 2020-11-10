/*
 * regulator.c
 *
 * Created: 09.11.2020 20:44:50
 *  Author: stoff
 */ 
#include <stdint.h>
#include <avr/eeprom.h>

uint8_t show_temps = 0;
uint8_t delta;

void regulator_init()
{
	delta = eeprom_read_byte(0);
}

void enable_show_temps(void)
{
	show_temps = 1;
}

void disable_show_temps(void)
{
	show_temps = 0;
}

void inc_delta()
{
	if(delta<100)
		delta++;
}

void dec_delta()
{
	if(delta>0)
		delta --;
}

uint8_t get_delta()
{
	return delta;
}


//sensor: 1=dach; 0=kessel
uint8_t get_temp(uint8_t sensor)
{
	if(sensor==0)
		return 111;	//dummy
	else
		return 222;	//dummy
}