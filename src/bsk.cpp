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
	char *value;
	if (index == 1){
		int read = pFrame->first_throw =  i2c_read(0x90, 0, 0, value, 1);
		if (read != 1){
			return ERR_READ_FAILED;
		}
	}
	else if (index == 2){
		int read = pFrame->second_throw = i2c_read(0x90, 0, 0, value, 1);
		if (read != 1){
			return ERR_READ_FAILED;
		}
	}
	else {
		return ERR_BAD_THROW_INDEX;
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
	int sum=0;
	int i=0;
	for (i=0; i< frames; i++){
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
	int sum = 0;
	if (pFrame->first_throw > 10 || pFrame->first_throw < 0){
		return ERR_BAD_FRAME;

	}else {
		sum += pFrame->first_throw;
	}
	if (pFrame->second_throw > 10 || pFrame->second_throw < 0){
		return ERR_BAD_FRAME;
	}
	else {
		sum += pFrame->second_throw;
	}
	if (sum > 10){
		return ERR_BAD_FRAME;
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
	bsk_game_t *bsk_game = new bsk_game_t;
	int f=0;


	//
	// show initial score (zero)
	//
	disp_show_decimal( sum );

	while(f < BSK_FRAMES_IN_GAME){
		bsk_frame_t* frame = new bsk_frame_t;
		bsk_get_throw(frame, 1);
		bsk_get_throw(frame, 2);
		if (bsk_valid_frame(frame) == 0){
			bsk_frame_t *t = &bsk_game->frames[f];
			t = frame;
			disp_show_decimal(sum);
			f++;
		}
	}


	return -1;
}
