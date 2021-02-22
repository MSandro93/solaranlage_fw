/*
 * uart.h
 *
 * Created: 10.11.2020 00:19:11
 *  Author: stoff
 */ 
#include <stdio.h>

#ifndef UART_H_
#define UART_H_


void uart_init();
int uart_putchar(char c, FILE *stream);


#endif /* UART_H_ */