#ifndef __LCD_1602__
#define __LCD_1602__

#include <stm32c031xx.h>
#include "delay.h"

typedef struct {
	STM_PinDef rs;
	STM_PinDef enable;
	STM_PinDef db4;
	STM_PinDef db5;
	STM_PinDef db6;
	STM_PinDef db7;
} LCD_TypeDef;

void LCD_init(LCD_TypeDef *lcd);
void LCD_writeCommand(LCD_TypeDef *lcd, uint8_t command, delayType delayFunc, uint32_t delayTime);
void LCD_writeData(LCD_TypeDef *lcd, uint8_t data, delayType delayFunc, uint32_t delayTime);
void LCD_writeText(LCD_TypeDef *lcd, const char *text, uint8_t length);
void LCD_clearScreen(LCD_TypeDef *lcd);
void LCD_displayControl(LCD_TypeDef *lcd, uint8_t displayOn, uint8_t cursorOn, uint8_t cursorBlinkingOn);

#endif