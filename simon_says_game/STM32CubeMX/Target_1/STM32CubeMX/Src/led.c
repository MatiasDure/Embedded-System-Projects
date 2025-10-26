#include "led.h"

static inline void led_set_output(STM_PinDef *firstLedStructMember, uint8_t length) {
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

void LED_turnOnLED(STM_PinDef greenLED) {
	greenLED.port->BSRR |= 1U << greenLED.pin_number; 
}

void LED_turnOffLED(STM_PinDef greenLED){
	greenLED.port->BSRR |= 1U << (greenLED.pin_number + 16U);
}
