/*
 * modbus.c
 *
 * Created: 22.04.2020 23:18:49
 *  Author: kosty
 */ 
#include "modbus.h"

#define ID_SLAVE 0x01

extern unsigned char regBuffer[LENGTH_BUF][2];

unsigned char counterTimout = 0;
static unsigned char modbusRX[8] = {0};
unsigned char modbusTX[255];
//счетчик принятых байтов по modbus
unsigned char modbus_i = 0;
//длина принятого сообщения
unsigned char modbusRX_length = 0;

//обработчик прерывания по приему по UART
ISR(USART_RXC_vect){
	T2_STOP;
	TCNT2 = 0x1F;
	//проверка на ошибку кадра
	if (UCSRA & (1<<FE))
		return;
	modbusRX[modbus_i] = UDR;
	//сброс бита завершения приема
	UCSRA &= 0b01111111;
	modbus_i++;
	counterTimout = 0;
	T2_RUN;
}

//обработчик прерывания по Timer0
ISR(TIMER2_OVF_vect) {
	TCNT2 = 0x1F;
	counterTimout++;
	//если timout то идем на разбор посылки
	//13 переполнений - это время равное 3.5 символам со скоростью 9600 бод (13)
	if (counterTimout == 130){
		T2_STOP;
		modbusRX_length = modbus_i;
		modbusReset();
		modbusRun();
	}
}

void modbusRun(void) {
	uint16_t crc;
	
	//если длина сообщения 1, то это байт для управления DAC
	if (modbusRX_length == 1){
		//вызов функции управления DAC
		control_DAC(modbusRX[0]);
		//вывод принятого на дисплей
		convertAndSend(modbusRX[0]);
		return;
	}
	
	//проверка на совпадение адреса
	if (modbusRX[0] != ID_SLAVE){
		return;
	}
	
	//проверка CRC
	crc = modbusCRC(modbusRX, modbusRX_length - 2);
	if ( (modbusRX[modbusRX_length-2] == (crc & 0x00FF)) && (modbusRX[modbusRX_length-1] == (crc >> 8)) ) {
		//выбор функции
		switch (modbusRX[1]) {
			case 0x04:
				readInputRegisters();
			break;
		}	
	}
}


void readInputRegisters(void) {
	uint16_t addressRegister = 0;
	uint16_t quantityRegister = 0;
	unsigned char addressReg_i = 0;
	uint16_t crc = 0;
	unsigned char modbusTX_length = 0;
	modbusTX[0] = modbusRX[0];							//адрес устройства
	modbusTX[1] = modbusRX[1];							//код функции
	//вычисление адреса первого регистра
	addressRegister = (modbusRX[2] << 8) + modbusRX[3];	//начальный адрес
	quantityRegister = (modbusRX[4] << 8) + modbusRX[5];//количество регистров
	modbusTX[2] = 2*quantityRegister;					//количество байт далее
	for (addressReg_i = 0; addressReg_i < quantityRegister; addressReg_i++){
		modbusTX[3+2*addressReg_i] = regBuffer[addressRegister+addressReg_i][0];		//запись на отправку значения регистра Hi
		modbusTX[4+2*addressReg_i] = regBuffer[addressRegister+addressReg_i][1];		//запись на отпрваку значения регистра Lo
	}
	modbusTX_length = 3 + 2*(addressReg_i) + 2;
	crc = modbusCRC(modbusTX, modbusTX_length - 2);
	modbusTX[modbusTX_length - 1] = crc >> 8;
	modbusTX[modbusTX_length - 2] = crc & 0x00FF;
	modbusTransmit(modbusTX_length);
}

void modbusTransmit(unsigned char modbusTX_length) {
	unsigned char modbusTX_i = 0;
	PORTD = PORTD | 0b00001000;
	for (modbusTX_i = 0; modbusTX_i < modbusTX_length; modbusTX_i++) {
		tx_UART(modbusTX[modbusTX_i]);
	}
	//ножка для приема по 485
	PORTD = PORTD & 0b11110111;
}


void modbusReset(void){
	T2_STOP;
	modbus_i = 0;
	counterTimout = 0;
	TCNT2 = 0x1F;
	//ножка для приема по 485
	PORTD = PORTD & 0b11110111;
}


unsigned int modbusCRC(unsigned char *buf, char bufsize) //Функция расчитывающая CRC16
{
	unsigned int crc = 0xFFFF;
	unsigned char pos = 0, CRC_i;
	for (pos = 0; pos < bufsize; pos++)
	{
		crc ^= (unsigned int)buf[pos];
		
		for (CRC_i = 8; CRC_i != 0; CRC_i--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			crc >>= 1;
		}
	}
	return crc;
}

//настройка Timer2 для отсчета timeout для MODBUS
void init_T2(void) {
	//		  FOC0
	//		  |WGM00
	//		  ||COM01
	//		  |||COM00
	//		  ||||WGM01
	//		  |||||CS02
	//		  ||||||CS01
	//		  |||||||CS00
	//		  ||||||||
	//		  76543210
	//разрешение прерывания от Timer2 по переполнению
	TIMSK |= (1 << TOIE2);
	TCNT2 = 0x1F;
}