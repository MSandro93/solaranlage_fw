/*
 * uart.c
 *
 * Created: 10.11.2020 00:18:52
 *  Author: stoff
 */ 
#include <avr/io.h>
#include <stdio.h>


void uart_init()
{
	UCSRB |= (1<<RXEN) | (1<<TXEN);  //enable receive and transmit
	UBRRL = 51;						 //setting datarate to 9600 baud/s

	UCSRB |= (1<<RXEN) | (1<<TXEN);

	UCSRC |= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
}


int uart_putchar(char c, FILE *stream)
{
	if(c=='\n')
		uart_putchar('\r', stream);
		
		
	while( (UCSRA & (1<<UDRE)) ==0 )
	{
	}
		
	UDR = c;
	
	return 0;
}
