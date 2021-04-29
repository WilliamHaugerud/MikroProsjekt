/*
 * Mikroprosjekt.c
 *
 * Created: 22.04.2021 21:04:02
 * Author : willi
 */ 

//Includes
#include <avr/io.h>
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

int main(void)
{
	//Inits//
	init_timer(); // Initialize timer 1sec
	sei(); // enables global interrupts
	initI2C()
	//Variabler//
	int moistureData, lightData;
	
    while (1) 
    {
		if (seconds >= 3600)
		{
			i2cStart();
			i2cSend(SLAVE_ADDRESS_R);
			moistureData = i2cReadAck();
			lightData = i2cReadNoAck();
			i2cStop();
		}
    }
}
void init_timer(){
	// TCCR1A = 0; CTC normal operation
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);        // CTC, TOP = OCR1A, TOV1 flag at MAX, freq scaling:/1024
	TIMSK1 = (1<<OCIE1A);                            // Interrupt enable when OC1RA compare match
	OCR1A = 15625;                                    // 16Mhz/1024 = 152625, a.k.a 1 second
}
ISR(TIMER1_COMPA_vect){
	seconds ++;
}
