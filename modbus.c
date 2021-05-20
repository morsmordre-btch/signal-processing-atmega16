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
//������� �������� ������ �� modbus
unsigned char modbus_i = 0;
//����� ��������� ���������
unsigned char modbusRX_length = 0;

//���������� ���������� �� ������ �� UART
ISR(USART_RXC_vect){
	T2_STOP;
	TCNT2 = 0x1F;
	//�������� �� ������ �����
	if (UCSRA & (1<<FE))
		return;
	modbusRX[modbus_i] = UDR;
	//����� ���� ���������� ������
	UCSRA &= 0b01111111;
	modbus_i++;
	counterTimout = 0;
	T2_RUN;
}

//���������� ���������� �� Timer0
ISR(TIMER2_OVF_vect) {
	TCNT2 = 0x1F;
	counterTimout++;
	//���� timout �� ���� �� ������ �������
	//13 ������������ - ��� ����� ������ 3.5 �������� �� ��������� 9600 ��� (13)
	if (counterTimout == 130){
		T2_STOP;
		modbusRX_length = modbus_i;
		modbusReset();
		modbusRun();
	}
}

void modbusRun(void) {
	uint16_t crc;
	
	//���� ����� ��������� 1, �� ��� ���� ��� ���������� DAC
	if (modbusRX_length == 1){
		//����� ������� ���������� DAC
		control_DAC(modbusRX[0]);
		//����� ��������� �� �������
		convertAndSend(modbusRX[0]);
		return;
	}
	
	//�������� �� ���������� ������
	if (modbusRX[0] != ID_SLAVE){
		return;
	}
	
	//�������� CRC
	crc = modbusCRC(modbusRX, modbusRX_length - 2);
	if ( (modbusRX[modbusRX_length-2] == (crc & 0x00FF)) && (modbusRX[modbusRX_length-1] == (crc >> 8)) ) {
		//����� �������
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
	modbusTX[0] = modbusRX[0];							//����� ����������
	modbusTX[1] = modbusRX[1];							//��� �������
	//���������� ������ ������� ��������
	addressRegister = (modbusRX[2] << 8) + modbusRX[3];	//��������� �����
	quantityRegister = (modbusRX[4] << 8) + modbusRX[5];//���������� ���������
	modbusTX[2] = 2*quantityRegister;					//���������� ���� �����
	for (addressReg_i = 0; addressReg_i < quantityRegister; addressReg_i++){
		modbusTX[3+2*addressReg_i] = regBuffer[addressRegister+addressReg_i][0];		//������ �� �������� �������� �������� Hi
		modbusTX[4+2*addressReg_i] = regBuffer[addressRegister+addressReg_i][1];		//������ �� �������� �������� �������� Lo
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
	//����� ��� ������ �� 485
	PORTD = PORTD & 0b11110111;
}


void modbusReset(void){
	T2_STOP;
	modbus_i = 0;
	counterTimout = 0;
	TCNT2 = 0x1F;
	//����� ��� ������ �� 485
	PORTD = PORTD & 0b11110111;
}


unsigned int modbusCRC(unsigned char *buf, char bufsize) //������� ������������� CRC16
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

//��������� Timer2 ��� ������� timeout ��� MODBUS
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
	//���������� ���������� �� Timer2 �� ������������
	TIMSK |= (1 << TOIE2);
	TCNT2 = 0x1F;
}