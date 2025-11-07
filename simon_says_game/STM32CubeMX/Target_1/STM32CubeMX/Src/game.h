#ifndef _GAME_
#define _GAME_

#include "led.h"
#include "button.h"
#include "lcd1602.h"

typedef enum {
	WAIT_FOR_START,
	CREATE_SEQUENCE,
	SHOW_SEQUENCE,
	SELECT_SEQUENCE,
	GAME_OVER
} GameState;

void gameLoop(LED_TypeDef *leds, Button_TypeDef *buttons, LCD_TypeDef *lcd, TIM_TypeDef *timer);


#endif