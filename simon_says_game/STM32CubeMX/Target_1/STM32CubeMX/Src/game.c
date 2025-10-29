#include <stdlib.h>

#include "stm32c031xx.h"
#include "game.h"
#include "delay.h"

#define SEQUENCE_LENGTH 10U
#define LED_INTERVAL 500U
#define AMOUNT_COLORS 3U
#define RED 'r'
#define GREEN 'g'
#define BLUE 'b'

// prototypes
static void startingScreen(DebouncedButton_TypeDef *startButton, LCD_TypeDef *lcd);
static void populateSequence(char *sequenceArr, uint8_t sequenceLength);
static void displaySequence(LED_TypeDef *leds, uint16_t interval, char *sequenceArr, uint8_t sequenceLength);
static void userGuesses(Button_TypeDef *buttons, LED_TypeDef *leds, uint16_t interval);
static void endingScreen(DebouncedButton_TypeDef *restartButton, uint8_t finalScore);

static volatile GameState state = WAIT_FOR_START;
static char colorSequence[SEQUENCE_LENGTH];
//static char userSequence[SEQUENCE_LENGTH];
static uint8_t userSeqIndex = 0U;
static uint8_t score = 0U;

void gameLoop(LED_TypeDef *leds, Button_TypeDef *buttons, LCD_TypeDef *lcd) {
	switch(state) {
		case WAIT_FOR_START:  
			startingScreen(&buttons->redButton, lcd);
			break;
		case CREATE_SEQUENCE:
			populateSequence(colorSequence, SEQUENCE_LENGTH);
			state = SHOW_SEQUENCE;
			break;
		case SHOW_SEQUENCE:
			displaySequence(leds, LED_INTERVAL, colorSequence, SEQUENCE_LENGTH);
			state = SELECT_SEQUENCE;
			break;
		case SELECT_SEQUENCE:
			if(userSeqIndex == SEQUENCE_LENGTH) {
				state = GAME_OVER;
				break;
			}
			
			userGuesses(buttons, leds, LED_INTERVAL);
			break;
		case GAME_OVER:
			endingScreen(&buttons->redButton, score);
			// show final score and ask to play again
			break;
		default:
			break;
	}
}

static void onStartButtonPressed(void) {
	state = CREATE_SEQUENCE;
}

static void startingScreen(DebouncedButton_TypeDef *startButton, LCD_TypeDef *lcd){
	LCD_writeText(lcd, "Press red button to start game", 30);
	
	while(!Button_readPress(startButton)){
		newDelay_ms(5);
	}
	onStartButtonPressed();
	// wait for 5ms before checking again to let cpu sleep
	
}

static void populateSequence(char *sequenceArr, uint8_t sequenceLength){
	// use srand() to specifify seed
	for(uint8_t i = 0; i < sequenceLength; ++i) {
		uint8_t value = rand() % AMOUNT_COLORS;
		
		switch(value) {
			case 0:
				sequenceArr[i] = RED;
				break;
			case 1:
				sequenceArr[i] = GREEN;
				break;
			case 2:
				sequenceArr[i] = BLUE;
				break;
			default:
				sequenceArr[i] = RED;
				break;
		}
	}
}

static void displaySequence(LED_TypeDef *leds, uint16_t interval, char *sequenceArr, uint8_t sequenceLength) {
	// wait 1 second before displaying sequence
	newDelay_ms(1000);
	for(uint8_t i = 0; i < sequenceLength; ++i) {
		STM_PinDef led = LED_retrieveLEDByChar(leds, sequenceArr[i]);
		LED_turnOnLED(led);
		newDelay_ms(interval);
		LED_turnOffLED(led);
		newDelay_ms(interval);
	}
}

static uint8_t checkGuess(char color) {
	return color == colorSequence[userSeqIndex];
}

static void onIncorrectGuess(void) {
	// do things when user guessed incorrectly
	
}

static void onCorrectGuess(void) {
	// do things when user guessed correctly
	score++;
}

static void flashLED(const STM_PinDef *led, uint16_t interval) {
	led->port->BSRR = 1U << led->pin_number;
	newDelay_ms(interval);
	led->port->BSRR = 1U << (led->pin_number + 16U);
}

static void onUserGuess(char color, const STM_PinDef *led, uint16_t interval) {
	if(checkGuess(color)) onCorrectGuess();
	else onIncorrectGuess();
	
	flashLED(led, interval);
	
	++userSeqIndex;
	//userSequence[userSeqIndex++] = color;
}

static void userGuesses(Button_TypeDef *buttons, LED_TypeDef *leds, uint16_t interval) {
	if(Button_readPress(&buttons->redButton)) onUserGuess(RED, &leds->redLED, interval);
	if(Button_readPress(&buttons->greenButton)) onUserGuess(GREEN, &leds->greenLED, interval);
	if(Button_readPress(&buttons->blueButton)) onUserGuess(BLUE, &leds->blueLED, interval);
	newDelay_ms(5);
}

static void ResetStats(void) {
	score = 0;
	userSeqIndex = 0;
	state = CREATE_SEQUENCE;
}

static void onRestart(void) {
	ResetStats();
}

static void endingScreen(DebouncedButton_TypeDef *restartButton, uint8_t finalScore) {
	// display score
	// display wanna play again message
	// read restart button
	(void)finalScore;
	while(!Button_readPress(restartButton)){
		newDelay_ms(10);
	}
		
	onRestart();
}