#include "lcd.h"

unsigned char lcdPort = 0;	//для хранения данных порта микросхемы расишерния

unsigned char asciiNum[10] =  { 0x30,	//0
								0x31,	//1
								0x32,	//2
								0x33,	//3
								0x34,	//4
								0x35,	//5
								0x36,	//6
								0x37,	//7
								0x38,	//8
								0x39,};	//9


void init_LCD() { 
	_delay_ms(15);
	sendHalfByte(0b00000011);
	_delay_ms(4);
	sendHalfByte(0b00000011);
	_delay_us(100);
	sendHalfByte(0b00000011);
	_delay_ms(1);
	sendHalfByte(0b00000010);
	_delay_ms(1);
	sendByte(0b00101000, 0);
	_delay_ms(1);
	sendByte(0b00001100, 0);
	_delay_ms(1);
	sendByte(0b00000110, 0);
	_delay_ms(1);
	setLed();			//подсветка
	setWrite();			//установка записи в память дисплея
}

void clearLcd(void) {
	sendByte(0b00000001, 0);
	_delay_us(1500);
}

void sendHalfByte(unsigned char msgByte) {
	msgByte <<= 4;
	e1;
	_delay_us(50);
	lcdPort &= 0b00001111;
	lcdPort |= msgByte;
	I2C_SendByteByADDR(lcdPort, ADDR_TWI);
	e0;
	_delay_us(50);
}

void sendByte(unsigned char msgByte, unsigned char mode) {
	if (mode == 0)
		rs0;
	else 
		rs1;
	unsigned char msgHighByte = 0;
	msgHighByte = msgByte>>4;
	sendHalfByte(msgHighByte); sendHalfByte(msgByte);
}

void sendCharLcd(unsigned char msgByte) {
	sendByte(msgByte, 1);
}

void setPos(unsigned char x, unsigned char y) {
	char address;
	address = (0x40*y+x)|0b10000000;
	sendByte(address,0);
}

void sendMsgLcd(char str[]) {
	wchar_t n;
	for (n = 0; str[n]!='\0'; n++)
		sendCharLcd(str[n]);
}


void convertAndSend(unsigned char rx_UART) {
	uint8_t num1;
	uint8_t num2;
	uint8_t num3;
	num3 = rx_UART % 10;
	num2 = (rx_UART - num3) % 100 / 10;
	num1 = (rx_UART - (rx_UART % 100))/100;
	clearLcd();
	setPos(0,0);
	sendCharLcd(asciiNum[num1]);
	sendCharLcd(asciiNum[num2]);
	sendCharLcd(asciiNum[num3]);
}

