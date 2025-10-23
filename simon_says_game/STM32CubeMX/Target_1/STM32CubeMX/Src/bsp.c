#include "stm32c031xx.h"
#include "bsp.h"
#include "ring_buffer.h"
#include "delay.h"

#define USART2_TX 2U
#define USART2_RX 3U
#define RINGBUFFER_SIZE 128
#define NEWLINE 0x0A

void BSP_turnLED(char letter);
void BSP_usartInit(void);
void BSP_LEDsInit(void);
void BSP_portsInit(void);
void BSP_buttonsInit(void);
void BSP_timersInit(void);
void BSP_lcdInit(void);
void LCD_nibbles(uint8_t *highNibble, uint8_t *lowNibble, uint8_t hexValue);
void LCD_binaryNibble(uint8_t *buffer, uint8_t nibbleValue, uint8_t index);
void writeNibbles(uint8_t * nibbles);
void LCD_sendData(void);

volatile uint8_t buffer[RINGBUFFER_SIZE];
volatile uint8_t newLine = 0U;
ringBuffer rb = {
	length: RINGBUFFER_SIZE,
	buffer: buffer,
	readPosition: 0,
	writePosition: 0
};

void BSP_Init(void) {
	
	BSP_portsInit();
	BSP_lcdInit();
	//BSP_LEDsInit();
	//BSP_timersInit();
	//BSP_usartInit();
}

void BSP_portsInit(void) {
	// enable clock for port A and B
	RCC->IOPENR |= (1U << 0U) | (1U << 1U);
}

void BSP_buttonsInit(void) {
	// set button pins to input with pull-down
	GPIOA->MODER &= ~((3U << (BUTTON_GREEN_PA4 * 2U)) | (3U << (BUTTON_RED_PA7 * 2U)) | (3U << (BUTTON_BLUE_PA8 * 2U)));
	GPIOA->PUPDR &= ~((3U << (BUTTON_GREEN_PA4 * 2U)) | (3U << (BUTTON_RED_PA7 * 2U)) | (3U << (BUTTON_BLUE_PA8 * 2U)));
	GPIOA->PUPDR |= (2U << (BUTTON_GREEN_PA4 * 2U)) | (2U << (BUTTON_RED_PA7 * 2U)) | (2U << (BUTTON_BLUE_PA8 * 2U));
}

void BSP_timersInit(void) {
	// enable APB TIM1 peripheral clock (bit 11)
	RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
	
	// set buzzer pin as TIM alternate function to use PWM output signals
	GPIOA->MODER &= ~((3U << (BUZZER_PWM_TIM1_CH1_PA0 * 2U)));
	GPIOA->MODER |= (2U << (BUZZER_PWM_TIM1_CH1_PA0 * 2U));
	
	/* 
		Check Table 13: Port A alternate function mapping (AF0 to AF7) for the value 
		of the tim1_ch1 alternate function.
		PA0-PA7 are part of the low registers AFR[0].
	*/
	GPIOA->AFR[0] &= ~((0xFU << (BUZZER_PWM_TIM1_CH1_PA0 * 4U)));
	GPIOA->AFR[0] |= (5U << (BUZZER_PWM_TIM1_CH1_PA0 * 4U));
	
	// set prescaler to 12 to set clock speed to 1MHz (12MHz / 12 = 1MHz)
	TIM1->PSC = 12U;
	// Set counter top value (auto-reload) 
	// Timer counts from 0 (1.000.000 hz / 500 = 2kHz frequency for the PWM)
	TIM1->ARR = 499U;
	
	// duty cycle set to 50% of auto-reload value to stay HIGH half the duration of the timer count
	TIM1->CCR1 = 250;
	
	// Configure PWM mode
	TIM1->CCMR1 &= ~(7U << 4U);
	TIM1->CCMR1 |= (6U << 4U); 
	TIM1->CCMR1 |= (1U << 3U);     

	// Enable CH1 output
	TIM1->CCER |= (1U << 0U);
	
	// Main output enable
	TIM1->BDTR |= (1U << 15U);
	
	// Enable counter
	TIM1->CR1 |= (1U << 0U);

	// Reinitialize the counter and generate an update of the registers
	TIM1->EGR |= (1U << 0U);
}

