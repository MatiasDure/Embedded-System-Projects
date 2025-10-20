#ifndef __BSP_H__
#define __BSP_H__

// output peripherals
#define LED_GREEN 5U
#define LED_RED 6U
#define LED_BLUE 15U
#define LCD_REGISTER_SELECT 0U
#define LCD_ENABLE_SIGNAL 1U
// In 4-bit mode only DB4 - DB7 data busses are used
#define LCD_DB4 2U
#define LCD_DB5 3U
#define LCD_DB6 4U
#define LCD_BD7 5U

// input peripherals
#define BUTTON_GREEN 4U
#define BUTTON_RED 7U
#define BUTTON_BLUE 8U

// alternate function peripherals
#define BUZZER_PWM_TIM1_CH1 0U

// analog peripherals
#define POTENTIOMETER_ADC_IN1 1U

void BSP_Init(void);
void BSP_turnGreenLED(void);
void BSP_turnBlueLED(void);
void BSP_turnRedLED(void);
void BSP_turnOffGreenLED(void);
void BSP_turnOffBlueLED(void);
void BSP_turnOffRedLED(void);
void BSP_waitForCharacter(void);

#endif // __BSP_H__