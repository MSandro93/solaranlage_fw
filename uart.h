/*
 * uart.h
 *
 * Created: 10.11.2020 00:19:11
 *  Author: stoff
 */ 


#ifndef UART_H_
#define UART_H_


void uart_init(uint32_t baud);
void uart_send_blocking(uint8_t data);


#endif /* UART_H_ */