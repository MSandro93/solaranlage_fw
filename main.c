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
	extGPOS_clearAll();
	
	
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
		switch(state)
		{
			case INIT:
			{
				if(SevenSeg_get_state() == 1)						//disable display only if it is on
				{
					SevenSeg_off();
				}
				setState(DISPLAY_OFF);
				
				extGPOS_clearAll();
				extGPO_update();
				break;
			}
			
			case DISPLAY_OFF:
			{
				if(SevenSeg_get_state() == 1)						//disable display only if it is on
				{
					SevenSeg_off();
				}
				
				extGPOS_clearAll();
				extGPO_update();
				break;
			}
				
			case SHOW_TEMPS:
			{				
				SevenSeg_set_val(0, get_temp(0));
				SevenSeg_set_val(1, get_temp(1));			

				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAll();
				extGPO_switch(LED_TEMP_DACH, EXGPO_ON);
				extGPO_switch(LED_TEMP_KELLER, EXGPO_ON);
				extGPO_update();
				break;
			}
			
			case MODIFY_DELTA1: 
			{
				SevenSeg_set_val(1, get_delta(1));
				SevenSeg_set_val(0, 1000);							//set Kessel-display off
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAll();
				extGPO_switch(LED_DELTA1, EXGPO_ON);
				extGPO_update();
				break;
			}
			
			case MODIFY_DELTA2:
			{
				SevenSeg_set_val(1, 1000);							//set Dach-display off
				SevenSeg_set_val(0, get_delta(2));
				if(SevenSeg_get_state() == 0)						//enable display only if it is off
				{
					SevenSeg_on();
				}
				
				extGPOS_clearAll();
				extGPO_switch(LED_DELTA2, EXGPO_ON);
				extGPO_update();
				break;
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


