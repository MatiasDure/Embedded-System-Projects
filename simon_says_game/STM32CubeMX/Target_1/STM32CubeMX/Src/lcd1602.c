#include "lcd1602.h"

#define CHAR_LIMIT 16U

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
	delay_us(43);
	delayFunc(delayTime);
}

void LCD_writeText(LCD_TypeDef *lcd, const char *text, uint8_t length) {
	for(uint8_t i = 0; i < length; ++i) {
		LCD_writeData(lcd, *text++, delay_none, 0);
	}
}

static void fillWithSpaces(LCD_TypeDef *lcd, uint8_t spaces) {
	for(uint8_t i = 0; i < spaces; ++i) {
		LCD_writeData(lcd, 0x20U, delay_none, 0);
	}
}

static void scrollOut(LCD_TypeDef *lcd, const char *text, uint8_t charactersToPrint, uint8_t startingIndex) {
	const char *pointer = text + startingIndex;
	if(charactersToPrint >= CHAR_LIMIT) {
			LCD_writeText(lcd, pointer, CHAR_LIMIT);
	} else {
			LCD_writeText(lcd, pointer, charactersToPrint);
			fillWithSpaces(lcd, CHAR_LIMIT - charactersToPrint);
	}
	// reset AC to address 0x0
	LCD_writeCommand(lcd, 0x80, delay_us, 40);
}

static void scrollIn(LCD_TypeDef *lcd, const char *text, uint8_t startingAddres) {
	LCD_writeCommand(lcd, 0x80 + startingAddres, delay_us, 40);
	// print n characters, where n is 0xF - startAddress + 1. 0xF = highest memory address location
	LCD_writeText(lcd, text, 0xF - startingAddres + 1);
}

static void resetScroll(Scroll_TypeDef *scroll) {
	scroll->startIndex = 0U;
	scroll->startAddress = 0xFU;
}

void LCD_writeScrollText(LCD_TypeDef *lcd, Scroll_TypeDef *scroll, const char *text, uint8_t length, uint16_t interval) {
	if(!hasDelayElapsed(scroll->lastTickStored, interval)) return;
	// we don't need to scroll since everything can be shown
	if(length < (CHAR_LIMIT + 1U)) {
		// implement scrolling for any length
		//LCD_writeText(lcd, text, length);
		return;
	}
	
	switch(scroll->state) {
		case INACTIVE:
			break;
		case SCROLL_IN:
			scrollIn(lcd, text, scroll->startAddress--);
			scroll->lastTickStored = getSysTickCounter();
			//newDelay_ms(interval);
		
			if(scroll->startAddress == 0x0U) scroll->state = SCROLL_OUT; 
			break;
		case SCROLL_OUT:
			scrollOut(lcd, text, length - scroll->startIndex, scroll->startIndex++);
			scroll->lastTickStored = getSysTickCounter();	
			//newDelay_ms(interval);
		
			if(scroll->startIndex > length) scroll->state = RESETTING;
			break;
		case RESETTING:
			resetScroll(scroll);
			LCD_writeCommand(lcd, 0x8F, delay_us, 40);
			scroll->state = INACTIVE;
		default:
			break;
	}
}

void LCD_writeCharacter(LCD_TypeDef *lcd, const char character) {
	LCD_writeData(lcd, character, delay_none, 0);
}

void LCD_clearScreen(LCD_TypeDef *lcd) {
	LCD_writeCommand(lcd, 0x1, delay_ms, 2);
}

void LCD_displayControl(LCD_TypeDef *lcd, uint8_t displayOn, uint8_t cursorOn, uint8_t cursorBlinkingOn) {
	uint8_t displayControlCommand = (0x8U | displayOn << 2U | cursorOn << 1U | cursorBlinkingOn << 0U);
	LCD_writeCommand(lcd, displayControlCommand, delay_none, 0);
}

void LCD_placeCursorAt(LCD_TypeDef *lcd, uint8_t address) {
	LCD_writeCommand(lcd, 0x80 + address, &delay_none, 0); 
}

static void getAsciiValue(char *buffer, uint32_t number, uint8_t length) {
	// start from the end to reverse order
	char *ptr = buffer + length - 1;
	while(number > 0) {
		uint8_t res = number % 10;
		// hex ascii value for 0 is 0x30 and goes up to 0x39 for 9
		*ptr-- = res + '0';
		--length;
		number /= 10;
	}
	
	while(length--) {
		//set leftovers to character 0
		*ptr-- = '0';
	}
}

static uint8_t getBufferSize(uint32_t number) {
	uint8_t bufferSize = 0;
	do {
		number /= 10;
		++bufferSize;
	} while(number);
	
	return bufferSize;
}

void LCD_writeNumber(LCD_TypeDef *lcd, uint32_t number) {
	uint8_t bufferSize = getBufferSize(number);
	if(bufferSize < 2) {
		LCD_writeCharacter(lcd, (number + '0'));
		return;
	}
	char buffer[bufferSize];
	getAsciiValue(buffer, number, bufferSize);
	LCD_writeText(lcd, buffer, bufferSize);
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
