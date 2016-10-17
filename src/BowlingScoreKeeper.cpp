/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#include "swm.h"
#include "delay.h"
#include "display.h"
#include "i2c.h"
#include "bsk.h"

int main(void)
{
#if defined (__USE_LPCOPEN)
    SystemCoreClockUpdate();
#endif

    // Configuration of SWM and I2C
    swm_config_i2c();
    i2c_reset();

    // Display setup
	disp_on( DISP_SHOW_NONE );
	disp_reset( DISP_SHOW_NONE );
	// disp_reset( DISP_SHOW_ALL );

	// Main loop -- you do not have to touch it
	while( 1 )
	{
		play_game();
		delay_1s();
	}
    return 0 ;
}
