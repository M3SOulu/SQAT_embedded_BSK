/*
 * display.cpp
 *
 *  Created on: Jun 6, 2016
 *      Author: timppa
 */

#include "i2c.h"
#include "hardware.h"
#include "ht16k33.h"
#include "display.h"

int disp_off()
{
	const int addr = HW_I2C_ADDR_HT16K33;
	char data[10];
	int rc=0;

	data[0] = HT16K33_CMD_OSCILLATOR_OFF;
	data[1]=0;

	rc = i2c_write( addr,data,1 );
	return rc;
}

static char disp_msg_data[10]={
		0,0,
		1,0,
		2,0,
		3,0,
		4,0,
};

static int disp_last_message = 0;

//
// set all display data to either "all on" or "all off"
//
static void disp_set_all(int alloff)
{
	char value = 0;
	switch( alloff ){
	case DISP_SHOW_NONE:
		value = SEGMENTS_NONE;
		break;
	case DISP_SHOW_ALL:
		value = SEGMENTS_ALL;
		break;
	default:
		return;
	}
	disp_msg_data[1] = value;
	disp_msg_data[3] = value;
	disp_msg_data[5] = value;
	disp_msg_data[7] = value;
	disp_msg_data[9] = value;
}

//
// reset display data to all off/on
// clear display message value
//
void disp_reset(int alloff)
{
	disp_set_all( alloff );
	disp_last_message = 0;
}

//
// power on display, set the clocking and initial data
// - data is all off/on
//
int disp_on(int alloff)
{
	const int addr = HW_I2C_ADDR_HT16K33;
	char disp_cmd_data[10];
	int rc=0;

	disp_cmd_data[0] = HT16K33_CMD_OSCILLATOR_ON;
	disp_cmd_data[1]=0;

	rc = i2c_write( addr,disp_cmd_data,1 );
	if ( 0 <= rc ){
		disp_cmd_data[0] = HT16K33_BLINK_CMD | 0x01;
		rc = i2c_write( addr,disp_cmd_data,1 );
	} else {
		return rc;
	}
	if ( 0 <= rc ){
		disp_cmd_data[0] = HT16K33_CMD_BRIGHTNESS | 0x08;
		rc = i2c_write( addr,disp_cmd_data,1 );
	} else {
		return rc;
	}
	disp_set_all( alloff );
	rc = i2c_write( addr,disp_msg_data,10 );

	return rc;
}

/************************************************************************
 *
 *   IMPLEMENTATION BELOW
 *
 ************************************************************************/

//


//
// number segments are displayed with combination
// of following values:
//
//   1 1 1
// 32     2
// 32     2
// 32     2
//   64 64
// 16     4
// 16     4
// 16     4
//   8 8 8
//
//
// define these (correctly), now the all display as "-"
//
// 0 (zero) is 1+2+4+8+16+32 = 63
#define SEGMENTS_A 119
// 1 (one)  is 2+4 = 6
#define SEGMENTS_E 121
#define SEGMENTS_d 94
#define SEGMENTS_n 84
#define SEGMENTS_m 55
#define SEGMENTS_o 92
#define SEGMENTS_P 115
#define SEGMENTS_s 109
#define SEGMENTS_w 126
#define SEGMENTS_9 00

//
// mapping of number to its segment data:
//   element index  |  segment data
//   0              |  63 (segments for zero)
//   1              |   6 (segments for one)
/*const char digit_segments[10]={
		SEGMENTS_A,0,119,1,0,2,0,3,0,4,0,
		SEGMENTS_E,0,121,1,0,2,0,3,0,4,0,
		SEGMENTS_d,0,94,1,0,2,0,3,0,4,0,
		SEGMENTS_n,0,84,1,0,2,0,3,0,4,0,
		SEGMENTS_m,0,55,1,0,2,0,3,0,4,0,
		SEGMENTS_m,0,55,1,0,2,0,3,0,4,0,
		SEGMENTS_o,0,92,1,0,2,0,3,0,4,0,
		SEGMENTS_P,0,115,1,0,2,0,3,0,4,0,
		SEGMENTS_s,0,109,1,0,2,0,3,0,4,0,
		SEGMENTS_w,0,126,1,0,2,0,3,0,4,0,
};*/



//
// return the Nth rightmost digit from value
//   value | n | result
//   ------+---+-------
//   417   | 0 | 7
//   417   | 1 | 1
//   417   | 2 | 4
//   417   | 3 | 0
//


int disp_digit_of(int value,unsigned int n)
{
    int length = 0;
    int i = 0;
    int returnNumber = -1;
    //calculate the length of the value
    do{
        value /= 10;
        length++;
    }while(value != 0);
    //value to array
    int array[length];
    do{
        array[i] = value % 10;
        value /= 10;
        i++;
    }while(value != 0);
    //if position is off limits
    if(length < n) return -1;
    returnNumber = array[n];
    return returnNumber;

    }

//
// map decimal numbers of "value" to digits in the
// 7-segment display: calculate what segments to
// show on each position so that "value" is displayed
//
int disp_show_decimal(int value)
{
	const int addr = HW_I2C_ADDR_HT16K33;

	disp_msg_data[1] = SEGMENTS_E;
	disp_msg_data[2] = SEGMENTS_A;


	return i2c_write( addr,disp_msg_data,10 );
}
