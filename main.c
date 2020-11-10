/*
 * solaranlage_fw.c
 *
 * Created: 09.11.2020 17:55:05
 * Author : stoff
 */ 

#include <avr/io.h>
#include "main.h"
#include "7seg.h"
#include "encoder.h"
#include "regulator.h"
#include "uart.h"
#include "timeout.h"

#define WDI_PORT PORTC
#define WDI_PIN PC6

uint8_t state = 0;


int main(void)
{
	PORTA = 0x00;
	PORTB = 0x00;
	
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC |= (1<<WDI_PIN);
	DDRD |= (1<<PD5);
	
	uart_init(0);
	regulator_init();
	
	SevenSeg_init();
	SevenSeg_set_val(1, 123);
	SevenSeg_set_val(0, 1000);
	
	Timeout_init();
	Encoder_init();
	
    /* Replace with your application code */
    while (1) 
    {
		switch(state)
		{
			case 0:
			{
				SevenSeg_off();
				setState(1);
				break;
			}
			
			case 1:
			{
				SevenSeg_off();
				break;
			}
				
			case 2:
			{
				SevenSeg_set_val(1, get_temp(1));
				SevenSeg_set_val(0, get_temp(0));
				SevenSeg_on();
				break;
			}
			
			case 3: 
			{
				SevenSeg_set_val(1, get_delta());
				SevenSeg_set_val(0, 1000); //set Kessel-display off
				break;
			}
		}
		
		WDI_PORT ^= (1<<WDI_PIN);
    }
}

void setState(uint8_t s)
{
	state = s;
	uart_send_blocking(s);
}

uint8_t getState()
{
	return state;
}


