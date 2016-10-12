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

	char pinsKnocked = 0;
	if (i2c_read(0x90, 0, 0, &pinsKnocked, 1) != 1)
		return ERR_READ_FAILED;

	if (index == 1) {
		pFrame->first_throw = pinsKnocked;
		return pFrame->first_throw;
	} else if (index == 2) {
		pFrame->second_throw = pinsKnocked;
		return pFrame->second_throw;
	}
	//
	// reminder about pointers:
	//
	// pFrame->first_throw  gives the value of "first_throw"
	//
	// pFrame->first_throw = 2; set the value of "first_throw"
	//

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
	int sum=0;

	for (int i = 0; i < frames; i++) {
		sum += pGame->frames[i].first_throw;
		sum += pGame->frames[i].second_throw;
	}


	return sum;
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

	if (pFrame->first_throw > 10 || pFrame->first_throw < 0 ||
		pFrame->second_throw > 10 || pFrame->second_throw < 0 ||
		(pFrame->first_throw + pFrame->second_throw) > 10) {
		return 1;
	}

	return 0;
}

int sum=0;
bsk_game_t bsk_game;
int err = 0;
int nextThrow = 1;
int validFrames = 0;
int displayValidFrames = 0;
bsk_frame_t* frame;

void update_score() {
	sum = bsk_calculate(&bsk_game, displayValidFrames);
	disp_show_decimal( sum );
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
	//
	// show initial score (zero)
	//
	disp_show_decimal( sum );

	if (validFrames >= 10)
		return -1;

	frame = &bsk_game.frames[validFrames];
	err = bsk_get_throw(frame, nextThrow);

	if (err == ERR_PARAM_NULL || err == ERR_READ_FAILED || err == ERR_BAD_THROW) {
		nextThrow = 1;
		return -1;
	}

	if (nextThrow == 1) {
		frame->second_throw = 0;
	}

	if (bsk_valid_frame(frame) != 0) {
		frame->first_throw = 0;
		frame->second_throw = 0;
		nextThrow = 1;

		update_score();

		return -1;
	}

	if (frame->first_throw == 10) {
		nextThrow = 1;
	} else {
		nextThrow++;
		if (nextThrow == 3)
			nextThrow = 1;
	}

	if (nextThrow == 1)
		validFrames++;

	if (nextThrow == 2) {
		displayValidFrames = validFrames + 1;
	} else {
		displayValidFrames = validFrames;
	}

	update_score();

	return -1;
}

