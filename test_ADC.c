/*
 * ADC-test.c
 *
 * Created: 28.04.2021 19.23.18
 * Author : Hollingen
 */ 

#include <avr/io.h>
#include "USART.h"
#include <util/delay.h>


int main(void){
	ADMUX |= 5;
	ADMUX |= (1 << REFS0); // AVcc som referanse
	ADMUX |= (1 << ADLAR); // venstreskifter slik at 2 LSB havner i ADL og de 8 MSB havner i adh
	ADCSRA |= (1 <<ADPS2)|(1 <<ADPS1)|(1 <<ADPS0); // klokkefrekvens /128, dvs. 125kHz
	ADCSRA |= (1 << ADEN); // enables adc
	
	PORTB = (1<<PORTB0); // Setting internal pull-up
	
	initUSART();
	printString("Hello World!\r\n");
	
	//uint8_t adc_result, adc_lownibble, adc_highnibble;
	float adc_result, maxValueADC, minValueADC;
	uint8_t high_adc_result, low_adc_result, high_maxValueADC, low_maxValueADC, high_minValueADC, low_minValueADC;
	
	do {
		
		loop_until_bit_is_clear(PINB, PINB0); // Waits for PB0
		ADCSRA |= (1<<ADSC);// start adc conversion
		do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
		low_maxValueADC = ADCL;
		high_maxValueADC = ADCH;
		//maxValueADC = 230.0;
		printString("\r\nkis\r\n");
		maxValueADC = (high_maxValueADC << 2)|low_maxValueADC;
		printWord(maxValueADC);
		_delay_ms(1000);
		
		loop_until_bit_is_clear(PINB, PINB0); // Waits for PB0
		ADCSRA |= (1<<ADSC);// start adc conversion
		do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
		low_minValueADC = ADCL;
		high_minValueADC = ADCH;
		minValueADC = (high_minValueADC << 2)|low_minValueADC;
		printString("\r\nkis\r\n");
		printWord(minValueADC);
		_delay_ms(1000);
		
		loop_until_bit_is_clear(PINB, PINB0); // Waits for PB0
		ADCSRA |= (1<<ADSC);// start adc conversion
		do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
		low_adc_result = ADCL;
		high_adc_result = ADCH;
		adc_result = (high_adc_result << 2)|low_adc_result;
		printString("\r\nkis\r\n");
		printWord(adc_result);
		printString("\r\n");
		_delay_ms(1000);
		
		//printByte(adc_result);
		//transmitByte(adc_lownibble);
		// skriver verdi fra 0x00 - 0xff til terminalen
		
		//uint16_t moist = ((adc_result-minValueADC)/(maxValueADC-minValueADC)) * 100;
		
		float resistorVoltage = (adc_result / 1024.0) * 5.0; //spenningen er 5V og verdi mellom 0-1024
		float ldrVoltage = 5.0 - resistorVoltage;
		float ldrResistance = (ldrVoltage/resistorVoltage) * 10000.0; //referanse resistanse brukt er 10k
		float ldrLux = 12518931.0 * pow(ldrResistance,  -1.405); //tallene er skalarer for å få resistansen om til lux.
		
		printWord(ldrLux);
		_delay_ms(4000);
	
	}while(1);
	
}
