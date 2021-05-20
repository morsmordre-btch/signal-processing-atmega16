#define F_CPU 8000000UL

#define LENGTH_BUF 128
#define BUF_MASK_1 63
#define BUF_MASK_2 127


#ifndef MAIN_H_
#define MAIN_H_
	#include <avr/io.h>
	#include <avr/iom16.h>
	#include <avr/interrupt.h>
	#include <stdint.h>
	#include <util/delay.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include "uart.h"
	#include "pwm.h"
	#include "twi.h"
	#include "lcd.h"
	#include "dac.h"
	#include "modbus.h"
#endif /* MAIN_H_ */
