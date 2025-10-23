#include <stdint.h>
#include "delay.h"
/* 
	Update delay with systick implementation for more accurate delays. 
	Currently it works, but delays are not accurate when updating optimization levels (makes sense)
*/
void delay_ms(uint32_t ms) {
	for(uint32_t i = 0; i < ms; ++i) {
		// clock speed is 12MHz, so for 1 ms, we want to waste 12000 cycles 12000/12000000 = 0.001s = 1ms, 
		// the for loop takes different amount of cycles depending on the compiler optimization, so we divide 12000 by 12 for a close estimate
		for(volatile uint16_t j = 0; j < 1000U; ++j) {}
	}
}

void delay_us(uint32_t us){
	for(uint32_t i = 0; i < us; ++i) {
		// clock speed is 12MHz, so for 1 us, we want to waste 12 cycles 12/12000000 = 0.000001s = 1us. 
		// In this case it doesn't matter as much because it's a very small delay
		for(volatile uint8_t j = 0; j < 12U; ++j) {}
	}
}

// dummy function to avoid wasting for loop cycles in delay implementations
void delay_none(uint32_t time){}