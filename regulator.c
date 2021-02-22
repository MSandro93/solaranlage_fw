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
#include <math.h>
#include "uart.h"

volatile uint8_t show_temps = 0;
volatile uint8_t delta;
volatile uint8_t temp_dach = 0;
volatile uint8_t temp_kessel = 0;
volatile uint8_t loop_cnt = 0;

void regulator_init()
{
	delta = eeprom_read_byte(0);
	
	ADMUX = 0x00;
	ADMUX &= ~(1<<ADLAR);			  //enable left-alignment
	ADCSRA |= (1<<ADEN);			  //enable ADC
	
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
uint8_t measure_temp(uint8_t sensor)
{
	ADMUX &= ~0x1F; //clear MUX4:0
	
	if(sensor == 0)
		ADMUX |= (1<<MUX0);	//set ADC to CH1
		
	ADCSRA |= (1<<ADSC);				  //start conversion
	
	while( (ADCSRA & (1<<ADSC)) > 0 ) //wait for end of conversion
	{
	}
	
	uint16_t adc_val = ADCL;
	adc_val += (ADCH<<8);
	
	
	float voltage = adc_val * 4.8828e-3f;
	
	float temp_f = 2.5f*((1000*voltage)/(5-voltage)) - 251.75f;
	
	uint8_t temp = (uint8_t) roundf( temp_f );
	
	printf("sensor %d: ADC-val= %d, temp= %d\n", sensor, adc_val, temp);
		
	return temp;
}

//sensor: 1=dach; 0=kessel
uint8_t get_temp(uint8_t sensor)
{
	if(sensor == 1)
		return temp_dach;
	else if(sensor == 0)
		return temp_kessel;
	else
		return 1000;
}



ISR(TIMER2_OVF_vect)
{
	cli();
	
	if(loop_cnt < 62) //if 2s are not passed
	{
		loop_cnt++;
	}
	
	else	//if it is time to work...
	{
		PORTD ^= (1<<PD5);

		temp_dach   = measure_temp(1);
		temp_kessel = measure_temp(0);
		
		loop_cnt = 0;
	}
	
	
	TIFR &= ~(1<<TOV2);			//clear flag
		
	sei();
}