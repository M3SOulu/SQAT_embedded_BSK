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
	if( index > 2 || index < 1) {
		return ERR_BAD_THROW_INDEX;
	}
	if ( 0==pFrame ){
		return ERR_PARAM_NULL;
	}
	char data = 11;
	while(1) {
		while(data > 10) {
			if(i2c_read(0x90, 0, 0, &data, 1) != 1) {
				return ERR_READ_FAILED;
			}
			if(data > 10) {
				index = 1;
			}
		}

		if(index == 1) {
			pFrame->first_throw = data;
			if(pFrame->first_throw == 10) {
				pFrame->second_throw = 0;
				return index;
			}
		}
		if(index == 2) {
			pFrame->second_throw = data;
			if(pFrame->first_throw + pFrame->second_throw > 10) {
				index = 1;
			} else {
				return index;
			}
		}
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
	for(int i = 0; i <= frames; i++) {
		sum = pGame->frames[i].first_throw +pGame->frames[i].second_throw;
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

	return -1;
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
	int current = 1;
	int ret;

	while(1) {
		ret = bsk_get_throw(&(bsk_game.frames[f]), current);

		if(ret == current) {
			current += 1;
			if(ret == 2 && bsk_valid_frame(&(bsk_game.frames[f]))) {
				sum = bsk_calculate(&bsk_game, f);
				f = f + 1;
				if(f > 9) {
					break;
				}
				current = 1;
			}
		} else {
			if(current == 2 && ret == 1) {
				current = 1;
				continue;
			}
			//return -1;
		}


		sum = bsk_calculate(&bsk_game, f);

	//
	// show initial score (zero)
	//
		disp_show_decimal( sum );
		delay_1s();
	}
	disp_show_decimal( sum );
			delay_1s();
	return -1;
}
