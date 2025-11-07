#include "stm32c031xx.h"
#include "bsp.h"
#include "delay.h"
#include "lcd1602.h"
#include "led.h"
#include "button.h"
#include "game.h"
#include "timer.h"
#include "sound.h"

void BSP_usartInit(void);
void BSP_portsInit(void);

LCD_TypeDef lcd = {
	{ GPIOB, LCD_REGISTER_SELECT_PB0 },
	{ GPIOB, LCD_ENABLE_SIGNAL_PB1 },
	{ GPIOB, LCD_DB4_PB2 },
	{ GPIOB, LCD_DB5_PB3 },
	{ GPIOB, LCD_DB6_PB4 },
	{ GPIOB, LCD_DB7_PB5 }
};

LED_TypeDef led = {
	{ GPIOA, LED_BLUE_PA15 },
	{ GPIOA, LED_RED_PA6 },
	{ GPIOA, LED_GREEN_PA5 },
};

Button_TypeDef buttons = {
	{ {GPIOA, BUTTON_RED_PA0}, 0U, NOT_DEBOUNCING, 0U },
	{ {GPIOA, BUTTON_GREEN_PA4}, 0U, NOT_DEBOUNCING, 0U },
	{ {GPIOA, BUTTON_BLUE_PA8}, 0U, NOT_DEBOUNCING, 0U },
};

Timer_TypeDef timer = {
	TIM14,
	{ GPIOA, BUZZER_PWM_TIM1_CH1_PA7 }
};

void BSP_Init(void) {
	BSP_portsInit();
	DelayService_init();
	LCD_init(&lcd);
	LED_LEDsInit(&led);
	Button_init(&buttons);
	Timer_init(&timer);
}

void BSP_portsInit(void) {
	// enable clock for port A and B
	RCC->IOPENR |= (1U << 0U) | (1U << 1U);
	// enable APB clock for TIM14 peripheral (bit 15)
	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;
}

void BSP_Run(void) {
	 gameLoop(&led, &buttons, &lcd, timer.timer);
}
