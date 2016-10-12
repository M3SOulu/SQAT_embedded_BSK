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
int bsk_get_throw(bsk_frame_t* pFrame, int index) {
	if (0 == pFrame) {
		return ERR_PARAM_NULL;
	}
	//
	// reminder about pointers:
	//
	// pFrame->first_throw  gives the value of "first_throw"
	//
	// pFrame->first_throw = 2; set the value of "first_throw"
	//
	if (index == 1)
		return pFrame->first_throw;
	else if (index == 2)
		return pFrame->second_throw;

	return ERR_BAD_THROW;
}

void bsk_set_throw(bsk_frame_t* pFrame, int index, int value) {
	//if (0 == pFrame) {
	//	return ERR_PARAM_NULL;
	//}
	//
	// reminder about pointers:
	//
	// pFrame->first_throw  gives the value of "first_throw"
	//
	// pFrame->first_throw = 2; set the value of "first_throw"
	//
	if (index == 1)
		pFrame->first_throw = value;
	else if (bsk_get_throw(pFrame, 1) < 10 && index == 2)
		pFrame->second_throw = value;

	//return ERR_BAD_THROW;
}

int bsk_frame_sum(bsk_frame_t* pFrame) {
	if (0 == pFrame) {
		return -1;
	}
	int sum = 0;
	sum = (bsk_get_throw(pFrame, 1) + bsk_get_throw(pFrame, 2));
	return sum;
}
//
// return "0" (zero) of whole frame is valid
//
// return -1 if pFrame (pointer) is invalid (NULL)
//
// return 1 if frame content is bad (first or second throw or sum)
//
int bsk_valid_frame(bsk_frame_t* pFrame) {
	if (0 == pFrame) {
		return -1;
	}
	if (bsk_get_throw(pFrame, 1) >= 0 && bsk_get_throw(pFrame, 2) >= 0
			&& bsk_frame_sum(pFrame) <= 10 && bsk_frame_sum(pFrame) >= 0)
		return 0;

	return -1;
}

//
// calculate the sum of points of "frames" first frames
//
int bsk_calculate(bsk_game_t* pGame, int frames) {
	if (0 == pGame) {
		return ERR_PARAM_NULL;
	}
	int sum = 0;

	for (int loop = 0; loop < frames; loop++) {
		if (bsk_valid_frame(&pGame->frames[loop]) == 0) {
			sum = sum + bsk_frame_sum(&pGame->frames[loop]);
		}
	}

	return sum;
	//return -1;
}

//
// play the full game
// - display points initially
// - get score for throw and spend its delay
// - get score for 2nd throw and spend its delay
// - and so on
//
int play_game() {
// use these variables if you wish; they are not compulsory
	int sum = 0;
	bsk_game_t* bsk_game = new bsk_game_t;
	int f = 0;
	int first_throw_value = 0;
	int second_throw_value = 0;

	disp_show_decimal(sum);

	while (f < 10) {
		// reads first frame first throw
		bsk_set_throw(&bsk_game->frames[f], 1, first_throw_value);

		if (first_throw_value == 10)
			f++;
		else // second throw value
		{
			bsk_set_throw(&bsk_game->frames[f], 2, second_throw_value);
			f++;
		}

	}

//
// show initial score (zero)
//
	bsk_calculate(bsk_game, f);
	disp_show_decimal(sum);

	return -1;
}
