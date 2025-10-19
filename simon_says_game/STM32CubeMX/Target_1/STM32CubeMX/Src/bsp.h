#ifndef __BSP_H__
#define __BSP_H__

// output peripherals
#define LED_GREEN 5U
#define LED_RED 6U
#define LED_BLUE 15U

// input peripherals
#define BUTTON_GREEN 4U
#define BUTTON_RED 7U
#define BUTTON_BLUE 8U

// alternate function peripherals
#define LCD_I2C_SCL 9U
#define LCD_I2C_SDA 10U
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