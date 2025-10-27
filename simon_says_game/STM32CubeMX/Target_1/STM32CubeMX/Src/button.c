#include "button.h"
#include "delay.h"

static inline void button_set_input(DebouncedButton_TypeDef *button, uint8_t length) {
	for(uint8_t i = 0; i < length; i++) {
		button[i].pinInfo.port->MODER &= ~(3U << (button[i].pinInfo.pin_number * 2U));
		button[i].pinInfo.port->PUPDR &= ~(3U << (button[i].pinInfo.pin_number * 2U));
		button[i].pinInfo.port->PUPDR |= (2U << (button[i].pinInfo.pin_number * 2U));
	}
}

void Button_init(Button_TypeDef *buttons) {
	button_set_input(&buttons->redButton, 3U);
}

void Button_readPress(DebouncedButton_TypeDef *button, void (*onButtonPressed)()) {
	switch(button->state) {
		case NOT_DEBOUNCING:
			// button is not pressed
			if(!(button->pinInfo.port->IDR & (1U << button->pinInfo.pin_number))) {
				button->isPressed = 0;
				return;
			}
			button->lastTrackedTime = getSysTickCounter();
			// button was already being pressed
			if(button->isPressed) return;
			button->state = DEBOUNCING;
			break;
		case DEBOUNCING:
			if(!(button->pinInfo.port->IDR & (1U << button->pinInfo.pin_number))) {
					// button was released or some contact cutoff happened
					button->state = NOT_DEBOUNCING;
					break;
				}
			if(!hasDelayElapsed(button->lastTrackedTime, 5)) return;
				
			button->state = NOT_DEBOUNCING;
			button->isPressed = 1;
			onButtonPressed();
			break;
		default:
			// shouldn't go in here
			break;
	}
}