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
// 0 (zero) is 1+2+4+8+16+32 = 63
#define SEGMENTS_0 63
// 1 (one)  is 2+4 = 6
#define SEGMENTS_1 6

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

#define SEGMENTS_2 91
#define SEGMENTS_3 79
#define SEGMENTS_4 102
#define SEGMENTS_5 109
#define SEGMENTS_6 125
#define SEGMENTS_7 7
#define SEGMENTS_8 127
#define SEGMENTS_9 111


//
// mapping of number to its segment data:
//   element index  |  segment data
//   0              |  63 (segments for zero)
//   1              |   6 (segments for one)
const char digit_segments[10]={
		SEGMENTS_0,
		SEGMENTS_1,
		SEGMENTS_2,
		SEGMENTS_3,
		SEGMENTS_4,
		SEGMENTS_5,
		SEGMENTS_6,
		SEGMENTS_7,
		SEGMENTS_8,
		SEGMENTS_9,
};

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
	int res = value;

	int pow_ten = 1;
	for (int i=0; i<n; i++) {
		pow_ten *= 10;
	}
	// cut off right part beside digit
	res = res / pow_ten;

	// cut off left part besides the digit
	res = res % 10;

	return res;
}



//
// map decimal numbers of "value" to digits in the
// 7-segment display: calculate what segments to
// show on each position so that "value" is displayed
//
int disp_show_decimal(int value)
{
	const int addr = HW_I2C_ADDR_HT16K33;

	for (int i=0; i<4; i++) {
		if (i < 2) {
			disp_msg_data[i*2 + 1] = digit_segments[disp_digit_of(value, (3-i))];
		} else {
			disp_msg_data[(i+1)*2 + 1] = digit_segments[disp_digit_of(value, (3-i))];
		}
	}
	disp_msg_data[5] = 0;


	return i2c_write( addr,disp_msg_data,10 );
}


