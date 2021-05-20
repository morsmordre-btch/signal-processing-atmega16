/*
 * dac.c
 *
 * Created: 15.04.2020 1:08:23
 *  Author: kosty
 */ 

#include "dac.h"


uint32_t rx_DAC;
uint32_t digit_DAC;

void init_PORTA(void) {
	DDRA = 0xFF;
}

void init_PORTB(void) {
	DDRB = 0xFF;
}

void init_DAC(void) {
	init_PORTA();
	init_PORTB();
}

void control_DAC(unsigned char rx_UART) {
	rx_DAC = rx_UART;
	if (rx_DAC > 240){
		rx_DAC = 240;
	}
	digit_DAC = 4095*rx_DAC/240;
	PORTA = digit_DAC & 0xFF;
	PORTB = digit_DAC >> 8;
}