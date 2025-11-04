#include "stm32c031xx.h"
#include "bsp.h"
#include "ring_buffer.h"
#include "delay.h"
#include "lcd1602.h"
#include "led.h"
#include "button.h"
#include "game.h"
#include "timer.h"

#define RINGBUFFER_SIZE 128
#define NEWLINE 0x0A

void BSP_usartInit(void);
void BSP_portsInit(void);
void BSP_buttonsInit(void);
void BSP_timersInit(void);

volatile uint8_t buffer[RINGBUFFER_SIZE];
volatile uint8_t newLine = 0U;
ringBuffer rb = {
	RINGBUFFER_SIZE,
	buffer,
	0, 0
};

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
	{ {GPIOA, BUTTON_RED_PA7}, 0U, NOT_DEBOUNCING, 0U },
	{ {GPIOA, BUTTON_GREEN_PA4}, 0U, NOT_DEBOUNCING, 0U },
	{ {GPIOA, BUTTON_BLUE_PA8}, 0U, NOT_DEBOUNCING, 0U },
};

Timer_TypeDef timer = {
	TIM14,
	{ GPIOA, BUZZER_PWM_TIM1_CH1_PA7 }
};

void BSP_Init(void) {
	BSP_portsInit();
	LCD_init(&lcd);
	/*uint8_t score = 0;
	char buffer[3];
	getAsciiValue(buffer, score, 3);
	LCD_writeText(&lcd, buffer, 3);
	delay_ms(2000);
	score+= 15;
	getAsciiValue(buffer, score, 3);
	LCD_writeText(&lcd, buffer, 3);
	LCD_clearScreen(&lcd);
	LCD_displayControl(&lcd, 0,0,0);
	*/
	/*LED_LEDsInit(&led);
	LED_turnOnLED(led.blueLED);
	delay_ms(2000);
	LED_turnOffLED(led.blueLED);
	*/
	//LED_LEDsInit(&led);
	//Button_init(&buttons);
	DelayService_init();
	Timer_init(&timer);
	//BSP_timersInit();
	//BSP_usartInit();
}

void BSP_portsInit(void) {
	// enable clock for port A and B
	RCC->IOPENR |= (1U << 0U) | (1U << 1U);
	// enable APB clock for TIM14 peripheral (bit 15)
	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;
}


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

void redButtonCallback(void) {
	LED_turnOnLED(led.redLED);
	delay_ms(2000);
	//newDelay_ms(2000);
	LED_turnOffLED(led.redLED);
}

void greenButtonCallback(void) {
	LED_turnOnLED(led.greenLED);
	//delay_ms(2000);
	newDelay_ms(2000);
	LED_turnOffLED(led.greenLED);
}

void blueButtonCallback(void) {
	LED_turnOnLED(led.blueLED);
	//delay_ms(2000);
	newDelay_ms(2000);
	LED_turnOffLED(led.blueLED);
}

typedef struct {
    uint16_t frequency; // Frequency in Hz
    uint16_t duration;  // Duration in milliseconds
} Note;


