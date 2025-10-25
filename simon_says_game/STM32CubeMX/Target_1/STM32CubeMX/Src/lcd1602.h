#ifndef __LCD_1602__
#define __LCD_1602__

#include <stm32c031xx.h>
#include "delay.h"

typedef struct {
	GPIO_TypeDef *port;
	uint8_t pin_number;
} LCD_PinDef;

typedef struct {
	LCD_PinDef rs;
	LCD_PinDef enable;
	LCD_PinDef db4;
	LCD_PinDef db5;
	LCD_PinDef db6;
	LCD_PinDef db7;
} LCD_TypeDef;

void LCD_init(LCD_TypeDef *lcd);
void LCD_WriteCommand(LCD_TypeDef *lcd, uint8_t command, delayType delayFunc, uint32_t delayTime);
void LCD_WriteData(LCD_TypeDef *lcd, uint8_t data, delayType delayFunc, uint32_t delayTime);

#endif