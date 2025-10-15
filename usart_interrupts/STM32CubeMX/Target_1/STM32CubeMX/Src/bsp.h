#ifndef __BSP_H__
#define __BSP_H__

#define LED_PA9 9U
#define LED_PA10 10U
#define LED_PA15 15U

void BSP_Init(void);
void BSP_turnGreenLED(void);
void BSP_turnBlueLED(void);
void BSP_turnRedLED(void);
void BSP_turnOffGreenLED(void);
void BSP_turnOffBlueLED(void);
void BSP_turnOffRedLED(void);
void BSP_waitForCharacter(void);

#endif // __BSP_H__