/*
 * I2cSlave.cpp
 *
 * Created: 12/15/2014 8:12:00 PM
 *  Author: Kyle
 */ 
extern "C"{
	#include <avr/sfr_defs.h>
	#include <avr/interrupt.h>
};

#include "I2cSlave.h"

void (*I2cSlave::user_onRequest)(void) = nullptr;
void (*I2cSlave::user_onReceive)(int) = nullptr;
volatile uint8_t I2cSlave::_dataBuffer[DATA_BUFFER_SIZE];
volatile uint8_t* I2cSlave::_registerBuffer = nullptr;
volatile uint8_t I2cSlave::_registerSize = 0;
volatile uint8_t  I2cSlave::_isBusy = 0;
volatile uint8_t I2cSlave::_currentRegAddr = 0;
volatile uint8_t I2cSlave::_dataBufferPos = 0;
volatile uint8_t I2cSlave::_numRxBytesReady = 0;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~(1<<bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= (1<<bit))
#endif

void I2cSlave::Initialize(uint8_t devAddr, uint32_t sysClk, uint32_t busFreq)
{
	 // Initialize I2C pre-scaler and bit rate
	cbi(TWSR, TWPS0);
	cbi(TWSR, TWPS1);
	TWBR = ((sysClk / busFreq) - 16) / 2;
	
	// Set I2C slave address (skip over TWGCE bit)
	TWAR = devAddr << 1;     
	
	ReadyTWCR();                  
}

void I2cSlave::SetRegisterBuffer(uint8_t* regs, uint8_t size)
{
	_registerBuffer = regs;
	_registerSize = size;
}

void I2cSlave::RxByte()
{
	if(_dataBufferPos < DATA_BUFFER_SIZE)
		_dataBuffer[_dataBufferPos++] = TWDR;
}

void I2cSlave::RxComplete()
{
	_numRxBytesReady = _dataBufferPos;
	_dataBufferPos = 0;
	if(_registerBuffer != nullptr)
	{
		_currentRegAddr = _dataBuffer[0];
		if(_numRxBytesReady > 1)
			_registerBuffer[_currentRegAddr] = _dataBuffer[1];
#if AUTO_INCR_RX
		for(int i = 2; i < _numRxBytesReady && _currentRegAddr < _registerSize; i++)
			_registerBuffer[_currentRegAddr++] = _dataBuffer[i];
#else
		_registerBuffer[_currentRegAddr] = _dataBuffer[0];
#endif
	}
	if(user_onReceive != nullptr)
		user_onReceive(_numRxBytesReady);
}

void I2cSlave::TxByte()
{
	if(_registerBuffer != nullptr && _currentRegAddr < _registerSize)
	{
#if AUTO_INCR_TX
		_dataBuffer[0] = _registerBuffer[_currentRegAddr++];	
#else
		_dataBuffer[0] = _registerBuffer[_currentRegAddr];
#endif
	}
	
	if(user_onRequest != nullptr)
		user_onRequest();
		
	TWDR = _dataBuffer[0];
}


void I2cSlave::ReadyTWCR()
{
	TWCR = (1<<TWEN)|                          // TWI Interface enabled
	(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to send byte
	(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
	(0<<TWWC);
}

void I2cSlave::EndTransmission()
{
	_isBusy = 0;
	_dataBufferPos = 0;
}

ISR(TWI_vect)
{
	switch (TWSR)
	{	
		// Slave transmitter address or data acknowledged
		case I2cSlave::I2C_DEFINITIONS::TWI_STX_ADR_ACK:  
		case I2cSlave::I2C_DEFINITIONS::TWI_STX_DATA_ACK: 
			I2cSlave::TxByte();
			I2cSlave::ReadyTWCR();
			break;
			
		// Data byte in TWDR has been transmitted; NACK has been received.
		case I2cSlave::I2C_DEFINITIONS::TWI_STX_DATA_NACK:   
			I2cSlave::EndTransmission();              
			I2cSlave::ReadyTWCR();  
			break;
		
		// Own SLA+W has been received ACK has been returned
		case I2cSlave::I2C_DEFINITIONS::TWI_SRX_ADR_ACK:         
			I2cSlave::ClearBuffer();  
			I2cSlave::ReadyTWCR();
			break;
			
		case I2cSlave::I2C_DEFINITIONS::TWI_SRX_ADR_DATA_ACK:       // Previously addressed with own SLA+W; data has been received; ACK has been returned
		case I2cSlave::I2C_DEFINITIONS::TWI_SRX_GEN_DATA_ACK:       // Previously addressed with general call; data has been received; ACK has been returned
			I2cSlave::RxByte();
			I2cSlave::ReadyTWCR();                               
			break;
			
		// A STOP condition or repeated START condition has been received while still addressed as Slave	
		case I2cSlave::I2C_DEFINITIONS::TWI_SRX_STOP_RESTART:       
			I2cSlave::RxComplete();
			I2cSlave::EndTransmission();
			I2cSlave::ReadyTWCR();	// Enter not addressed mode and listen to address match
			break;
			
		case I2cSlave::I2C_DEFINITIONS::TWI_SRX_ADR_DATA_NACK:			// Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
		case I2cSlave::I2C_DEFINITIONS::TWI_SRX_GEN_DATA_NACK:			// Previously addressed with general call; data has been received; NOT ACK has been returned
		case I2cSlave::I2C_DEFINITIONS::TWI_STX_DATA_ACK_LAST_BYTE:	// Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
		case I2cSlave::I2C_DEFINITIONS::TWI_BUS_ERROR:					// Bus error due to an illegal START or STOP condition
			TWCR =   (1<<TWSTO)|(1<<TWINT);		//Recover from TWI_BUS_ERROR, this will release the SDA and SCL pins thus enabling other devices to use the bus
		break;
		default:
			I2cSlave::EndTransmission();  
			I2cSlave::ReadyTWCR();
			break;
	}
}

I2cSlave I2cSlaveIf = I2cSlave();