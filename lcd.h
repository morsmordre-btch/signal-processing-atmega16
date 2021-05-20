/*
 * lcd.h
 *
 * Created: 13.04.2020 14:22:55
 *  Author: kosty
 */ 


#ifndef LCD_H_
#define LCD_H_
	#include "main.h"
	
	#define ADDR_TWI 0x4E				//����� ����������� ������
	#define e1 I2C_SendByteByADDR(lcdPort|=0x04,ADDR_TWI) // ��������� ����� E � 1
	#define e0 I2C_SendByteByADDR(lcdPort&=~0x04,ADDR_TWI) // ��������� ����� E � 0
	#define rs1 I2C_SendByteByADDR(lcdPort|=0x01,ADDR_TWI) // ��������� ����� RS � 1
	#define rs0 I2C_SendByteByADDR(lcdPort&=~0x01,ADDR_TWI) // ��������� ����� RS � 0
	#define setLed() I2C_SendByteByADDR(lcdPort|=0x08,ADDR_TWI) // ��������� ���������
	#define setWrite() I2C_SendByteByADDR(lcdPort&=~0x02,ADDR_TWI) // ��������� ������ � ������ �������

	void init_LCD(void);
	void sendHalfByte(unsigned char msgByte);
	void sendByte(unsigned char msgByte, unsigned char mode);
	void sendCharLcd(unsigned char msgByte);
	void setPos(unsigned char x, unsigned char y);
	void clearLcd(void);
	void sendMsgLcd(char str[]);
	void convertAndSend(unsigned char rx_UART);

#endif /* LCD_H_ */