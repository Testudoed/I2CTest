/*
 * prism_interface.cpp
 *
 * Created: 5/8/2014 11:13:17 AM
 *  Author: kyle.roberts
 */ 

#include "prism_interface.h"
#include "hardware.h"

// Initialize register array
uint8_t PrismInterface::m_prismRegisters[REGISTER_LENGTH];
volatile uint8_t PrismInterface::m_currentRegister = 0x00;
volatile uint8_t PrismInterface::_dataBufferPos = 0;
uint8_t* PrismInterface::_sendData = nullptr;
uint8_t PrismInterface::_sendDataLength = 0;


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
	I2cSlaveIf.ClearBuffer();
}

void PrismInterface::i2c_request()
{	
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
			BufferedSend();
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
	_dataBufferPos = 0;
	_sendDataLength = length;
	m_prismRegisters[XAPI_TX_NUM_BYTES] = _sendDataLength;
}

void PrismInterface::BufferedSend()
{
	if(_dataBufferPos < _sendDataLength)
		I2cSlaveIf.SendByte(_sendData[_dataBufferPos++]);
	else
		I2cSlaveIf.SendByte(0xFF);
}

PrismInterface PrismIf = PrismInterface();