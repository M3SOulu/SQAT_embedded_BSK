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
int bsk_get_throw(bsk_frame_t* pFrame, int index)
{
	if (!pFrame) {
		return ERR_PARAM_NULL;
	}

	if (index != 1 && index != 2) {
		return ERR_BAD_THROW;
	}

	char result;
	if (i2c_read(0x90, 0, 0, &result, 1) != 1) {
		return ERR_READ_FAILED;
	}

	//
	// reminder about pointers:
	//
	// pFrame->first_throw  gives the value of "first_throw"
	//
	// pFrame->first_throw = 2; set the value of "first_throw"
	//

	if (index == 1) {
		pFrame->first_throw = result;
	} else {
		pFrame->second_throw = result;
	}

	return index;
}

//
// calculate the sum of points of "frames" first frames
//
int bsk_calculate(bsk_game_t* pGame, int frames)
{
	if (!pGame) {
		return ERR_PARAM_NULL;
	}

	int sum = 0;

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
	if (!pFrame) {
		return -1;
	}

	// bad first throw
	if (pFrame->first_throw < 0 || pFrame->first_throw > 10) {
		return 1;
	}

	// bad second throw
	if (pFrame->second_throw < 0 || pFrame->second_throw > 10) {
		return 1;
	}

	// bad sum
	if (pFrame->first_throw + pFrame->second_throw > 10) {
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
	int sum = 0;
	bsk_game_t bsk_game;

	//
	// show initial score (zero)
	//
	disp_show_decimal(sum);
	delay_1s();

	//
	// subsequent throws
	//
	for (int i = 0; i < 10; i++) {
		while (1) {
			bsk_game.frames[i].first_throw = 0;
			bsk_game.frames[i].second_throw = 0;

			// first throw
			if (bsk_get_throw(&bsk_game.frames[i], 1) != 1) {
				continue;
			}

			if (bsk_valid_frame(&bsk_game.frames[i]) != 0) {
				continue;
			}

			int first_throw_score = bsk_game.frames[i].first_throw;
			if (first_throw_score == 10) {
				sum += first_throw_score;
				break;
			}

			disp_show_decimal(sum + first_throw_score);
			delay_1s();

			// second throw
			if (bsk_get_throw(&bsk_game.frames[i], 2) != 2) {
				continue;
			}

			if (bsk_valid_frame(&bsk_game.frames[i]) != 0) {
				continue;
			}

			sum += first_throw_score;
			sum += bsk_game.frames[i].second_throw;

			break;
		}

		disp_show_decimal(sum);
		delay_1s();
	}

	return -1;
}
