/*
 * bsk.h
 *
 *  Created on: Oct 11, 2016
 *      Author: timppa
 */

#ifndef BSK_H_
#define BSK_H_

#define ERROR_IN_GAME -1

#define FIRST_THROW 1
#define SECOND_THROW 2

#define READ_OK			0
#define ERR_PARAM_NULL  -1
#define ERR_BAD_THROW   -2
#define ERR_READ_FAILED -3
#define ERR_BAD_FRAME   -4

#define IS_VALID_FRAME 0
#define VALID_FRAME_BAD_FRAME -1
#define VALID_FRAME_DATA_OVER_BOUNDS 1

typedef struct bsk_frame {
	int first_throw;
	int second_throw;
} bsk_frame_t;

#define BSK_FRAMES_IN_GAME 10

typedef struct bsk_game {
	bsk_frame_t frames[BSK_FRAMES_IN_GAME];
} bsk_game_t;

int bsk_get_throw(bsk_frame_t* pFrame,int index);
int bsk_calculate(bsk_game_t* pGame,int frames);

int play_game();

#endif /* BSK_H_ */
