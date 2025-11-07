#include <stdlib.h>

#include "stm32c031xx.h"
#include "game.h"
#include "delay.h"
#include "sound.h"

#define SEQUENCE_LENGTH 5U
#define LED_INTERVAL 500U
#define AMOUNT_COLORS 3U
#define RED 'r'
#define GREEN 'g'
#define BLUE 'b'

static void startingScreen(DebouncedButton_TypeDef *startButton, LCD_TypeDef *lcd, TIM_TypeDef *timer);
static void populateSequence(char *sequenceArr, uint8_t sequenceLength);
static void displaySequence(LED_TypeDef *leds, uint16_t interval, char *sequenceArr, uint8_t sequenceLength, LCD_TypeDef *lcd, TIM_TypeDef *timer);
static void userGuesses(Button_TypeDef *buttons, LED_TypeDef *leds, uint16_t interval, LCD_TypeDef *lcd, TIM_TypeDef *timer);
static void endingScreen(DebouncedButton_TypeDef *restartButton, uint8_t finalScore, LCD_TypeDef *lcd, TIM_TypeDef *timer);

static volatile GameState state = WAIT_FOR_START;
static char colorSequence[SEQUENCE_LENGTH];
static uint8_t userSeqIndex = 0U;
static uint8_t score = 0U;
static Scroll_TypeDef lcdScroll = { 0, 0, RESETTING, 0};
static Sound_TypeDef soundState = {PLAY, 0, 0};

void gameLoop(LED_TypeDef *leds, Button_TypeDef *buttons, LCD_TypeDef *lcd, TIM_TypeDef *timer) {
	switch(state) {
		case WAIT_FOR_START:
			startingScreen(&buttons->redButton, lcd, timer);
			break;
		case CREATE_SEQUENCE:
			populateSequence(colorSequence, SEQUENCE_LENGTH);
			state = SHOW_SEQUENCE;
			break;
		case SHOW_SEQUENCE:
			displaySequence(leds, LED_INTERVAL, colorSequence, SEQUENCE_LENGTH, lcd, timer);
			state = SELECT_SEQUENCE;
			break;
		case SELECT_SEQUENCE:
			if(userSeqIndex == SEQUENCE_LENGTH) {
				state = GAME_OVER;
				break;
			}
			userGuesses(buttons, leds, LED_INTERVAL, lcd, timer);
			break;
		case GAME_OVER:
			// show final score and ask to play again
			endingScreen(&buttons->redButton, score, lcd, timer);
			break;
		default:
			break;
	}
}

static void ResetSoundState() {
	soundState.index = 0;
	soundState.state = PLAY;
	soundState.lastTimestamp = 0;
}

static void onStartButtonPressed(TIM_TypeDef *timer) {
	TurnOffSound(timer);
	ResetSoundState();
	state = CREATE_SEQUENCE;
}

static void resetScroll() {
	lcdScroll.lastTickStored = 0;
	lcdScroll.startAddress = 0xFU;
	lcdScroll.startIndex = 0;
}

static void startingScreen(DebouncedButton_TypeDef *startButton, LCD_TypeDef *lcd, TIM_TypeDef *timer){
	if(lcdScroll.state == INACTIVE) lcdScroll.state = SCROLL_IN;
	
	TurnOnSound(timer);
	PlaySequence(timer, &soundState, hedwigTheme, hedwigThemeLength, 0);
	char text[] = "Press the red button to start the game";
	LCD_writeScrollText(lcd, &lcdScroll, text, (sizeof(text) - 1), 450);
	if(Button_readPress(startButton)){
		onStartButtonPressed(timer);
	}
}

