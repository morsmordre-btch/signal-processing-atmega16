/*
 * UART.h
 *
 * Created: 11.04.2020 13:47:42
 *  Author: kosty
 */ 


#ifndef UART_H_
#define UART_H_
	#include "main.h"
	void init_UART(unsigned int baud);
	void tx_UART(unsigned char data);

	

#endif /* UART_H_ */