void BSP_lcdInit(void) {
	// setting lcd pins as output and no pull-up/pull-down resistors
	GPIOB->MODER &= ~((3U << (LCD_REGISTER_SELECT_PB0 * 2U)) |
										(3U << (LCD_ENABLE_SIGNAL_PB1 * 2U)) |
										(3U << (LCD_DB4_PB2 * 2U)) |
										(3U << (LCD_DB5_PB3 * 2U)) |
										(3U << (LCD_DB6_PB4 * 2U)) |
										(3U << (LCD_DB7_PB5 * 2U)));
	GPIOB->MODER |= ((1U << (LCD_REGISTER_SELECT_PB0 * 2U)) |
										(1U << (LCD_ENABLE_SIGNAL_PB1 * 2U)) |
										(1U << (LCD_DB4_PB2 * 2U)) |
										(1U << (LCD_DB5_PB3 * 2U)) |
										(1U << (LCD_DB6_PB4 * 2U)) |
										(1U << (LCD_DB7_PB5 * 2U)));
	GPIOB->PUPDR &= ~((3U << (LCD_REGISTER_SELECT_PB0 * 2U)) |
										(3U << (LCD_ENABLE_SIGNAL_PB1 * 2U)) |
										(3U << (LCD_DB4_PB2 * 2U)) |
										(3U << (LCD_DB5_PB3 * 2U)) |
										(3U << (LCD_DB6_PB4 * 2U)) |
										(3U << (LCD_DB7_PB5 * 2U)));
	
	/*
		* Initialization sequence to wake up lcd
		* Information about the initialization sequence can be found in the 
		LCD-1602A datasheet: https://www.alldatasheet.com/datasheet-pdf/pdf/1574132/CRYSTAIFONTZ/LCD-1602A.html
	*/
	delay_ms(16);
	// knocking door to force lcd to wake up in 8-bit mode
	BSP_LCD_WriteCommand(0x30, &delay_ms, 5);
	BSP_LCD_WriteCommand(0x30, &delay_us, 150);
	BSP_LCD_WriteCommand(0x30, &delay_us, 1);
	// set interface to 4-bit mode
	BSP_LCD_WriteCommand(0x20, &delay_none, 0);
	// specify display lines and character font
	BSP_LCD_WriteCommand(0x28, &delay_none, 0);
	// display off
	BSP_LCD_WriteCommand(0x8, &delay_none, 0);
	// display clear
	BSP_LCD_WriteCommand(0x1, &delay_ms, 2);
	// Entry mode set
	BSP_LCD_WriteCommand(0x6, &delay_none, 0);
	
	// Set DDRAM address to 0
	BSP_LCD_WriteCommand(0x80, &delay_none, 0); 
	// turn on display
	BSP_LCD_WriteCommand(0x0C, delay_us, 40);
	// writing chars to tests
	BSP_LCD_WriteData('H', &delay_none, 0);
	BSP_LCD_WriteData('e', &delay_none, 0);
	BSP_LCD_WriteData('l', &delay_none, 0);
	BSP_LCD_WriteData('l', &delay_none, 0);
	BSP_LCD_WriteData('o', &delay_none, 0);
	// sending h as hex value
	BSP_LCD_WriteData(0x68, &delay_none, 0);
	// sending h as decimal
	BSP_LCD_WriteData(104, &delay_none, 0);
	// sending h as binary
	BSP_LCD_WriteData(0b01101000, &delay_none, 0);
	BSP_LCD_WriteData('!', &delay_none, 0);
	delay_ms(5000);
	// clear display
	BSP_LCD_WriteCommand(0x1, delay_ms, 2);
	// turn off display
	BSP_LCD_WriteCommand(0x8, delay_us, 40);
}

