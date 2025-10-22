#include <stdint.h>

#ifndef __BSP_H__
#define __BSP_H__

// output peripherals
#define LED_GREEN_PA5 5U
#define LED_RED_PA6 6U
#define LED_BLUE_PA15 15U
#define LCD_REGISTER_SELECT_PB0 0U
#define LCD_ENABLE_SIGNAL_PB1 1U
// In 4-bit mode only DB4 - DB7 data busses are used
#define LCD_DB4_PB2 2U
#define LCD_DB5_PB3 3U
#define LCD_DB6_PB4 4U
#define LCD_DB7_PB5 5U

// input peripherals
#define BUTTON_GREEN_PA4 4U
#define BUTTON_RED_PA7 7U
#define BUTTON_BLUE_PA8 8U

// alternate function peripherals
#define BUZZER_PWM_TIM1_CH1_PA0 0U

// analog peripherals
#define POTENTIOMETER_ADC_IN1_PA1 1U

typedef void (*delayType)(uint32_t);

void BSP_Init(void);
void BSP_turnGreenLED(void);
void BSP_turnBlueLED(void);
void BSP_turnRedLED(void);
void BSP_turnOffGreenLED(void);
void BSP_turnOffBlueLED(void);
void BSP_turnOffRedLED(void);
void BSP_waitForCharacter(void);
void BSP_LCD_WriteCommand(uint8_t command, delayType delayFunc, uint32_t delayTime);
void BSP_LCD_WriteData(uint8_t data, delayType delayFunc, uint32_t delayTime);

#endif // __BSP_H__