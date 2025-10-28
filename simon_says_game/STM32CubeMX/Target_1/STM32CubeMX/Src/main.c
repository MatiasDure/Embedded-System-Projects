#include "bsp.h"
#include "delay.h"

int main(void) {
	BSP_Init();
	
	while(1) {
	BSP_waitForCharacter();
	}
	
	return 0;
}
