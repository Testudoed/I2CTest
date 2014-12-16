/*
 * prism_interface.h
 *
 * Created: 5/8/2014 11:13:01 AM
 *  Author: kyle.roberts
 */ 


#ifndef PRISM_INTERFACE_H_
#define PRISM_INTERFACE_H_

#include "I2cSlave.h"

/// <summary>	Interface object to the PRISM device, acts as I2C slave</summary>
class PrismInterface
{
public:
/// <summary>	Enumeration of the Prism device states. </summary>
	enum PRISM_STATES
	{
		OFF = 0x00,
		INITIALIZATION = 0x01,
		IDLE = 0x02,
		MIFI_IDLE = 0x03,
		MIFI_INTERROGATE = 0x04,
		RUNNING_INTERROGATION = 0x05,
		RUNNING_DISCOVERY = 0x06,
		STARTUP = 0x07
	};
	/// <summary>	Enumeration of the indexies of Prism Slave device registers. </summary>
	enum PRISM_REGISTERS
	{
		ACTION1 = 0x00,
		STATE = 0x01,
		EVENT,
		ETHER_SWITCH_TEMP, 
		FPGA_TEMP,
		OCTASIC_TEMP,
		RFIC_TEMP,
		RF_POWER_LEVEL,
		LCP_TEMP,
		CCP_TEMP,
		ACTION = 0x20,
		XAPI_RX = 0x21,
		XAPI_TX_NUM_BYTES = 0x22,
		XAPI_TX = 0x23
	};
	/// <summary>	Enumeration of the Prism event types. </summary>
	enum PRISM_EVENTS
	{
		ALERT = 0x01,
		ZEROIZE = 0x02
	};

	/// <summary>	Default constructor. </summary>
	PrismInterface();

	/// <summary>	Initializes this object. </summary>
	void Initialize();

	/// <summary>	Reads a register. </summary>
	///
	/// <param name="reg">	The register. </param>
	///
	/// <returns>	The register. </returns>
	inline uint8_t ReadRegister(PRISM_REGISTERS reg) { return m_prismRegisters[reg]; }

	/// <summary>	Writes a register. </summary>
	///
	/// <param name="reg"> 	The register. </param>
	/// <param name="data">	The data. </param>
	inline void WriteRegister(PRISM_REGISTERS reg, uint8_t data) { m_prismRegisters[reg] = data; }
		
	/// <summary>	Start Prism Action </summary>	
	inline void StartAction() {m_prismRegisters[ACTION1] = 0x01;}
	
	/// <summary>	Start Prism Action </summary>
	inline void StopAction() {m_prismRegisters[ACTION1] = 0x02;}
		
	/// <summary>	Start Prism Action </summary>
	inline void Zeroize() {m_prismRegisters[ACTION1] = 0x03;}
		
	/// <summary>	Send data via XAPI. </summary>
	///
	/// <param name="reg"> 	The data buffer. </param>
	/// <param name="data">	The length. </param>
	static void SendData(uint8_t* dataBuffer, uint8_t length);
		
	/// <summary>	Clears all data registers. </summary>	
	void Reset();

protected:
	/// <summary>	Prism device status and event registers[]. </summary>
	static uint8_t m_prismRegisters[];
	/// <summary>	Maintains current register index </summary>
	static volatile uint8_t m_currentRegister;
	/// <summary>	Data Buffer position</summary>
	static volatile uint8_t _dataBufferPos;
	/// <summary>	Data Send</summary>
	static uint8_t* _sendData;
	/// <summary>	Data Buffer position</summary>
	static uint8_t _sendDataLength;

		
private:
	/// <summary>	Length of the register. </summary>
	const static uint8_t REGISTER_LENGTH = 0x40;
	/// <summary>	Prism Slave Device I2C address. </summary>
	const static uint8_t I2C_ADDRESS = 0x70;
	/// <summary>	I2C Bitrate </summary>
	const static uint32_t I2C_BITRATE = 100000;
	/// <summary>	I2C receive event handle. </summary>
	///
	/// <param name="numBytes">	Number of bytes. </param>
	static void i2c_receive(int numBytes);

	/// <summary>	I2C Request Event Handle. </summary>
	static void i2c_request(void);
	
	static void BufferedSend();
};

extern PrismInterface PrismIf;

#endif /* PRISM_INTERFACE_H_ */