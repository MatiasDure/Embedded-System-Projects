#include <stdlib.h>

#include "stm32c031xx.h"
#include "game.h"
#include "delay.h"

#define SEQUENCE_LENGTH 10U
#define LED_INTERVAL 500U
#define AMOUNT_COLORS 3U

// prototypes
static void startingScreen(DebouncedButton_TypeDef *startButton);
static void populateSequence(char *sequenceArr, uint8_t sequenceLength);
static void displaySequence(LED_TypeDef *leds, uint16_t interval, char *sequenceArr, uint8_t sequenceLength);

static volatile GameState state = WAIT_FOR_START;
static char colorSequence[SEQUENCE_LENGTH];
static char userSequence[SEQUENCE_LENGTH];

void gameLoop(LED_TypeDef *leds, Button_TypeDef *buttons) {
	switch(state) {
		case WAIT_FOR_START:
			startingScreen(&buttons->redButton);
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
			break;
		case GAME_OVER:
			break;
		default:
			break;
	}
}

static void onStartButtonPressed(void) {
	state = CREATE_SEQUENCE;
}

static void startingScreen(DebouncedButton_TypeDef *startButton){
	Button_readPress(startButton, onStartButtonPressed);
	// wait for 5ms before checking again to let cpu sleep
	newDelay_ms(5);
}

static void populateSequence(char *sequenceArr, uint8_t sequenceLength){
	// use srand() to specifify seed
	for(uint8_t i = 0; i < sequenceLength; ++i) {
		uint8_t value = rand() % AMOUNT_COLORS;
		
		switch(value) {
			case 0:
				sequenceArr[i] = 'r';
				break;
			case 1:
				sequenceArr[i] = 'g';
				break;
			case 2:
				sequenceArr[i] = 'b';
				break;
			default:
				sequenceArr[i] = 'r';
				break;
		}
	}
}

static void displaySequence(LED_TypeDef *leds, uint16_t interval, char *sequenceArr, uint8_t sequenceLength) {
	for(uint8_t i = 0; i < sequenceLength; ++i) {
		STM_PinDef led = LED_retrieveLEDByChar(leds, sequenceArr[i]);
		LED_turnOnLED(led);
		newDelay_ms(interval);
		LED_turnOffLED(led);
		newDelay_ms(interval);
	}
}
