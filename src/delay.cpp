/*
 * delay.c
 *
 *  Created on: Oct 3, 2016
 *      Author: timppa
 */

#include "delay.h"

//
// this causes delay of approx 1s (when clocking is the default)
// - in general it is not very smart to use delays
// - and it is not very smart to create busy counting delays like this
//

void delay(volatile int d)
{
	while( --d );
}

#define DELAY_1_0 1120000
//#define DELAY_1_0    2724631

void delay_1s()
{
	// MUST use volatile here because otherwise the compiler
	// might optimize the whole loop away!

	volatile int d = DELAY_1_0;
//	int d = DELAY_1_0;
	delay( d );
}

