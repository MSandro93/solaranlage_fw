/*
 * encoder.c
 *
 * Created: 09.11.2020 18:36:22
 *  Author: stoff
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "7seg.h"
#include "encoder.h"
#include "regulator.h"
#include "main.h"
#include "timeout.h"

#define ENC_PORT PORTD
#define ENC_PINPORT PIND
#define ENC_A_PIN PD2
#define ENC_B_PIN PD4

uint8_t selected_seg = 1;


void Encoder_init()
{
	ENC_PORT &= ~((1<<ENC_A_PIN) | (1<<ENC_A_PIN));							//setting encoder terminals to input
	MCUCR |= (1<<ISC01) | (1<<ISC00) | (1<<ISC11);				//setting ext-int0 to sensitive at rising edge
	GICR |= (1<<INT0) | (1<<INT1);											//enable external interrupt 0 (ENC_A_PIN)
	sei();
}



ISR(INT0_vect)
{	
	cli();
	
	switch(getState())
	{
		case 3:	
		{	
			if( (ENC_PINPORT & (1<<ENC_B_PIN)) > 0) //if ENC_B_PIN == 1  //gegen den Uhrzeigersinn
			{
				inc_delta();
			}		
			else									//im Uhrzeigersinn
			{				
				dec_delta();
			}
		}
		default:
			break;
	}
	
	stop_timeout_timer();
	start_timeout_timer();
	
	
	GIFR &= ~(1<<INTF0);  //clear ext-interrupt-0 flag
	
	sei();
}

ISR(INT1_vect)  //if the encoder got pushed
{
	cli();
	
	switch(getState())
	{
		case 1:
		{
			setState(2);
			start_timeout_timer();
			break;
		}
		case 2:
		{
			setState(3);
			start_timeout_timer();
			break;
		}
		case 3:
		{
			if((getState() == 3) && (eeprom_read_byte(0) != get_delta()))
			{
				eeprom_update_byte(0, get_delta());
			}
			setState(2);
			break;
		}
	}
	
	sei();
}