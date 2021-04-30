/*
 * ProsjektMikro.c
 *
 * Created: 22.04.2021 12.30.07
 * Author : Hollingen
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint16_t seconds;
float adc_result, maxValueADC, minValueADC;
uint8_t high_adc_result, low_adc_result, high_maxValueADC, low_maxValueADC, high_minValueADC, low_minValueADC;
volatile start_calibration;

void init_timer(){
													// TCCR1A = 0; CTC normal operation
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);		// CTC, TOP = OCR1A, TOV1 flag at MAX, freq scaling:/1024
	TIMSK1 = (1<<OCIE1A);							// Interrupt enable when OC1RA compare match
	OCR1A = 15625;									// 16Mhz/1024 = 152625, a.k.a 1 second
}

void init_ADC(){
	
	ADMUX |= (1 << REFS0); // AVcc as referance
	ADMUX |= (1 << ADLAR); // venstreskifter slik at 2 LSB havner i ADL og de 8 MSB havner i adh
	ADCSRA |= (1 <<ADPS2)|(1 <<ADPS1)|(1 <<ADPS0); // clk /128 -> 125kHz
	ADCSRA |= (1 << ADEN); // enables adc
}

int start_ADC(int pin){
	
	ADMUX |= pin; // Which inputs to use MUX[3:0]
	ADCSRA |= (1<<ADSC);// start adc conversion
	do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
	low_adc_result = ADCL;
	high_adc_result = ADCH;
	adc_result = (high_adc_result << 2)|low_adc_result;
	
	return adc_result;
	// SEND SHITN//
	
	
	//----Skalering---//
	if(pin == 0){
		uint16_t moist = ((adc_result-minValueADC)/(maxValueADC-minValueADC)) * 100;
	} else if(pin == 1){
		float resistorVoltage = (adc_result / 1024.0) * 5.0; //spenningen er 5V og verdi mellom 0-1024
		float ldrVoltage = 5.0 - resistorVoltage;
		float ldrResistance = (ldrVoltage/resistorVoltage) * 10000.0; //referanse resistanse brukt er 10k
		float ldrLux = 12518931.0 * pow(ldrResistance,  -1.405); //tallene er skalarer for å få resistansen om til lux.
		
		return ldrLux;
	}
	
	
	//Skrive til USART eller LED
	
}

void adc_calibration(){
	
	// USART "Put in dry, press when ready" elns
	loop_until_bit_is_clear(PINB, PINB0); // Waits for PB0
	ADCSRA |= (1<<ADSC);// start adc conversion
	do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
	low_minValueADC = ADCL;
	high_minValueADC = ADCH;
	minValueADC = (high_minValueADC << 2)|low_minValueADC;
	
	// USART "Put in water, press when ready" elns
	loop_until_bit_is_clear(PINB, PINB0); // Waits for PB0
	ADCSRA |= (1<<ADSC);// start adc conversion
	do {} while (ADCSRA & (1<<ADSC)); // venter på at omgjøringen skal bli ferdig
	low_maxValueADC = ADCL;
	high_maxValueADC = ADCH;
	maxValueADC = (high_maxValueADC << 2)|low_maxValueADC;
	
}
int main(void)
{
	PORTB = (1<<PORTB0); // Setting internal pull-up
	
	init_timer(); // Initialize timer 1sec
	init_ADC(); // Initialize ADC
	sei(); // enables global interrupts
	
    do {
		if(start_calibration){
			adc_calibration();
		}
		if(seconds >= 3600){
			//do shit
			seconds = 0;
			//start_ADC();
		}
    } while (1);
}


ISR(TIMER1_COMPA_vect){
	seconds ++;
}

