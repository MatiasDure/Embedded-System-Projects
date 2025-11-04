#ifndef _TIMER_
#define _TIMER_

#include "stm32c031xx.h"

typedef struct {
	TIM_TypeDef *timer;
	STM_PinDef buzzerPin;
} Timer_TypeDef;

void Timer_init(Timer_TypeDef *timer);
void Timer_updateFreq(TIM_TypeDef *timer, uint16_t autoReloadValue, uint8_t dutyCyclePercentage);

#endif