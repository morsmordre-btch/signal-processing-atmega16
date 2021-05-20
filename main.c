#include "main.h"


#define BAUD 9600
#define VALUE_UBRR F_CPU/16/BAUD-1



int main(void)
{

	DDRA = 0xFF;
	//разрешение прерываний
	sei();
	//настройка и запуск ШИМ
	init_PWM();
	//настройка UART
	init_UART(VALUE_UBRR);
	//настройка I2C
	init_I2C();
	//настройка LCD
	init_LCD();
	clearLcd();
	//настройка timer2 для modbus
	init_T2();
    while (1) 
    {
    }
}


