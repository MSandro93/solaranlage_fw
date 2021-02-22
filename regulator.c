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
volatile uint8_t delta1;
volatile uint8_t delta2;
volatile uint8_t temp_dach = 0;
volatile uint8_t temp_kessel = 0;
volatile uint8_t loop_cnt = 0;

void regulator_init()
{
	delta1 = eeprom_read_byte(0);
	delta2 = eeprom_read_byte(1);
	
	ADMUX = 0x00;
	ADMUX &= ~(1<<ADLAR);			  //enable left-alignment
	ADCSRA |= (1<<ADEN);			  //enable ADC
	
	TCCR2 |= (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21) | (1<<WGM20) | (1<<COM21) ;  //setting prescaler to /1024; setting mode to Fast PWM.
	TIMSK |= (1<<TOIE2);						 //enable overflow interrupt for TIM2
	
	DDRD |= (1<<PD7); //the PWM-output to output-mode
	
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

void inc_delta(uint8_t i_)
{
	if(i_==1)
	{
		if(delta1<100)
			delta1++;
	}
	
	if(i_==2)
	{
		if(delta2<100)
			delta2++;
	}			
}

void dec_delta(uint8_t i_)
{
	if(i_==1)
	{
		if(delta1>0)
			delta1--;
	}
	
	if(i_==2)
	{
		if(delta2>0)
			delta2--;
	}
}

uint8_t get_delta(uint8_t i_)
{
	if(i_==1)
	{
		return delta1;
	}
	if(i_==2)
	{
		return delta2;
	}
}


//sensor: 1=dach; 0=kessel
uint8_t measure_temp(uint8_t sensor)
{
	ADMUX &= ~0x1F;							//clear MUX4:0
	
	if(sensor == 0)
		ADMUX |= (1<<MUX0);					//set ADC to CH1
		
	ADCSRA |= (1<<ADSC);					//start conversion
	
	while( (ADCSRA & (1<<ADSC)) > 0 )		//wait for end of conversion
	{
	}
	
	uint16_t adc_val = ADCL;				//get conversion result low-byte
	adc_val += (ADCH<<8);					//and high-byte
	
	
	float voltage = adc_val * 4.854e-3f;	//get voltage from ADC-values
	
	float temp_f = (7382.06f - voltage*2751.75f)/(voltage - 29.323f);		//get temperature from voltage
	
	uint8_t temp = (uint8_t) roundf( temp_f );						//round temperature and cast it to int
	
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
		return 255;
}


//sets duty cycle; 0-255
void set_PWM(uint8_t duty)
{
	OCR2 = duty;
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