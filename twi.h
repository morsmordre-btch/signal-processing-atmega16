/*
 * twi.h
 *
 * Created: 13.04.2020 9:37:21
 *  Author: kosty
 */ 


#ifndef TWI_H_
#define TWI_H_
	#include "main.h"
	void init_I2C(void);
	void I2C_StartCondition(void);
	void I2C_StopCondition(void);
	void I2C_SendByte(unsigned char msgByte);
	void I2C_SendByteByADDR(unsigned char msgByte, unsigned char addr);

#endif /* TWI_H_ */