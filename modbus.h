#ifndef MODBUS_H_
#define MODBUS_H_
	#include "main.h"
	
	#define T2_RUN  TCCR2 |= 0b00000001
	#define T2_STOP TCCR2 = 0b11111000
	
	void modbusReset(void);
	void modbusRun(void);
	void readInputRegisters(void);
	void modbusTransmit(unsigned char modbusTX_length);
	unsigned int modbusCRC(unsigned char *buf, char bufsize);
	void init_T2(void);

	
#endif /* MODBUS_H_ */
