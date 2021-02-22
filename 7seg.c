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


uint8_t patterns[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00};
uint16_t segs[6] = {0, 0, 0, 0, 0, 0};
uint8_t seg_cnt = 0;
uint16_t dach_anzeige = 0;
uint16_t kessel_anzeige = 0;
uint8_t on = 0;


void SevenSeg_init()
{
	TIMSK |= (1<<TOIE0);	//enable overflow-interrupt
	
	sei();
}

//seg: 1=dach; 0=kessel;   val: 0-9: 0-9; 10: off
void SevenSeg_set_val(uint8_t seg, uint16_t val)
{
	if(seg==1)
	{
		if (val!=dach_anzeige)
		{
			dach_anzeige = val;
			printf("segment 'dach' set to %d\n", val);
		}
	}
	else if(seg==0)
	{
		if(val!=kessel_anzeige)
		{
			kessel_anzeige = val;
			printf("segment 'kessel' set to %d\n", val);
		}
	}
	
	uint8_t h = (uint8_t) (val/100);
	uint8_t z = (uint8_t) ((val- (h*100)) / 10 );
	uint8_t e = (uint8_t)  (val- (h*100)- (z*10));
	
	if(val==1000)
	{
		segs[seg]   = 10;
		segs[seg+2] = 10;
		segs[seg+4] = 10;
		return;
	}
	
	segs[seg]   = e;
	segs[seg+2] = z;
	segs[seg+4] = h;
}



ISR(TIMER0_OVF_vect)
{
	cli();
	
	if(segs[seg_cnt] < 10)					//if the current digit to display is valid (0..9)..
	{
		PORTB = patterns[segs[seg_cnt]];	//set the propper pattern to the anodes.
	}
	else									//if the current digit to display is NOT valid, it is meant to be ignored..
	{
		PORTB = 0x00;						//therefore clear the anodes
		seg_cnt ++;							//switch to nex segment
		
		if(seg_cnt == 6)	//looping
		{
			seg_cnt = 0;
		}
		sei();
		return;								//leave. The common cathode of this segment shall not be activated.
	}
	
	//if digit for this segment is valid..
	uint8_t a = (uint8_t) (~(1<<(seg_cnt + 2)));
	PORTA = a & 0xFC;	//mask to avoid ADC-channels 0 and 1
	seg_cnt ++;			//switching to next segment
	
	if(seg_cnt == 6)	//looping
	{
		seg_cnt = 0;
	}
	
	TIFR &= ~(1<<TOV0);	//clear timer0 overflow interrupt flag
	
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