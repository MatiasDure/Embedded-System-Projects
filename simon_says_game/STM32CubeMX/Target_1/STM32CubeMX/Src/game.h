#ifndef _GAME_
#define _GAME_

#include "led.h"
#include "button.h"

typedef enum {
	WAIT_FOR_START,
	CREATE_SEQUENCE,
	SHOW_SEQUENCE,
	SELECT_SEQUENCE,
	GAME_OVER
} GameState;

void gameLoop(LED_TypeDef *leds, Button_TypeDef *buttons);


#endif