/*
 * I2cSlave.h
 *
 * Created: 12/15/2014 7:57:01 PM
 *  Author: Kyle
 */ 


#ifndef I2CSLAVE_H_
#define I2CSLAVE_H_

// Define AUTO_INCR_RX to automatically increment current register address for 
// successive reads
#ifndef AUTO_INCR_RX
#define AUTO_INCR_RX 0
#endif

// Define AUTO_INCR_TX to automatically increment current register address for
// successive writes
#ifndef AUTO_INCR_TX
#define AUTO_INCR_TX 0
#endif


extern "C"{
	#include <avr/io.h>	
	void TWI_vect(void)  __attribute__ ((signal));
};

class I2cSlave 
{
public:
	I2cSlave() {}
	void Initialize(uint8_t devAddr, uint32_t sysClk, uint32_t busFreq);
	inline uint8_t IsBusy() {return _isBusy;}
	void onReceive( void (*function)(int) ) { user_onReceive = function; }
	void onRequest( void (*function)(void) ) { user_onRequest = function; }
	void SetRegisterBuffer(uint8_t* regs, uint8_t size);
	
	inline uint8_t GetCurrentRegisterAddr() { return _currentRegAddr; }
	inline void SendByte(uint8_t dataByte) { _registerBuffer[0] = dataByte; }
		
	inline uint8_t Available() { return _numBytesRx;}
	inline uint8_t Read() { _numBytesRx--; return _dataBuffer[_dataBufferPos++];}
	inline static void ClearBuffer() { _numBytesRx = 0; }
	
	friend void TWI_vect();
protected:
	const static uint32_t DATA_BUFFER_SIZE = 32;
	static volatile uint8_t* _registerBuffer;
	static volatile uint8_t _registerSize;
	static volatile uint8_t _isBusy;
	static volatile uint8_t _currentRegAddr;
	static volatile uint8_t _dataBuffer[DATA_BUFFER_SIZE];
	static volatile uint8_t _dataBufferPos;
	static volatile uint8_t _numBytesRx;
	static void (*user_onRequest)(void);
	static void (*user_onReceive)(int);

	static void RxByte();
	static void TxByte();
	static void RxComplete();
	static void EndTransmission();
	static void ReadyTWCR();
	enum I2C_DEFINITIONS
	{
		/****************************************************************************
		  TWI State codes
		****************************************************************************/
		// General TWI Master status codes  
		// START has been transmitted                    
		TWI_START =						0x08,  
		// Repeated START has been transmitted
		TWI_REP_START =					0x10,  
		// Arbitration lost
		TWI_ARB_LOST  =					0x38,  

		// TWI Master Transmitter status codes  
		// SLA+W has been transmitted and ACK received                    
		TWI_MTX_ADR_ACK =				0x18,  
		// SLA+W has been transmitted and NACK received
		TWI_MTX_ADR_NACK =				0x20,  
		// Data byte has been transmitted and ACK received
		TWI_MTX_DATA_ACK =				0x28,  
		// Data byte has been transmitted and NACK received 
		TWI_MTX_DATA_NACK =				0x30,  

		// TWI Master Receiver status codes 
		// SLA+R has been transmitted and ACK received
		TWI_MRX_ADR_ACK =				0x40, 
		// SLA+R has been transmitted and NACK received 
		TWI_MRX_ADR_NACK =				0x48,  
		// Data byte has been received and ACK transmitted
		TWI_MRX_DATA_ACK =				0x50,  
		// Data byte has been received and NACK transmitted
		TWI_MRX_DATA_NACK =				0x58,  

		// TWI Slave Transmitter status codes
		// Own SLA+R has been received; ACK has been returned
		TWI_STX_ADR_ACK =				0xA8,  
		// Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
		TWI_STX_ADR_ACK_M_ARB_LOST =	0xB0,  
		// Data byte in TWDR has been transmitted; ACK has been received
		TWI_STX_DATA_ACK =				0xB8, 
		// Data byte in TWDR has been transmitted; NOT ACK has been received 
		TWI_STX_DATA_NACK =				0xC0,  
		// Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
		TWI_STX_DATA_ACK_LAST_BYTE =	0xC8,  

		// TWI Slave Receiver staus codes
		// Own SLA+W has been received ACK has been returned
		TWI_SRX_ADR_ACK =				0x60,  
		// Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
		TWI_SRX_ADR_ACK_M_ARB_LOST =	0x68,  
		// General call address has been received; ACK has been returned
		TWI_SRX_GEN_ACK =				0x70,  
		// Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
		TWI_SRX_GEN_ACK_M_ARB_LOST =	0x78,  
		// Previously addressed with own SLA+W; data has been received; ACK has been returned
		TWI_SRX_ADR_DATA_ACK =			0x80,  
		// Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
		TWI_SRX_ADR_DATA_NACK =			0x88,  
		// Previously addressed with general call; data has been received; ACK has been returned
		TWI_SRX_GEN_DATA_ACK =			0x90,  
		// Previously addressed with general call; data has been received; NOT ACK has been returned
		TWI_SRX_GEN_DATA_NACK =			0x98,  
		// A STOP condition or repeated START condition has been received while still addressed as Slave
		TWI_SRX_STOP_RESTART =			0xA0,  

		// TWI Miscellaneous status codes
		TWI_NO_STATE =					0xF8,  // No relevant state information available; TWINT = “0”
		TWI_BUS_ERROR =					0x00  // Bus error due to an illegal START or STOP condition
	};
};

extern I2cSlave I2cSlaveIf;


#endif /* I2CSLAVE_H_ */