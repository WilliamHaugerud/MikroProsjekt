/*
 * Slave.c
 *
 * Created: 22.04.2021 21:04:02
 * Author : willi
 */ 

//Includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "i2c.h"
#include "Macros.h"


//Defines
#define SLAVE_ADDRESS			0b00000010
/* TWSR interrupt koder */

// TWI Slave Receiver status codes
#define	TWI_SRX_ADR_ACK				0x60  // Own SLA+W has been received ACK has been returned
#define	TWI_SRX_ADR_ACK_M_ARB_LOST	0x68  // Own SLA+W has been received; ACK has been returned
#define	TWI_SRX_GEN_ACK				0x70  // General call address has been received; ACK has been returned
#define	TWI_SRX_GEN_ACK_M_ARB_LOST	0x78  // General call address has been received; ACK has been returned
#define	TWI_SRX_ADR_DATA_ACK		0x80  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define	TWI_SRX_ADR_DATA_NACK		0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define	TWI_SRX_GEN_DATA_ACK		0x90  // Previously addressed with general call; data has been received; ACK has been returned
#define	TWI_SRX_GEN_DATA_NACK		0x98  // Previously addressed with general call; data has been received; NOT ACK has been returned
#define	TWI_SRX_STOP_RESTART		0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave

// TWI Slave Transmitter status codes
#define	TWI_STX_ADR_ACK				0xA8  // Own SLA+R has been received; ACK has been returned
#define	TWI_STX_ADR_ACK_M_ARB_LOST	0xB0  // Own SLA+R has been received; ACK has been returned
#define	TWI_STX_DATA_ACK			0xB8  // Data byte in TWDR has been transmitted; ACK has been received
#define	TWI_STX_DATA_NACK			0xC0  // Data byte in TWDR has been transmitted; NOT ACK has been received
#define	TWI_STX_DATA_ACK_LAST_BYTE	0xC8  // Last byte in TWDR has been transmitted (TWEA = 0); ACK has been received

// TWI Miscellaneous status codes
#define	TWI_NO_STATE				0xF8  // No relevant state information available; TWINT = 0
#define	TWI_BUS_ERROR				0x00  // Bus error due to an illegal START or STOP condition


//Funksjoner

int main(void)
{
	//Inits//
	sei(); // Enabler globale interrupts sånn at vi kan benytte oss av et interrupt basert system.
	TWAR = SLAVE_ADDRESS;
	//Setter slaven i Slave reciever mode. Enabler interrupts
	TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);
	//Variabler//
	
    while (1) 
    {
		
    }
}

	ISR( TWI_vect )
	{
		switch( TWSR )
		{
			case TWI_SRX_ADR_ACK:									// 0x60 Own SLA+W has been received ACK has been returned. Expect to receive data.
			//		case TWI_SRX_ADR_ACK_M_ARB_LOST:				// 0x68 Own SLA+W has been received; ACK has been returned. RESET interface.
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);	// Prepare for next event. Should be DATA.
			break;

			case TWI_SRX_ADR_DATA_ACK:								// 0x80 Previously addressed with own SLA+W; Data received; ACK'd
			case TWI_SRX_GEN_DATA_ACK:								// 0x90 Previously addressed with general call; Data received; ACK'd
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);	// Prepare for next event. Should be more DATA.
			break;
			
			case TWI_SRX_GEN_ACK:									// 0x70 General call address has been received; ACK has been returned
			//		case TWI_SRX_GEN_ACK_M_ARB_LOST:				// 0x78 General call address has been received; ACK has been returned
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);	// Prepare for next event. Should be DATA.
			break;

			case TWI_STX_ADR_ACK:									// 0xA8 Own SLA+R has been received; ACK has been returned. Load DATA.
			TWDR = moistureFunction();
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);
			break;
			
			
			//		case TWI_STX_ADR_ACK_M_ARB_LOST:				// 0xB0 Own SLA+R has been received; ACK has been returned
			case TWI_STX_DATA_ACK:									// 0xB8 Data byte in TWDR has been transmitted; ACK has been received. Load DATA.
			TWDR = lightFunction();
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);	// Prepare for next event.
			break;

			case TWI_STX_DATA_NACK:									// 0xC0 Data byte in TWDR has been transmitted; NOT ACK has been received. End of Sending.
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);	// Prepare for next event. Should be new message.
			break;

			case TWI_SRX_STOP_RESTART:								// 0xA0 A STOP condition or repeated START condition has been received while still addressed as Slave
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);	// Prepare for next event.
			break;

			case TWI_SRX_ADR_DATA_NACK:								// 0x88 Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
			case TWI_SRX_GEN_DATA_NACK:								// 0x98 Previously addressed with general call; data has been received; NOT ACK has been returned
			case TWI_STX_DATA_ACK_LAST_BYTE:						// 0xC8 Last byte in TWDR has been transmitted (TWEA = 0); ACK has been received
			case TWI_NO_STATE:										// 0xF8 No relevant state information available; TWINT = 0
			case TWI_BUS_ERROR:										// 0x00 Bus error due to an illegal START or STOP condition
			TWCR =   (1<<TWSTO)|(1<<TWINT);							// Recover from TWI_BUS_ERROR
			// TODO: Set an ERROR flag to tell main to restart interface.
			break;

			default:							// OOPS
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);		// Prepare for next event. Should be more DATA.
			break;
		}
	}
