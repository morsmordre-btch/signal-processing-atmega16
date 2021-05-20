/*
 * project_module.c
 *
 * Created: 11.04.2020 13:18:25
 * Author : kosty
 */ 

#include "main.h"


#define BAUD 9600
#define VALUE_UBRR F_CPU/16/BAUD-1



int main(void)
{

	DDRA = 0xFF;
	//���������� ����������
	sei();
	//��������� � ������ ���
	init_PWM();
	//��������� UART
	init_UART(VALUE_UBRR);
	//��������� I2C
	init_I2C();
	//��������� LCD
	init_LCD();
	clearLcd();
	//��������� timer2 ��� modbus
	init_T2();
    while (1) 
    {
    }
}