void BSP_waitForCharacter(void) {
	// Used note sequence from: https://blogs.glowscotland.org.uk/sb/public/youthmusicinitiative/uploads/sites/3670/2020/09/09143506/Ipad-Bigger-Harry-Potter-Tune-Notes-Only.pdf
	Note hedwigTheme[] = {
    {494, 400}, 
    {659, 600}, 
    {784, 200},   
    {740, 400},   
    {659, 800},   
    {988, 400},   
    {880, 1000},  
    {740, 1000},  
    {659, 600},   
    {784, 200},   
    {740, 400},   
    {622, 600},   
    {698, 400},   
    {494, 1400},  
		//---------------
		{494, 400},   
    {659, 600},   
    {784, 200},   
    {740, 400},   
    {659, 800},   
    {988, 400},   
    {1175, 800},  
    {1109, 400},  
    {1047, 800},  
    {831, 400},  
    {1047, 600},  
    {988, 200}, 
    {932, 400},   
    {466, 800},   
    {784, 400},  
    {659, 1400},  
		// --------------
		{784, 400},    
    {988, 800},   
    {784, 400},   
    {988, 800},   
    {784, 400},    
    {1047, 800},   
    {988, 400},   
    {932, 800},    
    {698, 400},   
    {784, 600},   
    {988, 200},   
    {932, 400},   
    {466, 800},   
    {494, 400},    
    {988, 1400},   
		// ---------------
		{784, 400},    
    {988, 800},   
    {784, 400},   
    {988, 800},   
    {784, 400},   
    {1175, 800},  
    {1109, 400},   
    {1047, 800},   
    {831, 400},   
    {1047, 600},
    {988, 200},   
    {932, 400},  
    {466, 800},  
    {784, 400},   
    {659, 1400},   
};

// melody from: https://blogs.glowscotland.org.uk/sb/public/youthmusicinitiative/uploads/sites/3670/2020/09/09142521/Ipad-Pirates-of-the-Caribbean-Tune-Notes-Only.pdf
Note piratesTheme[] = {
		{587, 400}, // d
    {587, 600}, // d
    {587, 200},   // d 
    {659, 200},   // e - here
    {698, 400},   // f
    {698, 600},   // f
    {698, 200},  // f
    {784, 200},  // g
    {659, 400},   // e
    {659, 600},   // e
    {587, 200},   // d
    {523, 200},   // c
    {523, 200},   // c
    {587, 1000},  // d
		//---------------
		{587, 400}, // d
    {587, 600}, // d
    {587, 200},   // d 
    {659, 200},   // e - here
    {698, 400},   // f
    {698, 600},   // f
    {698, 200},  // f
    {784, 200},  // g
    {659, 400},   // e
    {659, 600},   // e
    {587, 200},   // d
    {523, 200},   // c
    {523, 800},   // c
		//---------------
		{587, 400}, // d
    {587, 600}, // d
    {587, 200},   // d 
    {698, 200},   // f
    {784, 400},   // g
    {784, 600},   // g
    {784, 200},  // g
    {880, 200},  // a
    {932, 400},   // Bb
    {932, 400},   // Bb
    {880, 200},   // A
    {784, 200},   // G
    {880, 200},   // A
		{587, 600},   // D
		//---------------
		{698, 400}, // f
    {698, 400}, // f
    {784, 400},   // g 
    {880, 200},   // a
    {587, 600},   // d
    {587, 200},   // d
    {698, 200},  // f
    {659, 400},  // e
    {659, 600},   // e
    {698, 200},   // f
    {587, 200},   // d
    {659, 1400},   // e
    //---------------
		{698, 400}, // f
    {698, 400}, // f
    {784, 400},   // g 
    {880, 200},   // a
    {587, 600},   // d
    {587, 200},   // d
    {698, 200},  // f
    {659, 400},  // e
    {659, 600},   // e
    {587, 200},   // d
    {523, 200},   // c
    {587, 1400},   // d
};
	
for (uint8_t i = 0; i < sizeof(piratesTheme)/sizeof(piratesTheme[0]); ++i) {
    Timer_updateFreq(timer.timer, piratesTheme[i].frequency, 50); // 50% duty cycle
    newDelay_ms(piratesTheme[i].duration);
}

	//LCD_writeScrollText(&lcd, &startingScroll, "Press red button to start game", 30, 450);
	//while(1) {
		//__WFI();
	//}
	//LCD_clearScreen(&lcd);
	//LCD_displayControl(&lcd, 0,0,0);
	

/*
for (uint8_t i = 0; i < sizeof(hedwigThemeRow2)/sizeof(hedwigThemeRow2[0]); ++i) {
    Timer_updateFreq(timer.timer, hedwigThemeRow2[i].frequency, 50); // 50% duty cycle
    newDelay_ms(hedwigThemeRow2[i].duration);
}

for (uint8_t i = 0; i < sizeof(hedwigThemeRow3)/sizeof(hedwigThemeRow3[0]); ++i) {
    Timer_updateFreq(timer.timer, hedwigThemeRow3[i].frequency, 50); // 50% duty cycle
    newDelay_ms(hedwigThemeRow3[i].duration);
}

for (uint8_t i = 0; i < sizeof(hedwigThemeRow4)/sizeof(hedwigThemeRow4[0]); ++i) {
    Timer_updateFreq(timer.timer, hedwigThemeRow4[i].frequency, 50); // 50% duty cycle
    newDelay_ms(hedwigThemeRow4[i].duration);
}*/




	// gameLoop(&led, &buttons, &lcd);
	//Button_readPress(&buttons.redButton, &redButtonCallback);
	//Button_readPress(&buttons.greenButton, &greenButtonCallback);
	//Button_readPress(&buttons.blueButton, &blueButtonCallback);
	
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
