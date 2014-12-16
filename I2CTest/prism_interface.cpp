/*
 * prism_interface.cpp
 *
 * Created: 5/8/2014 11:13:17 AM
 *  Author: kyle.roberts
 */ 

#include "prism_interface.h"

#ifndef F_CPU
// Frequency CPU
#define F_CPU				20000000UL
#endif

// Initialize register array
uint8_t PrismInterface::m_prismRegisters[REGISTER_LENGTH];
uint8_t PrismInterface::m_currentRegister = 0x00;
uint8_t* PrismInterface::_sendData = nullptr;

PrismInterface::PrismInterface()
{
	Reset();
}

void PrismInterface::Initialize()
{
	// Initialize I2C Slave
	I2cSlaveIf.Initialize((I2C_ADDRESS>>1), F_CPU, I2C_BITRATE);
	I2cSlaveIf.onReceive(i2c_receive);
	I2cSlaveIf.onRequest(i2c_request);
}

void PrismInterface::i2c_receive(int numBytes)
{
	m_currentRegister = I2cSlaveIf.Read();
	if(numBytes >= 2)
	{
		m_prismRegisters[m_currentRegister] = I2cSlaveIf.Read();
	}
	while(I2cSlaveIf.Available())
	{
		// clear buffer
		I2cSlaveIf.Read();
	}
}

void PrismInterface::i2c_request()
{
	//I2cSlaveIf.SendByte(m_prismRegisters[m_currentRegister]);
	
	switch(m_currentRegister)
	{
		case ACTION:
			m_currentRegister = ACTION1;
		case ACTION1:
			I2cSlaveIf.SendByte(m_prismRegisters[m_currentRegister]);
			m_prismRegisters[m_currentRegister] = 0; 
			break;
		case XAPI_TX_NUM_BYTES:
			I2cSlaveIf.SendByte(m_prismRegisters[m_currentRegister]);
			break;
		case XAPI_TX:
			//I2cSlaveIf.SendByte(_sendData, m_prismRegisters[XAPI_TX_NUM_BYTES]);
			m_prismRegisters[XAPI_TX_NUM_BYTES] = 0;
			break;
		default: I2cSlaveIf.SendByte(m_prismRegisters[m_currentRegister]); break;
	}
}

void PrismInterface::Reset()
{
	for(uint8_t i = 0; i < REGISTER_LENGTH; i++)
	{
		m_prismRegisters[i] = 0;
	}
	m_currentRegister = 0;
}

void PrismInterface::SendData(uint8_t* dataBuffer, uint8_t length)
{
	_sendData = dataBuffer;
	m_prismRegisters[XAPI_TX_NUM_BYTES] = length;
}

PrismInterface PrismIf = PrismInterface();