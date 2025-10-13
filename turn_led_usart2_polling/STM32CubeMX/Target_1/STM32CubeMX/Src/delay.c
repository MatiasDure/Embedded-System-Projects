#include "delay.h"

void delay(unsigned int volatile iterations) {
		
	while(iterations > 0){
		iterations--;
	}
}