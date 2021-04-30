/*
 * CFile1.c
 *
 * Created: 26.04.2021 16:02:57
 *  Author: willi
 */ 
//Includes
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/power.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "i2c.h"
#include "Macros.h"


//Defines
#define SLAVE_ADDRESS			0b00000010
#define moistureSensor			0
#define button1					PD2
#define button2					PD3

float adc_result, maxValueADC, minValueADC;
uint8_t high_adc_result, low_adc_result, high_maxValueADC, low_maxValueADC, high_minValueADC, low_minValueADC;
//Funksjoner

void init_ADC(){
	
	ADMUX |= (1 << REFS0); // AVcc as referance
	ADMUX |= (1 << ADLAR); // venstreskifter slik at 2 LSB havner i ADL og de 8 MSB havner i adh
	ADCSRA |= (1 <<ADPS2)|(1 <<ADPS1)|(1 <<ADPS0); // clk /128 -> 125kHz
	ADCSRA |= (1 << ADEN); // enables adc
}
void init_Buttons(){
	
	PORTD = 0b11111111; //Aktiverer pullup resistor på PD2 og PD3. Aktiverer også pullup for å unngå flytende inputs
	DDRD =  0b00000000; //Setter alle til inputs
	PORTC = 0b00000000; //
	DDRC = 0b00000000; //Setter alle til inputs
}
void init_Interrupt(){
	
	EIMSK = (1<<INT0); //Interrupt på INT0
	EICRA |= (1<< ISC01) | (1<<ISC10); //Trigger på negativ flanke
}
void init_I2C_Slave(){
	
	TWAR = SLAVE_ADDRESS;
	//Setter slaven i Slave reciever mode.
	TWCR = (1<<TWEN)|(0<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);
}
int start_ADC(int pin){
	// finne pinnene som adc inputen skal være på
	clear_bit(ADMUX, 0);
	ADMUX |= pin; // Which inputs to use MUX[3:0]
	ADCSRA |= (1<<ADSC);// start adc conversion
	do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
	low_adc_result = ADCL;
	high_adc_result = ADCH;
	adc_result = (high_adc_result << 2)|(low_adc_result >> 6);
	
	//----Skalering---//
	if(pin == 0){
		uint16_t moist = ((adc_result-minValueADC)/(maxValueADC-minValueADC)) * 100;
		return moist;
		}
	 else if(pin == 1){
		return high_adc_result;
		}
}
void adc_calibration(){
	cli();
	
	printString("\r\nPress button 2 for dry calibration\r\n");
	loop_until_bit_is_clear(PIND, button2); // Waits for PB1
	ADCSRA |= (1<<ADSC);// start adc conversion
	do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
	low_minValueADC = ADCL;
	high_minValueADC = ADCH;
	minValueADC = (high_minValueADC << 2)|(low_minValueADC >> 6);
	_delay_ms(2000);
	printString("\r\nPress button 2 for wet calibration\r\n");
	loop_until_bit_is_clear(PIND, button2); // Waits for PB1
	ADCSRA |= (1<<ADSC);// start adc conversion
	do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
	low_maxValueADC = ADCL;
	high_maxValueADC = ADCH;
	maxValueADC = (high_maxValueADC << 2)|(low_maxValueADC >> 6);
	_delay_ms(2000);
	
	sei();
}
ISR(INT0_vect){
	printString("\r\nCalibration started!\r\n");
	adc_calibration();
}

int main(void)
{
	//Inits//
	//Knapper
	init_Buttons();
	//INT0 interrupt
	init_Interrupt();
	//ADC
	//DIDR0 = (1<<ADC0D) | (1<<ADC1D); //Må disable digital input på inngangene vi bruker adc på
	init_ADC(); // Initialize ADC
	//USART
	initUSART();
	//I2C Slave 
	init_I2C_Slave();
	//Variabler//
	sei(); // Enabler globale interrupts sånn at vi kan benytte oss av et interrupt basert system.
	
	while (1)
	{
// 		printByte(TWSR); //Testing purposes
// 		_delay_ms(1000);
		if (TWSR == 0xA8) // Own SLA+R has been received; ACK has been returned
		{
			TWDR = start_ADC(0); // Input bitmaskene for fuktighet
			printByte(TWDR);
			TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
			loop_until_bit_is_set(TWCR, TWINT);
			TWDR = start_ADC(1); //Input bitmaskene for lys
			printByte(TWDR);
			TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
			printString("\r\nData sent\r\n");
		}
		else if (TWSR == 0xC0) //No ack recieved. Master har avsluttet sending og slave blir resatt.
		{
			init_I2C_Slave();
		}
		else if (TWSR == 0x00) //Ulovlig modus. Skjer om sending foregår mens kalibrering skjer
		{
			init_I2C_Slave();
		}
		
	}
}

