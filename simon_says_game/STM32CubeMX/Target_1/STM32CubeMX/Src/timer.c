#include "timer.h"

static void triggerManualUpdateEvent(TIM_TypeDef *timer) {
	// set UG bit to trigger update of registers 
	timer->EGR = 1U << 0U;
}

void Timer_init(Timer_TypeDef *timer) {
	// setting pin as alternate function Tim14
	timer->buzzerPin.port->MODER &= ~(3U << (timer->buzzerPin.pin_number * 2U)); 
	timer->buzzerPin.port->MODER |= 2U << (timer->buzzerPin.pin_number * 2U); 
	timer->buzzerPin.port->AFR[0] &= ~(0xFU << (timer->buzzerPin.pin_number * 4U));
	timer->buzzerPin.port->AFR[0] |= 0x4U << (timer->buzzerPin.pin_number * 4U);

	// timer setup
	// set auto reload preload enable to allow ARR to be buffered (have shadow register)
	timer->timer->CR1 |= 1U << 7U;
	
	// set capture/compare mode to output, 
	// output compare mode to PWM mode 2 for inactive when TIMx_CNT < TIMx_CCR1, else active, 
	// and output compare preload enable to update duty cycle value only on UEV
	timer->timer->CCMR1 &= ~(3U << 0U);
	timer->timer->CCMR1 |= (7U << 4U) | (1U << 3U);
	
	// setting prescaler and auto-reload values: n - 1 (1 is automatically added to both values)
	// frequency formula = clock frequncy / (PSC+1) * (ARR+1)
	// if clock freq = 12MHz, to get an output frequency of 1khz (1000 hz), we can
	// set the prescaler as 11 (11+1=12): 12MHz / 12 = 1MHz and the ARR at 999 (999+1=1000)
	// 1MHz/1000 = 1khz
	timer->timer->PSC = 119U;
	timer->timer->ARR = 999U;
	
	// set duty cycle (capture/compare) value as 50% of ARR value
	timer->timer->CCR1 = 10U;
	
	triggerManualUpdateEvent(timer->timer);
	// enable counter
	timer->timer->CR1 |= 1U << 0U;
	
	// enable signal output to pin
	//timer->timer->CCER |= (1U << 0U);
}

void Timer_updateFreq(TIM_TypeDef *timer, uint16_t frequency, uint8_t dutyCyclePercentage) {	
	// this overflows a 2 bytes integer
	if(frequency < 2U || frequency > 50000U) return;
	
	uint16_t autoReloadValue = ((12000000U / 120U) / frequency);
	timer->ARR = autoReloadValue - 1U;
	timer->CCR1 = (autoReloadValue * dutyCyclePercentage) / 100U;
	triggerManualUpdateEvent(timer);
}

void Timer_enablePWM(TIM_TypeDef *timer) {
	timer->CCER |= 1U << 0U;
}

void Timer_disablePWM(TIM_TypeDef *timer) {
	timer->CCER &= ~(1U << 0U);
}

