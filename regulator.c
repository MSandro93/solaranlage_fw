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
#include "main.h"

volatile uint8_t show_temps = 0;
volatile uint8_t delta1;
volatile uint8_t delta2;
volatile int16_t temp_dach = 0;
volatile int16_t temp_kessel = 0;
volatile uint8_t loop_cnt = 0;
volatile int16_t d_teta = 0;
volatile uint8_t duty = 0;
volatile uint8_t log_counter = 0;
volatile uint8_t comming_from_high_temp = 0;
volatile float   k = 2.0f;
volatile uint8_t current_mode = MODE_AUTO;

#define OVERSAMPLING_CNT 10

void regulator_init()
{
	delta1 = eeprom_read_byte((uint8_t*)0);
	delta2 = eeprom_read_byte((uint8_t*)1);
	     k = eeprom_read_float((float*)2);
	
	if(delta1 > 70)						//clip if a non plausible value was loaded from the EEPROM. (e.g. after flashing the fw, 255 is readed because the EEPROM was ereased before programing.
	{
		delta1 = 70;
		eeprom_update_byte((uint8_t*)0, 70);
		
	}
	
	if(delta2 > 70)						//clip if a non plausible value was loaded from the EEPROM. (e.g. after flashing the fw, 255 is readed because the EEPROM was ereased before programing.
	{
		delta1 = 70;
		eeprom_update_byte((uint8_t*)1, 70);
	}
	
	if((k > 9.99f) | (k < 0.00f) | (k!=k))	 //clip if a non plausible value was loaded from the EEPROM. (e.g. after flashing the fw, 255 is readed because the EEPROM was ereased before programing.
	{										 //"(k!=k)": "ccording to the IEEE standard, NaN values have the odd property that comparisons involving them are always false. That is, for a float f, f != f will be true only if f is NaN." (stackoverflow)
		k = 2.00f;
		eeprom_update_float((float*)2, 2.00f);
	}
	
	
	ADMUX = 0x00;
	ADMUX &= ~(1<<ADLAR);			  //enable left-alignment
	ADCSRA |= (1<<ADEN);			  //enable ADC
	
	TCCR2 |= (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21) | (1<<WGM20) | (1<<COM21) ;  //setting prescaler to /1024; setting mode to Fast PWM.
	TIMSK |= (1<<TOIE2);						 //enable overflow interrupt for TIM2
	
	DDRD |= (1<<PD7);				//set the PWM-output pin to output-mode
	
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
	uint32_t adc_val_avg = 0;
	uint16_t adc_val = 0;
	
	for(uint8_t i=0; i < OVERSAMPLING_CNT; i++)
	{
		ADMUX &= ~0x1F;							//clear MUX4:0
		
		if(sensor == 0)
		ADMUX |= (1<<MUX0);					//set ADC to CH1. If CH2 has to be sampled MUX[4:0] is already 0, because it was restted above
		
		ADCSRA |= (1<<ADSC);					//start conversion
		
		while( (ADCSRA & (1<<ADSC)) > 0 )		//wait for end of conversion
		{
		}
		
		adc_val = ADCL;							//get conversion result low-byte
		adc_val += (ADCH<<8);					//and high-byte
		
		adc_val_avg += adc_val;					//sum up ADC values for oversampling
	}
	
	adc_val = (uint16_t)(adc_val_avg / OVERSAMPLING_CNT); //calculate average with oversampling count
	
	float voltage = adc_val * 4.854e-3f;									//get voltage from ADC-values
	float temp_f = (7382.06f - voltage*2751.75f)/(voltage - 29.323f);		//get temperature from voltage
	int16_t temp = (int16_t) roundf( temp_f );								//round temperature and cast it to int

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


float get_k()
{
	return k;
}

void set_k(float f)
{
	k = f;
}

uint8_t get_current_mode()
{
	return current_mode;
}

void set_current_mode(uint8_t m)
{
	current_mode = m;
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
			comming_from_high_temp = 1;						//if we are coming from a temperature difference higher than delta1, set flag
		}
	
		if(comming_from_high_temp == 1)						//if we are coming from a temperature difference higher than delta1
		{
			if( d_teta >= delta2)							//if we are still above delta 2
			{
				if((d_teta * k) >= 0)						//if d_teta is negative the multiplication with k will lead to a negative duty cycle, so thsi has to be handled
				{
					duty = (uint8_t) d_teta * k;			//50 Kevlin -> 100% PWM
					if( duty > 100)
					{
						duty = 100;
					}
				}
				else										//if we are finally at delta2 or lower, reset flag. Only if we get above delta1 again, the pump is going to be activatda gain
				{
					duty = 0;
				}
			}
			else
			{
				duty = 0;
				comming_from_high_temp = 0;
			}
		}



		switch (current_mode)
		{			
			case MODE_ON:
			{
				duty = 100;
				break;
			}
			case MODE_OFF:
			{
				duty = 0;
				break;
			}
			case MODE_AUTO:
			{
				break;
			}
			default:
				break;
		}
		
		
		set_PWM(duty);
		
	
		loop_cnt = 0;

		// Enable logging with define		
		#ifdef LOGGING
			printf("%d,%d,%d,%d\r\n", temp_dach, temp_kessel, d_teta, duty);
		#endif
	}
	
	TIFR &= ~(1<<TOV2);			//clear flag
		
	sei();
}
