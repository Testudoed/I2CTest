/*
 * I2CTest.cpp
 *
 * Created: 12/15/2014 7:38:23 PM
 *  Author: Kyle
 */ 

#ifndef F_CPU
// Frequency CPU
#define F_CPU				20000000UL
#endif

extern "C" {
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/sleep.h>
}

#include "prism_interface.h"
#include "style2_pins.h"

const static uint8_t I2C_ADDRESS = 0x70;
const static uint32_t I2C_BITRATE = 100000;

void fb_init( void );

int main(void)
{
	uint8_t dataArray[10];
	// Perform GPIO Initializations
	fb_init();
	// Enable Interrupts
	sei();
	
	PrismIf.Initialize();
	
	for(int i = 0; i < 10; i++)
		dataArray[i] = i+1;
	
	PrismIf.SendData(dataArray, 10);
	
	PWR_LED_PORTR |= (1<<PWR_LED);
    while(1)
    {
		if(PrismIf.ReadRegister(PrismInterface::STATE) > 0)
		{
			MODE_LED_R_PORTR |= (1<<MODE_LED_R);
			MODE_LED_G_PORTR |= (1<<MODE_LED_G);
			MODE_LED_B_PORTR |= (1<<MODE_LED_B);
		}
		
		set_sleep_mode(SLEEP_MODE_IDLE);
		cli();
		sleep_enable();
		sei();
		// Enter sleep mode, wake on interrupt, blocks until woken
		sleep_cpu();
		sleep_disable();		
    }
}

/** Fireball initialize. */
void fb_init( void )
{
	DDRA_SETUP();
	DDRB_SETUP();
	DDRC_SETUP();
	DDRD_SETUP();
	
	// Disable JTAG
	MCUCR |=(1<<JTD);
	MCUCR |=(1<<JTD);
}