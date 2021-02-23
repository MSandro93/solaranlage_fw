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

#define WDI_PORT PORTC
#define WDI_PIN PC6

volatile uint8_t state = 0;

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);



int main(void)
{
	PORTA = 0x00;
	PORTB = 0x00;
	
	DDRA = 0xFC;
	DDRB = 0xFF;
	DDRC |= (1<<WDI_PIN);
	DDRD |= (1<<PD5);
	
	uart_init(0);
	
	
	stdout = &mystdout;
	
	printf("startup!\n");
	
	regulator_init();
	SevenSeg_init();
	Timeout_init();
	Encoder_init();
	
	set_PWM(63);
	
    /* Replace with your application code */
    while (1) 
    {
		switch(state)
		{
			case 0:
			{
				if(SevenSeg_get_state() == 1)  //disable display only if it is on
					SevenSeg_off();		
				setState(1);
				break;
			}
			
			case 1:
			{
				if(SevenSeg_get_state() == 1)  //disable display only if it is on
					SevenSeg_off();		
				break;
			}
				
			case 2:
			{
				if(get_temp(1) >= 0)	
					SevenSeg_set_val(1, get_temp(1));
				else
					SevenSeg_set_val(1, get_temp(0));
				
				SevenSeg_set_val(0, get_temp(0));
				if(SevenSeg_get_state() == 0)  //enable display only if it is off
					SevenSeg_on();
				break;
			}
			
			case 3: 
			{
				SevenSeg_set_val(1, get_delta(1));
				SevenSeg_set_val(0, 1000); //set Kessel-display off
				if(SevenSeg_get_state() == 0)  //enable display only if it is off
					SevenSeg_on();
				break;
			}
			
			case 4:
			{
				SevenSeg_set_val(1, 1000); //set Dach-display off
				SevenSeg_set_val(0, get_delta(2));
				if(SevenSeg_get_state() == 0)  //enable display only if it is off
				SevenSeg_on();
				break;
			}
		}
		
		WDI_PORT ^= (1<<WDI_PIN);
    }
}

void setState(uint8_t s)
{
	state = s;
	printf("state -> %d\n", state);
}

uint8_t getState()
{
	return state;
}


