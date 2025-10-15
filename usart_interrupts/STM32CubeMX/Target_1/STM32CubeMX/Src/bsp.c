#include "stm32c031xx.h"
#include "bsp.h"
#include "ring_buffer.h"

#define PIN2 2U
#define PIN3 3U
#define RINGBUFFER_SIZE 128
#define NEWLINE 0x0A

void BSP_turnLED(char letter);
void BSP_usartInit(void);
void BSP_LEDsInit(void);

volatile uint8_t buffer[RINGBUFFER_SIZE];
volatile uint8_t newLine = 0U;
ringBuffer rb = {
	length: RINGBUFFER_SIZE,
	buffer: buffer,
	readPosition: 0,
	writePosition: 0
};

void BSP_Init(void) {
	// enable clock for port A
	RCC->IOPENR |= (1U << 0U);
	
	BSP_LEDsInit();
	BSP_usartInit();
}

void BSP_usartInit(void) {
	// enable clock for USART2 in APB2 register
	RCC->APBENR1 |= (1U << 17U);
	
	// set pins 2 and 3 to alternate mode
	GPIOA->MODER &= ~((3U << (PIN2 * 2U)) | (3U << (PIN3 * 2U)));
	GPIOA->MODER |= (2U << (PIN2 * 2U)) | (2U << (PIN3 * 2U));
	
	// assigning usart to pa2 and pa3 as alternate function
	GPIOA->AFR[0] &= ~((0xFU << (PIN2 * 4U)) | (0xFU << (PIN3 * 4U)));
	GPIOA->AFR[0] |= ((1U << (PIN2 * 4U)) | (1U << (PIN3 * 4U)));
	
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
	GPIOA->MODER &= ~((3U << (LED_PA9 * 2U)) | (3U << (LED_PA15 * 2U)) | (3U << (LED_PA10 * 2U)));
	GPIOA->MODER |= ((1U << (LED_PA9 * 2U)) | (1U << (LED_PA15 * 2U))  | (1U << (LED_PA10 * 2U)));
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
	GPIOA->BSRR |= 1U << LED_PA10; 
}

void BSP_turnOffGreenLED(void) {
	GPIOA->BSRR |= 1U << (LED_PA10 + 16U);
}

void BSP_turnBlueLED(void) {
	GPIOA->BSRR |= 1U << LED_PA15;
}

void BSP_turnOffBlueLED(void) {
	GPIOA->BSRR |= 1U << (LED_PA15 + 16U);
}

void BSP_turnRedLED(void) {
	GPIOA->BSRR |= 1U << LED_PA9;
}

void BSP_turnOffRedLED(void) {
	GPIOA->BSRR |= 1U << (LED_PA9 + 16U);
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