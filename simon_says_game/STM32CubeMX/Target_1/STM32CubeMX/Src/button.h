#ifndef _BUTTON_
#define _BUTTON_

#include <stm32c031xx.h>

typedef enum {
	NOT_DEBOUNCING,
	DEBOUNCING
} DebounceState;

typedef struct {
	STM_PinDef pinInfo;
	uint8_t isPressed;
	DebounceState state;
	uint32_t lastTrackedTime;
} DebouncedButton_TypeDef;

typedef struct {
	DebouncedButton_TypeDef redButton;
	DebouncedButton_TypeDef greenButton;
	DebouncedButton_TypeDef blueButton;
} Button_TypeDef;

void Button_init(Button_TypeDef *buttons);
uint8_t Button_readPress(DebouncedButton_TypeDef *button);

#endif