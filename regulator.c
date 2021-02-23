/*
 * regulator.c
 *
 * Created: 09.11.2020 20:44:50
 *  Author: stoff
 */ 

#define LOGGING

#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "uart.h"

volatile uint8_t show_temps = 0;
volatile uint8_t delta1;
volatile uint8_t delta2;
volatile int16_t temp_dach = 0;
volatile int16_t temp_kessel = 0;
volatile uint8_t loop_cnt = 0;
volatile int16_t d_teta = 0;
volatile uint8_t k = 2;
volatile uint8_t duty = 0;
volatile uint8_t log_counter = 0;

void regulator_init()
{
	delta1 = eeprom_read_byte((uint8_t*)0);
	delta2 = eeprom_read_byte((uint8_t*)1);
	
	ADMUX = 0x00;
	ADMUX &= ~(1<<ADLAR);			  //enable left-alignment
	ADCSRA |= (1<<ADEN);			  //enable ADC
	
	TCCR2 |= (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21) | (1<<WGM20) | (1<<COM21) ;  //setting prescaler to /1024; setting mode to Fast PWM.
	TIMSK |= (1<<TOIE2);						 //enable overflow interrupt for TIM2
	
	///only for testing ///
	TIMSK |= (1<<OCIE2);
	DDRD |= (1<<PD6);
	///////////////////////
	
	
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
	return 255;
}


//sensor: 1=dach; 0=kessel
int16_t measure_temp(uint8_t sensor)
{
	ADMUX &= ~0x1F;							//clear MUX4:0
	
	if(sensor == 0)
		ADMUX |= (1<<MUX0);					//set ADC to CH1. If CH2 has to be sampled MUX[4:0] is alreadyy 0, because it was rsetted above
		
	ADCSRA |= (1<<ADSC);					//start conversion
	
	while( (ADCSRA & (1<<ADSC)) > 0 )		//wait for end of conversion
	{
	}
	
	uint16_t adc_val = ADCL;				//get conversion result low-byte
	adc_val += (ADCH<<8);					//and high-byte
	
	
	float voltage = adc_val * 4.854e-3f;									//get voltage from ADC-values	
	float temp_f = (7382.06f - voltage*2751.75f)/(voltage - 29.323f);		//get temperature from voltage	
	int16_t temp = (int16_t) roundf( temp_f );								//round temperature and cast it to int
	
	if(sensor == 1)
	{
		printf("%d;%.3f;%.3f;%d\n", adc_val, voltage, temp_f, temp);
	}

	return temp;
}

//sensor: 1=dach; 0=kessel
int16_t get_temp(uint8_t sensor)
{
	if(sensor == 1)
	{
		return temp_dach;
	}
	else if(sensor == 0)
	{
		return temp_kessel;
	}
	else
	{
		return -999;
	}
}


//sets duty cycle; 0-100%
void set_PWM(uint8_t duty)
{
	OCR2 = (uint8_t)((duty/100.0f)*255);
}



ISR(TIMER2_OVF_vect)
{
	/// only for testing ///
	if(OCR2 > 35)
	{
		PORTD |= (1<<PD6);
	}
	////////////////////////
	
	
	cli();
	
	if(loop_cnt < 62) //if 2s are not passed
	{
		loop_cnt++;
	}
	
	else	//if it is time to work...
	{
		if (log_counter == 255)
		{
			log_counter = 0;
		}
		
		PORTD ^= (1<<PD5);

		temp_dach   = measure_temp(1) - 3; //-3 to compensate the wires
		temp_kessel = measure_temp(0);
		
		
		d_teta = temp_dach - temp_kessel;
	
		if(d_teta > delta1)
		{
			if( d_teta >= delta2)
			{
				if((d_teta * k) >= 0)
				{
					duty = (uint8_t) d_teta * k; //50 Kevlin -> 100% PWM
					if( duty > 100)
					{
						duty = 100;
					}
				}
				duty = 0;
			}
			else
			{
				duty = 0;
			}
		}
		else
		{
			duty = 0;
		}
		
		set_PWM(duty);
	
		loop_cnt = 0;
		log_counter ++;
		
		
		#ifdef LOGGING
		if(log_counter==30) //every 60 secounds
		{
			log_counter = 0;
			printf("%d;%d;%d\n", temp_dach, temp_kessel, duty);
		}
		#endif
	}
	
	
	
	
	TIFR &= ~(1<<TOV2);			//clear flag
		
	sei();
}


/// only for testing ///
ISR(TIMER2_COMP_vect)
{
	PORTD &= ~(1<<PD6);
	TIFR &= ~(1<<OCF2);			//clear flag
}
///////////////////////