#include "stm32c031xx.h"
#include "bsp.h"
//#include "ring_buffer.h"
#include "delay.h"
#include "lcd1602.h"
#include "led.h"
#include "button.h"
#include "game.h"
#include "timer.h"
#include "sound.h"

//#define RINGBUFFER_SIZE 128
//#define NEWLINE 0x0A

void BSP_usartInit(void);
void BSP_portsInit(void);

//volatile uint8_t buffer[RINGBUFFER_SIZE];
//volatile uint8_t newLine = 0U;
/*ringBuffer rb = {
	RINGBUFFER_SIZE,
	buffer,
	0, 0
};*/

LCD_TypeDef lcd = {
	{ GPIOB, LCD_REGISTER_SELECT_PB0 },
	{ GPIOB, LCD_ENABLE_SIGNAL_PB1 },
	{ GPIOB, LCD_DB4_PB2 },
	{ GPIOB, LCD_DB5_PB3 },
	{ GPIOB, LCD_DB6_PB4 },
	{ GPIOB, LCD_DB7_PB5 }
};

LED_TypeDef led = {
	{ GPIOA, LED_BLUE_PA15 },
	{ GPIOA, LED_RED_PA6 },
	{ GPIOA, LED_GREEN_PA5 },
};

Button_TypeDef buttons = {
	{ {GPIOA, BUTTON_RED_PA0}, 0U, NOT_DEBOUNCING, 0U },
	{ {GPIOA, BUTTON_GREEN_PA4}, 0U, NOT_DEBOUNCING, 0U },
	{ {GPIOA, BUTTON_BLUE_PA8}, 0U, NOT_DEBOUNCING, 0U },
};

Timer_TypeDef timer = {
	TIM14,
	{ GPIOA, BUZZER_PWM_TIM1_CH1_PA7 }
};

void BSP_Init(void) {
	BSP_portsInit();
	DelayService_init();
	LCD_init(&lcd);
	LED_LEDsInit(&led);
	Button_init(&buttons);
	Timer_init(&timer);
	//BSP_usartInit();
}

void BSP_portsInit(void) {
	// enable clock for port A and B
	RCC->IOPENR |= (1U << 0U) | (1U << 1U);
	// enable APB clock for TIM14 peripheral (bit 15)
	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;
}
/*
void BSP_usartInit(void) {
	// enable clock for USART2 in APB2 register
	RCC->APBENR1 |= RCC_APBENR1_USART2EN;
	
	// set pins 2 and 3 to alternate mode
	GPIOA->MODER &= ~((3U << (USART2_TX_PA2 * 2U)) | (3U << (USART2_RX_PA3 * 2U)));
	GPIOA->MODER |= (2U << (USART2_TX_PA2 * 2U)) | (2U << (USART2_RX_PA3 * 2U));
	
	// assigning pa2 and pa3 as alternate function (usart)
	GPIOA->AFR[0] &= ~((0xFU << (USART2_TX_PA2 * 4U)) | (0xFU << (USART2_RX_PA3 * 4U)));
	GPIOA->AFR[0] |= ((1U << (USART2_TX_PA2 * 4U)) | (1U << (USART2_RX_PA3 * 4U)));
	
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
*/
void BSP_waitForCharacter(void) {
	 gameLoop(&led, &buttons, &lcd, timer.timer);
	
	/*while(!newLine) __WFI();
	
	while(rb.readPosition != rb.writePosition) {
		//BSP_turnLED(ringBuffer_read(&rb));
		
		//random delay
		uint32_t counter = 500000U; 
		while(counter > 0) counter--;
	}
	
	newLine = 0U;*/
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
/*
void USART2_IRQHandler(void) {
	if((USART2->ISR & (1U << 5U))) {
		uint8_t c = USART2->RDR;
		
		if(c == NEWLINE) {
			newLine = 1;
			return;
		}
		ringBuffer_write(&rb, c);
	}
}*/
