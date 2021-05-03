/*
 * Prosjekt_Master_C.c
 *
 * Created: 29.04.2021 13:54:08
 * Author : eirik
 */ 

//Includes

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

#include <util/delay.h>
#include <avr/power.h>
#include <avr/sfr_defs.h>
#include "USART.h"
#include "i2c.h"


//Defines
#define SLAVE_ADDRESS_W			0b00000010
#define SLAVE_ADDRESS_R			0b00000011

volatile uint16_t seconds;

//Funksjoner

void init_timer(){
	// TCCR1A = 0; CTC normal operation
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);        // CTC, TOP = OCR1A, TOV1 flag at MAX, freq scaling:/1024
	TIMSK1 = (1<<OCIE1A);                            // Interrupt enable when OC1RA compare match
	OCR1A = 15625;                                    // 16Mhz/1024 = 152625, a.k.a 1 second
}

void init_Buttons(){
	
	PORTD = 0b11111111; //Aktiverer pullup resistor på PD2 og PD3. Aktiverer også pullup for å unngå flytende inputs
	DDRD =  0b00000000; //Setter alle til inputs
	PORTC = 0b00000000; //
	DDRC = 0b00000000; //Setter alle til inputs
}

int main(void)
{
	PORTB |= (1 << PORTB0);
	//Inits//
	init_timer(); // Initialize timer 1sec
	sei(); // enables global interrupts
	initI2C();
	init_Buttons();
	//Variabler//
	initUSART();
	uint8_t moistureData, lowLightData, highLightData;
	
	while (1){
		
		if (seconds >= 3600){
			seconds = 0;
			
			i2cStart();
			i2cSend(SLAVE_ADDRESS_R);
			moistureData = i2cReadAck();
			highLightData = i2cReadAck();
			lowLightData = i2cReadNoAck();
			i2cStop();
			
			
			printString("\r\nMeasurement done! New values are ready!\r\n");
			printString("\r\nPress the button to display moisture and lumen\r\n");
			
		}
		if(!(PIND & PIND2)){
			_delay_ms(100);
			if(!(PIND & PIND2)){
				printString("\r\nMoisture: ");
				printByte(moistureData);
				printString("%");
				printString("\r\nLux: ");
				printWord(combineWord(lowLightData, highLightData));
			}
			
		}
	}
}

void combineWord(uint8_t lowByte, uint8_t highByte){
	uint16_t lightData = (highByte << 8)|lowByte;
	return lightData;
}

ISR(TIMER1_COMPA_vect){
	seconds ++;
}