void BSP_usartInit(void) {
	// enable clock for USART2 in APB2 register
	RCC->APBENR1 |= RCC_APBENR1_USART2EN;
	
	// set pins 2 and 3 to alternate mode
	GPIOA->MODER &= ~((3U << (USART2_TX * 2U)) | (3U << (USART2_RX * 2U)));
	GPIOA->MODER |= (2U << (USART2_TX * 2U)) | (2U << (USART2_RX * 2U));
	
	// assigning pa2 and pa3 as alternate function (usart)
	GPIOA->AFR[0] &= ~((0xFU << (USART2_TX * 4U)) | (0xFU << (USART2_RX * 4U)));
	GPIOA->AFR[0] |= ((1U << (USART2_TX * 4U)) | (1U << (USART2_RX * 4U)));
	
	// Disable USART UE until its configured
	USART2->CR1 &= ~(1U << 0U);
	
	//setting baud rate  12 MHz with baud rate 115200  12 MHz / 115200 = 104.7
	USART2->BRR = 104U;
	
	// setting word length to 1 start bit, 8 data bit, n stop bit i.e "00"
	USART2->CR1 &= ~((1U << 12U) | (1U << 28U));
	// enabling IRS for Rx and Rx
	USART2->CR1 |= ((1U << 2U) | (1U << 5U));
	
	// enabling USART UE
	USART2->CR1 |= (1U << 0U);
	
	// enable interrupts for usart2
	NVIC->ISER[0] |= (1U << USART2_IRQn);
}

void BSP_LEDsInit(void) {
	// set led pins to output
	GPIOA->MODER &= ~((3U << (LED_GREEN_PA5 * 2U)) | (3U << (LED_RED_PA6 * 2U)) | (3U << (LED_BLUE_PA15 * 2U)));
	GPIOA->MODER |= ((1U << (LED_GREEN_PA5 * 2U)) | (1U << (LED_RED_PA6 * 2U))  | (1U << (LED_BLUE_PA15 * 2U)));
}

void BSP_waitForCharacter(void) {
	while(!newLine) __WFI();
	
	while(rb.readPosition != rb.writePosition) {
		BSP_turnLED(ringBuffer_read(&rb));
		uint32_t counter = 500000U;
		while(counter > 0) counter--;
	}
	
	newLine = 0U;
	// Wait until REXNE bit is set to indicate that the content of the shift register
	// is transfered to the RDR
	// polling approach replaced with IRS
	/*
	while(((USART2->ISR & (1U << 5U)) == 0U)) {}
	uint8_t c = USART2->RDR;
	BSP_turnLED(c);
	if((USART2->ISR & (1U << 3U)) != 0U) {
		USART2->ICR = 1U << 3U;
	}		
	*/
}

void BSP_turnLED(char letter) {
	switch(letter) {
		case 'g':
			BSP_turnGreenLED();
			BSP_turnOffBlueLED();
			BSP_turnOffRedLED();
			break;
		case 'r':
			BSP_turnRedLED();
			BSP_turnOffGreenLED();
			BSP_turnOffBlueLED();
			break;
		case 'b':
			BSP_turnBlueLED();
			BSP_turnOffGreenLED();
			BSP_turnOffRedLED();
			break;
		case 'o':
			BSP_turnOffBlueLED();
			BSP_turnOffGreenLED();
			BSP_turnOffRedLED();
		default:
			// ignore other characters received for now
			break;
	}
}

void BSP_turnGreenLED(void) {
	GPIOA->BSRR |= 1U << LED_GREEN_PA5; 
}

void BSP_turnOffGreenLED(void) {
	GPIOA->BSRR |= 1U << (LED_GREEN_PA5 + 16U);
}

void BSP_turnBlueLED(void) {
	GPIOA->BSRR |= 1U << LED_BLUE_PA15;
}

void BSP_turnOffBlueLED(void) {
	GPIOA->BSRR |= 1U << (LED_BLUE_PA15 + 16U);
}

