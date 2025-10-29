#include "led.h"

static inline void led_set_output(const STM_PinDef *firstLedStructMember, uint8_t length) {
	for(uint8_t i = 0; i < length; ++i) {
		firstLedStructMember[i].port->MODER &= ~(3U << (firstLedStructMember[i].pin_number * 2U));
		firstLedStructMember[i].port->MODER |= (1U << (firstLedStructMember[i].pin_number * 2U));
	}
}

void LED_LEDsInit(LED_TypeDef * leds) {
	// pass first struct member and number of struct members 
	led_set_output(&leds->blueLED, 3);
}

void LED_turnLED(LED_TypeDef * leds, char letter) {
	switch(letter) {
		case 'g':
			LED_turnOnLED(leds->greenLED);
			LED_turnOffLED(leds->blueLED);
			LED_turnOffLED(leds->redLED);
			break;
		case 'r':
			LED_turnOnLED(leds->redLED);
			LED_turnOffLED(leds->greenLED);
			LED_turnOffLED(leds->blueLED);
			break;
		case 'b':
			LED_turnOnLED(leds->blueLED);
			LED_turnOffLED(leds->greenLED);
			LED_turnOffLED(leds->redLED);
			break;
		case 'o':
			LED_turnOffLED(leds->blueLED);
			LED_turnOffLED(leds->greenLED);
			LED_turnOffLED(leds->redLED);
		default:
			// ignore other characters received for now
			break;
	}
}

STM_PinDef LED_retrieveLEDByChar(LED_TypeDef* led, char letter) {
	switch(letter) {
		case 'g':
			return led->greenLED;
		case 'r':
			return led->redLED;
		case 'b':
			return led->blueLED;
		default:
			// ignore other characters received for now and just return red
			return led->redLED;
	}
}

void LED_turnOnLED(STM_PinDef led) {
	led.port->BSRR |= 1U << led.pin_number; 
}

void LED_turnOffLED(STM_PinDef led){
	led.port->BSRR |= 1U << (led.pin_number + 16U);
}
