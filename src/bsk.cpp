/*
 * bsk.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: timppa
 */

#include "hardware.h"
#include "i2c.h"
#include "delay.h"
#include "display.h"
#include "bsk.h"

/************************************************************************
 *
 *   IMPLEMENTATION BELOW
 *
 ************************************************************************/

//
// get_throw reads the BSK pin counting device and returns
// the number of knocked pins via the "pFrame" structure
//
// return value for the subroutine is:
//   ERR_PARAM_NULL if pFrame is zero/NULL
//   ERR_BAD_THROW_INDEX if index is not 1 or 2
//   1 for reading the first throw
//   2 for reading the second throw
//   ERR_READ_FAILED if i2c_read returns other than 1
//
int bsk_get_throw(bsk_frame_t* pFrame,int index)
{
	if ( 0==pFrame ){
		return ERR_PARAM_NULL;
	}

	//
	// reminder about pointers:
	//
	// pFrame->first_throw  gives the value of "first_throw"
	//
	// pFrame->first_throw = 2; set the value of "first_throw"
	//

	char c_data;
	i2c_read(0x90, 0, 0, &c_data, 1);
	int i_data = c_data - '0';

	if (index == 1) {
		pFrame->first_throw = i_data;
		if (i_data == 10) {
			pFrame->second_throw = 0;
		}
		return index;
	} else if (index == 2) {
		pFrame->second_throw = i_data;
		return index;
	}

	return ERR_BAD_THROW;
}

//
// calculate the sum of points of "frames" first frames
//
int bsk_calculate(bsk_game_t* pGame,int frames)
{
	if ( 0==pGame ){
		return ERR_PARAM_NULL;
	}

	int sum = 0;

	for (int i=0; i<frames; i++) {
		sum += pGame->frames[i].first_throw;
		sum += pGame->frames[i].second_throw;
	}

	return -1;
}

//
// return "0" (zero) of whole frame is valid
//
// return -1 if pFrame (pointer) is invalid (NULL)
//
// return 1 if frame content is bad (first or second throw or sum)
//
int bsk_valid_frame(bsk_frame_t* pFrame)
{
	if ( 0==pFrame ){
		return -1;
	}
	if (pFrame->first_throw < 0 || pFrame->second_throw < 0
			|| pFrame->first_throw + pFrame->second_throw > 10) {
		return 1;
	}

	return 0;
}

//
// play the full game
// - display points initially
// - get score for throw and spend its delay
// - get score for 2nd throw and spend its delay
// - and so on
//
int play_game()
{
	// use these variables if you wish; they are not compulsory
	int sum=0;
	bsk_game_t bsk_game;
	int f=0;

	//
	// show initial score (zero)
	//
	disp_show_decimal( sum );

	// points of one throw
	int t_value = 0;

	for (int t=0; t<10; t++) {
		// get value for first throw
		bsk_frame_t* pFrame;
		bsk_get_throw(pFrame, 1);
		t_value = pFrame->first_throw;
		sum += t_value;

		delay_1s();

		if (t_value < 10) {
			// get value for second throw
			bsk_get_throw(pFrame, 2);
			t_value = pFrame->second_throw;
			sum += t_value;
		}

		delay_1s();

		disp_show_decimal(sum);
	}

	return -1;
}
