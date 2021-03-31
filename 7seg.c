/*
 * _7seg.c
 *
 * Created: 09.11.2020 10:25:44
 *  Author: stoff
 */ 
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"


uint8_t patterns[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00, 0x40, 0x78, 0x77, 0x54, 0x5C, 0x71, 0x1C };
	               // '0' , '1' ,  '2', '3' , '4' , '5' , '6' , '7' , '8' , '9' ,  off,  '-', 't' , 'A' , 'n' , 'o' , 'f' , 'u'
				   //  0     1      2    3     4     5     6     7     8    9      10     11   12   13     14    15   16    17
uint16_t segs[6] = {0, 0, 0, 0, 0, 0};
uint8_t seg_cnt = 0;
uint16_t dach_anzeige = 0;
uint16_t kessel_anzeige = 0;
uint8_t dach_isFloat = 0;		//flags the value at display 1 as float (DP at the left segment will be switched on)
uint8_t kessel_isFloat = 0;		//flags the value at display 0 as float (DP at the left segment will be switched on)
uint8_t on = 0;


void SevenSeg_init()
{
	TIMSK |= (1<<TOIE0);	//enable overflow-interrupt
	
	sei();
}

//display: 1=dach; 0=kessel;   val: -99 to 999, val= 1000: off
void SevenSeg_set_val(uint8_t seg, int16_t val)
{
	uint8_t neg = 0;											//flag for negative values
	
	if(seg == 1)												
	{
		dach_isFloat = 0;
		if (val == dach_anzeige)								//if the value to set is not new, leave function
		{
			return;
		}
	}
	
	else if(seg == 0)		
	{
		kessel_isFloat = 0;
		if(val == kessel_anzeige)								//if the value to set is not new, leave function
		{
			return;
		}
	}
	
	if(val==1000)												//if this display shall be switched off
	{
		segs[seg]   = 10;
		segs[seg+2] = 10;
		segs[seg+4] = 10;
		return;
	}

	
	if(val < 0)													//if the value to set is negative
	{
		val *= -1;												//calculate absolute value
		neg = 1;
		
		if(val > 99)											//because one segment is needed for the sign, only two digits are allowed
		{
			val = 99;
		}
	}
	
	
	uint8_t h = (uint8_t) (val/100);							//separate the digits of the value
	uint8_t z = (uint8_t) ((val- (h*100)) / 10 );
	uint8_t e = (uint8_t)  (val- (h*100)- (z*10));

	
	segs[seg]   = e;											//set the digits at the propper position in the segment array
	segs[seg+2] = z;
	segs[seg+4] = h;
	
	if(neg == 1)												//if this is a negative value..
	{
		segs[seg+4] = 11;										//overwrite the left segment with sign	
	}
}


//display: 1=dach; 0=kessel;   val: 0.00 to 9.99, val= 1000: off
void SevenSeg_set_val_f(uint8_t seg, float val)
{		
		if(seg == 1)
		{			
			if ((val == dach_anzeige) && (dach_isFloat == 1))		//if the value to set is not new, leave function
			{
				return;
			}
			dach_isFloat = 1;
		}
		
		else if(seg == 0)
		{
			if ((val == kessel_anzeige) && (kessel_isFloat == 1))		//if the value to set is not new, leave function
			{
				return;
			}
			kessel_isFloat = 1;
		}
		
		if(val==1000)												//if this display shall be switched off
		{
			segs[seg]   = 10;
			segs[seg+2] = 10;
			segs[seg+4] = 10;
			return;
		}

		uint16_t val_i = (uint16_t)(val * 100);
		
		uint8_t h = (uint8_t) (val_i/100);							//separate the digits of the value
		uint8_t z = (uint8_t) ((val_i- (h*100)) / 10 );
		uint8_t e = (uint8_t)  (val_i- (h*100)- (z*10));

		
		segs[seg]   = e;											//set the digits at the propper position in the segment array
		segs[seg+2] = z;
		segs[seg+4] = h;
}

void SevenSeg_set_OpenLoad(uint8_t seg)
{
	segs[seg]   = 11;											
	segs[seg+2] = 11;
	segs[seg+4] = 11;
}

void SevenSeg_display_mode(uint8_t seg, uint8_t m)
{
	switch(m)
	{
		case MODE_AUTO:
		{
			segs[seg]   = 12;  //'t'
			segs[seg+2] = 17;  //'u'
			segs[seg+4] = 13;  //'A'
			break;
		}
		
		case MODE_ON:
		{
			segs[seg]   = 14;  //'n'
			segs[seg+2] = 15;  //'o'
			segs[seg+4] = 10;   //switch off
			break;
		}
		
		case MODE_OFF:
		{
			segs[seg]   = 16;  //'f'
			segs[seg+2] = 16;  //'f'
			segs[seg+4] = 15;  //'o'
			break;
		}
		
		default:
		break;
	}
	
	
}

ISR(TIMER0_OVF_vect)
{
	cli();
	
	if(segs[seg_cnt] != 10)					//if the current digit to display is valid (0..9)..
	{
		PORTB = patterns[segs[seg_cnt]];	//set the propper pattern to the anodes.
	}
	else if	(segs[seg_cnt] == 10)			//if the current digit is 10, the segment shall be switched off.
	{
		PORTB = 0x00;						//therefore clear the anodes
		seg_cnt ++;							//switch to next segment
		
		if(seg_cnt == 6)					//looping
		{
			seg_cnt = 0;
		}
		sei();
		return;								//leave. The common cathode of this segment shall not be activated.
	}
	
	
	//if digit for this segment is valid..
	uint8_t a = (uint8_t) (~(1<<(seg_cnt + 2)));
	PORTA = a & 0xFC;						//mask to avoid ADC-channels 0 and 1
	seg_cnt ++;								//switching to next segment
	
	if(seg_cnt == 6)						//loop segment counter
	{
		seg_cnt = 0;
	}
	
	
	//handling floats
	if(dach_isFloat == 1)
	{
		if(seg_cnt == 0)						//if the current segment is the hundreds-digit of the upper display
		{
			PORTB |= (1<<7);
		}
	}
	
	if(kessel_isFloat == 1)
	{
		if(seg_cnt == 5)						//if the current segment is the hundreds-digit of the upper display
		{
			PORTB |= (1<<7);
		}
	}
	//
	
	TIFR &= ~(1<<TOV0);						//clear timer0 overflow interrupt flag
	
	sei();
}


void SevenSeg_on()
{
	TCCR0 = (1<<CS01) | (1<<CS00);		//setting prescaler to /256
	on = 1;
}

void SevenSeg_off()
{
	TCCR0 &= ~((1<<CS01) | (1<<CS00));	//disable timer
	TCNT0 = 0;							//resetting counter
	PORTB = 0x00;
	PORTA = 0xFC;
	on = 0;
}


uint8_t SevenSeg_get_state()
{
	return on;
}