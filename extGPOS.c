#include <avr/io.h>
#include "extGPOS.h"


#define SR_DATA_PORT	PORTC
#define SR_CLOCK_PORT	PORTC
#define SR_LATCH_PORT	PORTC

#define SR_DATA_DDR		DDRC
#define SR_CLOCK_DDR	DDRC
#define SR_LATCH_DDR	DDRC

#define SR_DATA_PIN		PC0
#define SR_CLOCK_PIN	PC1
#define SR_LATCH_PIN	PC7


uint8_t buff = 0;


void extGPOs_init(void)
{
	SR_DATA_DDR  |= (1<<SR_DATA_PIN);			//set SR_DATA pin to output mode
	SR_CLOCK_DDR |= (1<<SR_CLOCK_PIN);			//set SR_CLOCK pin to output mode
	SR_LATCH_DDR |= (1<<SR_LATCH_PIN);			//set SR_LATCH pin to output mode
	
	SR_CLOCK_PORT &= ~(1<<SR_CLOCK_PIN);		//initial SR_CLOCK with LOW
	SR_DATA_PORT  &= ~(1<<SR_DATA_PIN);			//initial SR_DATA with LOW
	SR_LATCH_PORT &= ~(1<<SR_LATCH_PIN);		//initial SR_DATA with LOW
}

void extGPOS_clearAll(void)
{
	buff = 0;
}

uint8_t extGPO_switch(extGPO ego,  extGPO_state s)
{
	if (s == EXGPO_OFF)
	{
		buff &= ~(1<<ego);
		return 0;
	}
	else if (s == EXGPO_ON)
	{
		buff |= (1<<ego);
		return 0;
	}
	else
	{
		return 1;
	}
}


void extGPO_update()
{
	for(int8_t i=7; i>=0; i--)						//eight times do..
	{
		if ((buff & (1<<i)) > 0)
		{
			SR_DATA_PORT |= (1<<SR_DATA_PIN);
		}
		else
		{
			SR_DATA_PORT &= ~(1<<SR_DATA_PIN);
		}
		
		SR_CLOCK_PORT |=  (1<<SR_CLOCK_PIN);	//toggle...
		SR_CLOCK_PORT &= ~(1<<SR_CLOCK_PIN);    //SR_CLOCK HIGH all the way through the complete shift-register.
	}
		
	SR_DATA_PORT &= ~(1<<SR_DATA_PIN);			//reset SR_DATA to LOW
			
	SR_LATCH_PORT |=  (1<<SR_LATCH_PIN);		//update the outputs of the shift-register...
	SR_LATCH_PORT &= ~(1<<SR_LATCH_PIN);		//by toggling SR_LATCH
}

