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
	if ( 0==pFrame )
	{
		return ERR_PARAM_NULL;
	}
	if (index == 1 || index == 2)
	{
		if (1 == i2c_read(0x90, 0, 0, data, 1))
		{
			if (index == 1)
			{
				pFrame->first_throw = data[0];
			}
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
int bsk_calculate(bsk_game_t* pGame,int frames)
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
	if ( 0 == pFrame )
	{
		return -1;
	}
	int firstThrow = pFrame->first_throw;
	int secondThrow = pFrame->second_throw;
	int throwSum = firstThrow + secondThrow;
	if (firstThrow < 0 || firstThrow > 10
			|| secondThrow < 0 || secondThrow > 10
			|| throwSum < 0 || throwSum > 10)
	{
		return 1;
	}

	// No errors detected
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
	bsk_game_t bsk_game;
	int frameNumber = 0;
	int throwNumber = 0;


	for(int i = 0; i < BSK_FRAMES_IN_GAME; i++)
	{
		bsk_game.frames[i].first_throw = 0;
		bsk_game.frames[i].second_throw = 0;
	}

	//
	// show initial score (zero)
	//
	disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));

	do
	{
		disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));
		throwNumber = 1;
		if (READ_OK != bsk_get_throw(bsk_game.frames + frameNumber, throwNumber))
		{
			return -1;
		}
		if (0 != bsk_valid_frame(bsk_game.frames + frameNumber))
		{
			(bsk_game.frames + frameNumber)->first_throw = 0;
			(bsk_game.frames + frameNumber)->second_throw = 0;
			continue;
		}

		disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));
		throwNumber = 2;
		if (READ_OK != bsk_get_throw(bsk_game.frames + frameNumber, throwNumber))
		{
			return -1;
		}
		if (0 != bsk_valid_frame(bsk_game.frames + frameNumber))
		{
			(bsk_game.frames + frameNumber)->first_throw = 0;
			(bsk_game.frames + frameNumber)->second_throw = 0;
			continue;
		}
		disp_show_decimal(bsk_calculate(&bsk_game, BSK_FRAMES_IN_GAME));
		frameNumber++;
		delay_1s();
	}
	while (frameNumber < BSK_FRAMES_IN_GAME);

	while ( 1 )
	{

	}

	return -1;
}
