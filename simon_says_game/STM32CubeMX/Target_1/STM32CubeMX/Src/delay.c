#include <stdint.h>
#include "delay.h"

void delay_ms(uint32_t ms) {
	for(uint32_t i = 0; i < ms; ++i) {
		// clock speed is 12MHz, so for 1 ms, we want to waste 12000 cycles 12000/12000000 = 0.001s = 1ms, 
		// the for loop takes around 3-5 cycles depending on the compiler optimization, so we divide 12000 by 3 for a close estimate
		for(volatile uint16_t j = 0; j < 4000U; ++j) {}
	}
}

void delay_us(uint32_t us){
	for(uint32_t i = 0; i < us; ++i) {
		// clock speed is 12MHz, so for 1 us, we want to waste 12 cycles 12/12000000 = 0.000001s = 1us. 
		// In this case it doesn't matter as much because it's a very small delay
		for(volatile uint8_t j = 0; j < 12U; ++j) {}
	}
}
