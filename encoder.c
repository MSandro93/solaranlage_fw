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
#include "uart.h"
#include "version.h"

#define ENC_PORT PORTD
#define ENC_PINPORT PIND
#define ENC_A_PIN PD2
#define ENC_B_PIN PD4

uint8_t selected_seg = 1;


void Encoder_init()
{
	ENC_PORT &= ~((1<<ENC_A_PIN) | (1<<ENC_A_PIN));							//setting encoder terminals to input
	MCUCR |= (1<<ISC01) | (1<<ISC00) | (1<<ISC11);							//setting ext-int0 to sensitive at rising edge
	GICR |= (1<<INT0) | (1<<INT1);											//enable external interrupt 0 (ENC_A_PIN)
	sei();
}



ISR(INT0_vect)
{	
	cli();
	
	switch(getState())
	{
		case MODIFY_DELTA1:	
		{	
			if( (ENC_PINPORT & (1<<ENC_B_PIN)) > 0) //gegen den Uhrzeigersinn
			{
				if(get_delta(1) > 0)
					dec_delta(1);					//prevent delta1 from becoming negative
			}		
			else									//im Uhrzeigersinn
			{	
				if(get_delta(1) < 70)
					inc_delta(1);					//prevent delta2 from becoming lager than 70
			}
			break;
		}
		
		case MODIFY_DELTA2:
		{
			if( (ENC_PINPORT & (1<<ENC_B_PIN)) > 0) //gegen den Uhrzeigersinn
			{
				if(get_delta(2) > 0)
					dec_delta(2);					//prevent delta2 from becoming negative
			}
			else									//im Uhrzeigersinn
			{
				if(get_delta(2) < 70)
					inc_delta(2);					//prevent delta2 from becoming lager than 70
			}
			break;
		}
		
		case MODIFY_K:
		{
			if( (ENC_PINPORT & (1<<ENC_B_PIN)) > 0) //gegen den Uhrzeigersinn
			{
				if(get_k() > 0)
				set_k(get_k() - 0.01f);			    //prevent the proportional ration k from becoming negative
			}
			else									//im Uhrzeigersinn
			{
				if(get_k() < 9.99f)
				set_k(get_k() + 0.01f);				//prevent the proportional ration k from becoming lager than 9.99f
			}
			break;
		}
		
		case SWITCH_MODES:
		{
			int8_t m_ = MODE_AUTO;
			
			if( (ENC_PINPORT & (1<<ENC_B_PIN)) > 0) //gegen den Uhrzeigersinn
			{
				m_ = (int8_t)get_current_mode() - 1;
				if(m_ < MODE_AUTO)
				{
					m_ = MODE_OFF;
				}
			}
			else									//im Uhrzeigersinn
			{
				m_ = (int8_t)get_current_mode() + 1;
				if(m_ > MODE_OFF)
				{
					m_ = MODE_AUTO;
				}
			}
			set_current_mode(m_);
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
		case DISPLAY_OFF:
		{
			setState(SHOW_TEMPS);
			
			printf("Tag: %s\n\r", FW_VERSION_TAG);
			printf("Hash: %s\n\r", REPO_VERSION_HASH);
			
			stop_timeout_timer();
			start_timeout_timer();
			break;
		}
		
		case SHOW_TEMPS:
		{
			setState(MODIFY_DELTA1);
			start_timeout_timer();
			break;
		}
		
		case MODIFY_DELTA1:
		{
			if(eeprom_read_byte((uint8_t*)0) != get_delta(1))
			{
				eeprom_update_byte((uint8_t*)0, get_delta(1));
			}
			setState(MODIFY_DELTA2);
			break;
		}
		
		case MODIFY_DELTA2:
		{
			if(eeprom_read_byte((uint8_t*)1) != get_delta(2))
			{
				eeprom_update_byte((uint8_t*)1, get_delta(2));
			}
			setState(MODIFY_K);
			break;
		}
		
		case MODIFY_K:
		{			
			if(eeprom_read_float((float*)2) != get_k())
			{
				eeprom_update_float((float*)2, get_k());
			}
			setState(SWITCH_MODES);
			break;
		}
		
		case SWITCH_MODES:
		{
			switch(get_current_mode())
			{
				case MODE_OFF:
				{
					set_PWM(0);
					break;
				}
				case MODE_ON:
				{
					set_PWM(100);
					break;
				}
			}
			
			setState(SHOW_PWM);
			start_timeout_timer();
			break;
		}
		
		case SHOW_PWM:
		{
			setState(SHOW_TEMPS);
			break;
		}
	}
	
	sei();
}