/*
 * regulator.c
 *
 * Created: 09.11.2020 20:44:50
 *  Author: stoff
 */ 
#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

volatile uint8_t show_temps = 0;
volatile uint8_t delta;
volatile uint8_t temp_dach = 0;
volatile uint8_t temp_kessel = 0;
volatile uint8_t loop_cnt = 0;

void regulator_init()
{
	delta = eeprom_read_byte(0);
	
	
	
	TCCR2 |= (1<<CS22) | (1<<CS21) | (1<<CS20);  //setting prescaler to /1024
	TIMSK |= (1<<TOIE2);						 //enable overflow interrupt for TIM2
	sei();
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
		return temp_kessel;	//dummy
	else
		return temp_dach;	//dummy
}



ISR(TIMER2_OVF_vect)
{
	cli();
	
	if(loop_cnt < 62) //if 5s are not passed
	{
		loop_cnt++;
	}
	
	else	//if it is time to work...
	{
		PORTD ^= (1<<PD5);
		
		temp_dach   = get_temp(1);
		temp_kessel = get_temp(0);
		
		loop_cnt = 0;
	}
	
	
	TIFR &= ~(1<<TOV2);			//clear flag
		
	sei();
}