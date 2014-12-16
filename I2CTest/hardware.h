/*
 * hardware.h
 *
 * Created: 5/5/2014 10:25:27 AM
 *  Author: kyle.roberts
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_

#if __AVR_ATmega324A__ || __AVR_ATmega324PA__

#ifndef F_CPU
// Frequency CPU
#define F_CPU				20000000UL
#endif

// Clock Divisors
#define CLOCK_DIV_1			(1.00 / (F_CPU))
#define CLOCK_DIV_8			(8.00 / (F_CPU))
#define CLOCK_DIV_64		(64.00 / (F_CPU))
#define CLOCK_DIV_256		(256.00 / (F_CPU))
#define CLOCK_DIV_1024		(1024.00 / (F_CPU))

#define ONE_SEC_COUNTER		(1.00 / (CLOCK_DIV_1024))

#endif



#endif /* HARDWARE_H_ */