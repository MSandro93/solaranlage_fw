/*
 * timeout.c
 *
 * Created: 09.11.2020 19:47:47
 *  Author: stoff
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "main.h"
#include "regulator.h"
#include "uart.h"

void Timeout_init()
{
	TCCR1A = 0x00;
	
	TIMSK |= (1<<TOIE1);			//enable overflow-event
	sei();
}
void start_timeout_timer()
{
	TCNT1 = 0;						 //reset timer counter value
	TCCR1B = (1<<CS12) | (1<<CS10);  //start timer at prescaler /1024
}

void stop_timeout_timer()
{
	TCCR1B &= ~((1<<CS12) | (1<<CS10)); //stop timer
}




//timeouted
ISR(TIMER1_OVF_vect)
{	
	cli();
	
	if((getState() == 3) && (eeprom_read_byte((uint8_t*)0) != get_delta(1)))
	{
		eeprom_update_byte((uint8_t*)(0), (uint8_t)get_delta(1));
	}
	
	if((getState() == 4) && (eeprom_read_byte((uint8_t*)1) != get_delta(2)))
	{
		eeprom_update_byte((uint8_t*)(1), (uint8_t)get_delta(2));
	}

	if(eeprom_read_float((float*)2) != get_k())
	{
		eeprom_update_float((float*)2, get_k());
	}	
	
	setState(DISPLAY_OFF);
	
	TCCR1B &= ~((1<<CS12) | (1<<CS10)); //stop timer
	TCNT1 = 0;						   //resetting counter register
	
	
	TIFR &= ~(1<<TOV1);
	sei();
}