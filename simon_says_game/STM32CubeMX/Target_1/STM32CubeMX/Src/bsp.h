#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>

// output peripherals
// leds ---------
#define LED_GREEN_PA5 5U
#define LED_RED_PA6 6U
#define LED_BLUE_PA15 15U
// lcd ---------
#define LCD_REGISTER_SELECT_PB0 0U
#define LCD_ENABLE_SIGNAL_PB1 1U
// In 4-bit mode only DB4 - DB7 data busses are used
#define LCD_DB4_PB2 2U
#define LCD_DB5_PB3 3U
#define LCD_DB6_PB4 4U
#define LCD_DB7_PB5 5U

// input peripherals
// buttons ---------
#define BUTTON_GREEN_PA4 4U
#define BUTTON_RED_PA7 7U // need to change this one to a different pin
#define BUTTON_BLUE_PA8 8U

// alternate function peripherals
// buzzer ----------
//#define BUZZER_PWM_TIM1_CH1_PA0 0U
#define BUZZER_PWM_TIM1_CH1_PA7 7U
// UART ----------
#define USART2_TX_PA2 2U
#define USART2_RX_PA3 3U

// analog peripherals
#define POTENTIOMETER_ADC_IN1_PA1 1U

void BSP_Init(void);
void BSP_waitForCharacter(void);

#endif // __BSP_H__