void BSP_turnRedLED(void) {
	GPIOA->BSRR |= 1U << LED_RED_PA6;
}

void BSP_turnOffRedLED(void) {
	GPIOA->BSRR |= 1U << (LED_RED_PA6 + 16U);
}

void USART2_IRQHandler(void) {
	if((USART2->ISR & (1U << 5U))) {
		//BSP_turnLED(c);
		uint8_t c = USART2->RDR;
		
		if(c == NEWLINE) {
			newLine = 1;
			return;
		}
		ringBuffer_write(&rb, c);
	}
}

// To write in instruction input mode the RS bit needs to be set LOW
void BSP_LCD_WriteCommand(uint8_t command, delayType delayFunc, uint32_t delayTime) {
	// get nibbles
	uint8_t binaryLowNibbles[4];
	uint8_t binaryHighNibbles[4];
	LCD_nibbles(binaryHighNibbles, binaryLowNibbles, command);
	
	// write instruction
	GPIOB->BSRR = 1U << (LCD_REGISTER_SELECT_PB0 + 16U);
	// send high nibbles first
	writeNibbles(binaryHighNibbles);
	// send low nibbles
	writeNibbles(binaryLowNibbles);
	delay_us(40);
	delayFunc(delayTime);
}

// To write in data input mode the RS bit needs to be set HIGH
void BSP_LCD_WriteData(uint8_t data, delayType delayFunc, uint32_t delayTime) {
	// get nibbles
	uint8_t binaryLowNibbles[4];
	uint8_t binaryHighNibbles[4];
	LCD_nibbles(binaryHighNibbles, binaryLowNibbles, data);
	// write data
	GPIOB->BSRR = 1U << (LCD_REGISTER_SELECT_PB0);
	// send high nibbles first
	writeNibbles(binaryHighNibbles);
	// send low nibbles
	writeNibbles(binaryLowNibbles);
	delay_us(40);
	delayFunc(delayTime);
}

void writeNibbles(uint8_t *nibbles) {
	GPIOB->BRR = (1U << LCD_DB7_PB5) |
								(1U << LCD_DB6_PB4) |
								(1U << LCD_DB5_PB3) |
								(1U << LCD_DB4_PB2);
	// using ternary to ensure we are only setting 1 to the relevant bit, and not accidentally over-shifting if nibbles[x] > 1
	GPIOB->BSRR = (nibbles[0] ? (1U << LCD_DB7_PB5) : 0) |
								(nibbles[1] ? (1U << LCD_DB6_PB4) : 0) |
								(nibbles[2] ? (1U << LCD_DB5_PB3) : 0) |
								(nibbles[3] ? (1U << LCD_DB4_PB2) : 0);
	delay_us(1);
	LCD_sendData();
}

void LCD_sendData(void) {
	// pulse enable
	GPIOB->BSRR = 1U << LCD_ENABLE_SIGNAL_PB1;
	delay_us(1U);
	GPIOB->BSRR = 1U << (LCD_ENABLE_SIGNAL_PB1 + 16U);
}

// takes in a hexadecimal value and two pointers to separate the high a low nibbles for LCD 4-bit mode
void LCD_nibbles(uint8_t *highNibbleBuffer, uint8_t *lowNibbleBuffer, uint8_t hexValue) {
	uint8_t lowNibble = hexValue & 0xf;
	uint8_t highNibble = hexValue >> 4U;
	LCD_binaryNibble(highNibbleBuffer, highNibble, 3);
	LCD_binaryNibble(lowNibbleBuffer, lowNibble, 3);
}

void LCD_binaryNibble(uint8_t *buffer, uint8_t nibbleValue, uint8_t index) {
	if(index == 0U){
		buffer[0] = nibbleValue % 2U;
		return;
	}
	
	buffer[index] = nibbleValue % 2U;
	LCD_binaryNibble(buffer, nibbleValue / 2U, index - 1);
}

