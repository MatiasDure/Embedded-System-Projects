#include "lcd1602.h"

static void LCD_wake_up_sequence(LCD_TypeDef *lcd);
static void writeNibbles(LCD_TypeDef *lcd, uint8_t *nibbles);
static void LCD_sendData(STM_PinDef *enable_pin);
static void LCD_nibbles(uint8_t *highNibbleBuffer, uint8_t *lowNibbleBuffer, uint8_t hexValue);
static void LCD_binaryNibble(uint8_t *buffer, uint8_t nibbleValue, uint8_t index);
	
static inline void set_pin_output(STM_PinDef *lcd_pin) {
	lcd_pin->port->MODER &= ~(3U << (lcd_pin->pin_number * 2U));
	lcd_pin->port->MODER |= (1U << (lcd_pin->pin_number * 2U));
}
	
void LCD_init(LCD_TypeDef *lcd) {
	set_pin_output(&lcd->rs);
	set_pin_output(&lcd->enable);
	set_pin_output(&lcd->db4);
	set_pin_output(&lcd->db5);
	set_pin_output(&lcd->db6);
	set_pin_output(&lcd->db7);
	
	LCD_wake_up_sequence(lcd);
}

static void LCD_wake_up_sequence(LCD_TypeDef *lcd) {
	/*
		* Initialization sequence to wake up lcd
		* Information about the initialization sequence can be found in the 
		LCD-1602A datasheet: https://www.alldatasheet.com/datasheet-pdf/pdf/1574132/CRYSTAIFONTZ/LCD-1602A.html
	*/
	delay_ms(16);
	// knocking door to force lcd to wake up in 8-bit mode
	LCD_writeCommand(lcd, 0x30, &delay_ms, 5);
	LCD_writeCommand(lcd, 0x30, &delay_us, 150);
	LCD_writeCommand(lcd, 0x30, &delay_us, 1);
	// set interface to 4-bit mode
	LCD_writeCommand(lcd, 0x20, &delay_none, 0);
	// specify display lines and character font
	LCD_writeCommand(lcd, 0x28, &delay_none, 0);
	// display off
	LCD_writeCommand(lcd, 0x8, &delay_none, 0);
	// display clear
	LCD_writeCommand(lcd, 0x1, &delay_ms, 2);
	// Entry mode set
	LCD_writeCommand(lcd, 0x6, &delay_none, 0);
	
	// Set DDRAM address to 0
	LCD_writeCommand(lcd, 0x80, &delay_none, 0); 
	// turn on display
	LCD_writeCommand(lcd, 0x0C, delay_us, 40);
}

// To write in instruction input mode the RS bit needs to be set LOW
void LCD_writeCommand(LCD_TypeDef *lcd, uint8_t command, delayType delayFunc, uint32_t delayTime) {
	// get nibbles
	uint8_t binaryLowNibbles[4];
	uint8_t binaryHighNibbles[4];
	LCD_nibbles(binaryHighNibbles, binaryLowNibbles, command);
	
	// write instruction
	lcd->rs.port->BSRR = 1U << (lcd->rs.pin_number + 16U);
	
	// send high nibbles first
	writeNibbles(lcd, binaryHighNibbles);
	// send low nibbles
	writeNibbles(lcd, binaryLowNibbles);
	delay_us(40);
	delayFunc(delayTime);
}

// To write in data input mode the RS bit needs to be set HIGH
void LCD_writeData(LCD_TypeDef *lcd, uint8_t data, delayType delayFunc, uint32_t delayTime) {
	// get nibbles
	uint8_t binaryLowNibbles[4];
	uint8_t binaryHighNibbles[4];
	LCD_nibbles(binaryHighNibbles, binaryLowNibbles, data);
	// write data
	lcd->rs.port->BSRR = 1U << lcd->rs.pin_number;
	// send high nibbles first
	writeNibbles(lcd, binaryHighNibbles);
	// send low nibbles
	writeNibbles(lcd, binaryLowNibbles);
	delay_us(40);
	delayFunc(delayTime);
}

void LCD_writeText(LCD_TypeDef *lcd, const char *text, uint8_t length) {
	for(uint8_t i = 0; i < length; ++i) {
		LCD_writeData(lcd, *text++, delay_none, 0);
	}
}

void LCD_clearScreen(LCD_TypeDef *lcd) {
	LCD_writeCommand(lcd, 0x1, delay_ms, 2);
}

void LCD_displayControl(LCD_TypeDef *lcd, uint8_t displayOn, uint8_t cursorOn, uint8_t cursorBlinkingOn) {
	uint8_t displayControlCommand = (0x8U | displayOn << 2U | cursorOn << 1U | cursorBlinkingOn << 0U);
	LCD_writeCommand(lcd, displayControlCommand, delay_none, 0);
}

// takes in a hexadecimal value and two pointers to separate the high a low nibbles for LCD 4-bit mode
static void LCD_nibbles(uint8_t *highNibbleBuffer, uint8_t *lowNibbleBuffer, uint8_t hexValue) {
	uint8_t lowNibble = hexValue & 0xf;
	uint8_t highNibble = hexValue >> 4U;
	LCD_binaryNibble(highNibbleBuffer, highNibble, 3);
	LCD_binaryNibble(lowNibbleBuffer, lowNibble, 3);
}

static void LCD_binaryNibble(uint8_t *buffer, uint8_t nibbleValue, uint8_t index) {
	if(index == 0U){
		buffer[0] = nibbleValue % 2U;
		return;
	}
	
	buffer[index] = nibbleValue % 2U;
	LCD_binaryNibble(buffer, nibbleValue / 2U, index - 1);
}

static inline void clear_data_bit(STM_PinDef *data_pin) {
	data_pin->port->BRR = (1U << data_pin->pin_number);
}

static void write_data_bit(STM_PinDef *data_pin, uint8_t bit) {
	// using ternary to ensure we are only setting 1 to the relevant bit, and not accidentally over-shifting if nibbles[x] > 1
	data_pin->port->BSRR = bit ? (1U << data_pin->pin_number) : 0U;
}

static void writeNibbles(LCD_TypeDef *lcd, uint8_t *nibbles) {
	STM_PinDef *data_pins[4] = {&lcd->db7, &lcd->db6, &lcd->db5, &lcd->db4};
	
	for(uint8_t i = 0; i < 4; ++i) {
		clear_data_bit(data_pins[i]);
		write_data_bit(data_pins[i], nibbles[i]);
	}
	
	delay_us(1);
	LCD_sendData(&lcd->enable);
}

static void LCD_sendData(STM_PinDef *enable_pin) {
	// pulse signal to latch data
	enable_pin->port->BSRR = 1U << enable_pin->pin_number;
	delay_us(1U);
	enable_pin->port->BSRR = 1U << (enable_pin->pin_number + 16U);
}
