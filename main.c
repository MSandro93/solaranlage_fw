/*
 * solaranlage_fw.c
 *
 * Created: 09.11.2020 17:55:05
 * Author : stoff
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "main.h"
#include "7seg.h"
#include "encoder.h"
#include "regulator.h"
#include "uart.h"
#include "timeout.h"
#include "version.h"
#include "extGPOS.h"

#define WDI_PORT PORTC
#define WDI_PIN PC6

volatile uint8_t state = 0;

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);



int main(void)
{
	extGPOs_init();
	extGPOS_clearAllLEDs();
	extGPO_switch(BUZZER, EXGPO_OFF);
	extGPO_update();
	
	
	PORTA = 0x00;
	PORTB = 0x00;
	
	DDRA = 0xFC;
	DDRB = 0xFF;
	DDRC |= (1<<WDI_PIN);
	DDRD |= (1<<PD5);
	
	uart_init(0);

	stdout = &mystdout;
	
	printf("##### Solaranlage Version %s Hash %s #####\n", FW_VERSION_TAG, REPO_VERSION_HASH);
	
	regulator_init();
	SevenSeg_init();
	Timeout_init();
	Encoder_init();
	
	
	while(1)	
	{	
		//handling open load of sensors
		if( (get_openLoad(DISPLAY_DACH) == 1) | (get_openLoad(DISPLAY_KESSEL) == 1) )					//if sensor dach open load...
		{
			extGPO_switch(BUZZER, EXGPO_ON);
		}
		else
		{
			extGPO_switch(BUZZER, EXGPO_OFF);
		}
		extGPO_update();
		//
		
		switch(state)
		{
			case INIT:
			{
				if(SevenSeg_get_state() == 1)						//disable display only if it is on
				{
					SevenSeg_off();
				}
				setState(DISPLAY_OFF);
				
				extGPOS_clearAllLEDs();
				extGPO_update();
				break;
			}
			
			case DISPLAY_OFF:
			{
				if(SevenSeg_get_state() == 1)						//disable display only if it is on
				{
					SevenSeg_off();
				}
				
				extGPOS_clearAllLEDs();
				extGPO_update();
				break;
			}
				
			case SHOW_TEMPS:
			{				
			
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				SevenSeg_set_val(DISPLAY_DACH, get_temp(DISPLAY_DACH));
				SevenSeg_set_val(DISPLAY_KESSEL, get_temp(DISPLAY_KESSEL));

				extGPOS_clearAllLEDs();
				extGPO_switch(LED_TEMP_DACH, EXGPO_ON);
				extGPO_switch(LED_TEMP_KELLER, EXGPO_ON);
				extGPO_update();
				break;
			}
			
			case MODIFY_DELTA1: 
			{
				SevenSeg_set_val(DISPLAY_DACH, get_delta(1));
				SevenSeg_set_val(DISPLAY_KESSEL, 1000);							//set Kessel-display off
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAllLEDs();
				extGPO_switch(LED_DELTA1, EXGPO_ON);
				extGPO_update();
				break;
			}
			
			case MODIFY_DELTA2:
			{
				SevenSeg_set_val(DISPLAY_DACH, 1000);							//set Dach-display off
				SevenSeg_set_val(DISPLAY_KESSEL, get_delta(2));
				
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAllLEDs();
				extGPO_switch(LED_DELTA2, EXGPO_ON);
				extGPO_update();
				break;
			}
			
			case MODIFY_K:
			{
				SevenSeg_set_val_f(DISPLAY_KESSEL, get_k());
				SevenSeg_set_val(DISPLAY_DACH, 1000);
				
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAllLEDs();
				extGPO_switch(LED_K_FACTOR, EXGPO_ON);
				extGPO_update();
				break;
			}
			
			case SWITCH_MODES:
			{
				SevenSeg_display_mode(DISPLAY_DACH, get_current_mode());
				SevenSeg_set_val(DISPLAY_KESSEL, 1000);
				
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAllLEDs();
				extGPO_switch(LED_MODE, EXGPO_ON);
				extGPO_update();
				
				break;
			}
			
			default:
			{
				setState(INIT);
				DDRC &= ~(1<<WDI_PIN);  //set WDI-pin to input-mode -> prevents serving of the WD -> forces reset.
			}
		}
		WDI_PORT ^= (1<<WDI_PIN);
	}
}

void setState(uint8_t s)
{
	state = s;
//	printf("state -> %d\n", state);
}

uint8_t getState()
{
	return state;
}


