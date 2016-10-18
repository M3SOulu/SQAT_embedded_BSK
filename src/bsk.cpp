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
//   ERR_BAD_THROW if index is not 1 or 2
//   1 for reading the first throw
//   2 for reading the second throw
//   ERR_READ_FAILED if i2c_read returns other than 1
//
int bsk_get_throw(bsk_frame_t* pFrame, int index)
{
	char data[1] = "";
	if ( 0 == pFrame )
	{
		return ERR_PARAM_NULL;
	}
	if (index == FIRST_THROW || index == SECOND_THROW)
	{
		if (1 == i2c_read(0x90, 0, 0, data, 1))
		{
			if (index == FIRST_THROW)
			{
				pFrame->first_throw = data[0];
			}
			// index == SECOND_THROW
			else
			{
				pFrame->second_throw = data[0];
			}
			return READ_OK;
		}
		else
		{
			return ERR_READ_FAILED;
		}
	}
	else
	{
		return ERR_BAD_THROW;
	}

	//
	// reminder about pointers:
	//
	// pFrame->first_throw  gives the value of "first_throw"
	//
	// pFrame->first_throw = 2; set the value of "first_throw"
	//
}

//
// calculate the sum of points of "frames" first frames
//
int bsk_calculate(bsk_game_t* pGame, int frames)
{
	if ( 0==pGame ){
		return ERR_PARAM_NULL;
	}

	int totalPoints=0;

	for (int i = 0; i < frames; i++)
	{
		totalPoints += pGame->frames[i].first_throw;
		totalPoints += pGame->frames[i].second_throw;
	}

	return totalPoints;
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
	// Checking that pFrame in not NULL
	if ( 0 == pFrame )
	{
		return VALID_FRAME_BAD_FRAME;
	}

	// Checking that maximum 10 pins are used
	int firstThrow = pFrame->first_throw;
	int secondThrow = pFrame->second_throw;
	int throwSum = firstThrow + secondThrow;
	if (firstThrow < 0 || firstThrow > 10
			|| secondThrow < 0 || secondThrow > 10
			|| throwSum < 0 || throwSum > 10)
	{
		return VALID_FRAME_DATA_OVER_BOUNDS;
	}

	// No errors detected
	return IS_VALID_FRAME;
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
	bsk_game_t bsk_game;
	int frameNumber = 0;
	int throwNumber = 0;


	// initializes the game data
	for(int i = 0; i < BSK_FRAMES_IN_GAME; i++)
	{
		bsk_game.frames[i].first_throw = 0;
		bsk_game.frames[i].second_throw = 0;
	}

	// show initial score (zero)
	disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));

	do
	{
		int valid_frame_status = IS_VALID_FRAME;
		for (throwNumber = 1; throwNumber <=2; throwNumber++)
		{
			int get_throw_status = bsk_get_throw(bsk_game.frames + frameNumber, throwNumber);
			if (READ_OK != get_throw_status)
			{
				return ERROR_IN_GAME;
			}
			else
			{
				valid_frame_status = bsk_valid_frame(bsk_game.frames + frameNumber);
				if (IS_VALID_FRAME == valid_frame_status)
				{

				}
				else if (VALID_FRAME_BAD_FRAME == valid_frame_status)
				{
					return ERROR_IN_GAME;
				}
				else if (VALID_FRAME_DATA_OVER_BOUNDS == valid_frame_status)
				{
					(bsk_game.frames + frameNumber)->first_throw = 0;
					(bsk_game.frames + frameNumber)->second_throw = 0;
					continue;
				}
			}
			disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));
		}

/*		throwNumber = 2;
		if (READ_OK != bsk_get_throw(bsk_game.frames + frameNumber, throwNumber))
		{
			return ERROR_IN_GAME;
		}
		else if (IS_VALID_FRAME != bsk_valid_frame(bsk_game.frames + frameNumber))
		{
			(bsk_game.frames + frameNumber)->first_throw = 0;
			(bsk_game.frames + frameNumber)->second_throw = 0;
			continue;
		}
		disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));
*/
		if (IS_VALID_FRAME == valid_frame_status)
		{
			frameNumber++;
		}
		delay_1s();
	}
	while (frameNumber < BSK_FRAMES_IN_GAME);

	while ( 1 )
	{
delay_1s();
	}

	return -1;
}