static void populateSequence(char *sequenceArr, uint8_t sequenceLength){
	// update to srand() to specifify seed
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

static void PlayColorNote(TIM_TypeDef *timer, char color, uint16_t interval) {
	switch(color) {
		case RED:
			PlaySingleNote(timer, (Note){B_NOTE_5, interval});
			break;
		case GREEN:
			PlaySingleNote(timer, (Note){G_NOTE_5, interval});
			break;
		case BLUE:
			PlaySingleNote(timer, (Note){C_NOTE_5, interval});
			break;
		default:
			break;
		
}
	}

static void displaySequence(LED_TypeDef *leds, uint16_t interval, char *sequenceArr, uint8_t sequenceLength, LCD_TypeDef *lcd, TIM_TypeDef *timer) {
	resetScroll();
	LCD_clearScreen(lcd);
	lcdScroll.state = SCROLL_IN;
	char text[] = "Don't press until the color sequence is done";
	while(lcdScroll.state != INACTIVE) {
		LCD_writeScrollText(lcd, &lcdScroll, text, (sizeof(text) - 1), 400);
	}
	
	newDelay_ms(1000);
	for(uint8_t i = 0; i < sequenceLength; ++i) {
		TurnOnSound(timer);
		STM_PinDef led = LED_retrieveLEDByChar(leds, sequenceArr[i]);
		LED_turnOnLED(led);
		PlayColorNote(timer, sequenceArr[i], interval); 
		LED_turnOffLED(led);
		TurnOffSound(timer);
		newDelay_ms(interval);
	}
}

static uint8_t checkGuess(char color) {
	return color == colorSequence[userSeqIndex];
}

static void displayScore(LCD_TypeDef *lcd) {
	LCD_placeCursorAt(lcd, 0U);
	LCD_writeText(lcd, "Score: ", 7U);
	LCD_writeNumber(lcd, score);
	LCD_writeText(lcd, "/", 1U);
	LCD_writeNumber(lcd, SEQUENCE_LENGTH);
}

// Not doing anything for incorrect at the moment
static void onIncorrectGuess() {
}

static void onCorrectGuess() {
	// do things when user guessed correctly
	++score;
}

static void DisplayUserGuessFeedback(STM_PinDef led, uint16_t interval, TIM_TypeDef *timer, char color) {
	TurnOnSound(timer);
	LED_turnOnLED(led);
	PlayColorNote(timer, color, interval);
	LED_turnOffLED(led);
	TurnOffSound(timer);
}

static void onUserGuess(char color, const STM_PinDef *led, uint16_t interval, LCD_TypeDef *lcd, TIM_TypeDef *timer) {
	
	if(checkGuess(color)) onCorrectGuess();
	else onIncorrectGuess();
	displayScore(lcd);
	
	DisplayUserGuessFeedback(*led, interval, timer, color);
	
	++userSeqIndex;
}

static void userGuesses(Button_TypeDef *buttons, LED_TypeDef *leds, uint16_t interval, LCD_TypeDef *lcd, TIM_TypeDef *timer) {
	if(Button_readPress(&buttons->redButton)) onUserGuess(RED, &leds->redLED, interval, lcd, timer);
	if(Button_readPress(&buttons->greenButton)) onUserGuess(GREEN, &leds->greenLED, interval, lcd, timer);
	if(Button_readPress(&buttons->blueButton)) onUserGuess(BLUE, &leds->blueLED, interval, lcd, timer);
	newDelay_ms(5U);
}

typedef enum {
	START,
	WAITING,
	RESET,
} EndingState;

static EndingState endingState = START;

static void ResetStats(void) {
	score = 0U;
	userSeqIndex = 0U;
	state = CREATE_SEQUENCE;
}

static void onRestart(TIM_TypeDef *timer) {
	TurnOffSound(timer);
	ResetSoundState();
	ResetStats();
	endingState = START;
}

static void displayFinalScore(LCD_TypeDef *lcd, uint8_t finalScore) {
	LCD_clearScreen(lcd);
	LCD_placeCursorAt(lcd, 0x03);
	LCD_writeText(lcd, "GAME  OVER", 10);
	LCD_placeCursorAt(lcd, 0x40);
	LCD_writeText(lcd, "Final score: ", 13);
	LCD_writeNumber(lcd, finalScore);
	LCD_writeText(lcd, "/5", 2);
}

static void endingScreen(DebouncedButton_TypeDef *restartButton, uint8_t finalScore, LCD_TypeDef *lcd, TIM_TypeDef *timer) {
	switch(endingState) {
		case START:	
			displayFinalScore(lcd, finalScore);
			TurnOnSound(timer);
			endingState = WAITING;
			break;
		case WAITING:
			PlaySequence(timer, &soundState, piratesTheme, pirateThemeLength, 1);
			if(Button_readPress(restartButton)) endingState = RESET;
			break;
		case RESET:
			onRestart(timer);
			break;
	}	
}