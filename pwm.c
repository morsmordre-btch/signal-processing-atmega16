#include "pwm.h"

uint32_t counterISR_INT0 = 0;
uint8_t counterISR_T0 = 0;
uint32_t comparePWM = 0;



unsigned char regBuffer[LENGTH_BUF][2];
unsigned char buff_i = 0;


//обработчик прерывани€ по INT0
ISR(INT0_vect){
	counterISR_INT0++;
}

//обработчик прерывани€ по Timer0
ISR(TIMER0_OVF_vect) {
	TCNT0 = 0x05;
	counterISR_T0++;
	if (counterISR_T0 == 125) {
		counterISR_T0 = 0;
		comparePWM = 533*counterISR_INT0/16000;
		OCR1A = comparePWM;
		regBuffer[buff_i][0] = counterISR_INT0 >> 8;		//запись ст. байта
		regBuffer[buff_i][1] = counterISR_INT0 & 0xFF;		//запись мл. байта
		buff_i++;
		buff_i &= BUF_MASK_1;
		counterISR_INT0 = 0;
	}
}

void init_ISR_INT0(void) {
	//по переднему  фронту
	MCUCR |= (1 << ISC01)|(1 << ISC00);
	//разрешение прерываний по INT0 
	GICR |= (1 << INT0);
}

void init_T0(void) {
	//разрешение прерывани€ от Timer0 по переполнению
	TIMSK |= (1 << TOIE0);
	TCNT0 = 0xFA;
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
	TCCR0 |= 0b00000100;
}

void init_PWM(void) {
	//		  COM1A1       
	//		  |COM1A0    
	//		  ||COM1B1     
	//		  |||COM1B0    
	//		  ||||FOC1A     
	//		  |||||FOC1B   
	//		  ||||||WGM11  
	//		  |||||||WGM10  
	//		  ||||||||
	//		  76543210
	TCCR1A = 0b10000010;
	//		  ICNC1       
	//		  |ICES1    
	//		  ||-     
	//		  |||WGM13  
	//		  ||||WGM12     
	//		  |||||CS12   
	//		  ||||||CS11 
	//		  |||||||CS10  
	//		  ||||||||
	//		  76543210
	TCCR1B |= 0b00011001;
	//считать будет до 533 дл€ 15к√ц
	ICR1 = 533;
	//cравниваема€ величина
	OCR1A = 266;
	//Ќастройка ножки OC1A как выход:
	DDRD |= (1 << PD5);
	
	init_ISR_INT0();
	init_T0();
}
