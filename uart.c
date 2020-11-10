/*
 * uart.c
 *
 * Created: 10.11.2020 00:18:52
 *  Author: stoff
 */ 
#include <avr/io.h>


void uart_init(uint32_t baud)
{
	UCSRB |= (1<<RXEN) | (1<<TXEN);  //enable receive and transmit
	UBRRL = 51;						 //setting datarate to 9600 baud/s

	UCSRB |= (1<<RXEN) | (1<<TXEN);

	UCSRC |= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
}


void uart_send_blocking(uint8_t data)
{
	UDR = data;
	while( (UCSRA & (1<<TXC)) == 0 ) //wait for transfer finished
	{
	}
	UCSRA &= ~(1<<TXC);
}