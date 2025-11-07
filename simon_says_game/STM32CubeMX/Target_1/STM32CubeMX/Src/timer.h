#ifndef _TIMER_
#define _TIMER_

#include "stm32c031xx.h"

typedef struct {
	TIM_TypeDef *timer;
	STM_PinDef buzzerPin;
} Timer_TypeDef;

void Timer_init(Timer_TypeDef *timer);
void Timer_updateFreq(TIM_TypeDef *timer, uint16_t frequency, uint8_t dutyCyclePercentage);
void Timer_enablePWM(TIM_TypeDef *timer);
void Timer_disablePWM(TIM_TypeDef *timer);

#endif