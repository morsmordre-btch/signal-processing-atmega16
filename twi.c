/*
 * twi.c
 *
 * Created: 13.04.2020 9:37:30
 *  Author: kosty
 */ 

#include "twi.h"

void init_I2C(void) {
	TWBR = 0x20;			//скорость передачи 100к√ц
}

void I2C_StartCondition(void) {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while( !(TWCR & (1<<TWINT)) );
}

void I2C_StopCondition(void) {
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void I2C_SendByte(unsigned char msgByte) {
	TWDR = msgByte;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while( !(TWCR & (1<<TWINT)) );
}

void I2C_SendByteByADDR(unsigned char msgByte, unsigned char addr) {
	I2C_StartCondition();
	I2C_SendByte(addr);
	I2C_SendByte(msgByte);
	I2C_StopCondition();
}