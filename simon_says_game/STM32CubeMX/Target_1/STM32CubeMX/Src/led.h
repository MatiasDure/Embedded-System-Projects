#ifndef _LED_
#define _LED_

#include <stm32c031xx.h>

typedef struct {
	STM_PinDef blueLED;
	STM_PinDef redLED;
	STM_PinDef greenLED;
} LED_TypeDef;

void LED_LEDsInit(LED_TypeDef *leds);
void LED_turnLED(LED_TypeDef *leds, char letter);
void LED_turnOnLED(STM_PinDef greenLED);
void LED_turnOffLED(STM_PinDef greenLED);

#endif