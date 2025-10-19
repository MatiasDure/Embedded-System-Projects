#include "bsp.h"

int main(void) {
	
	BSP_Init();
	
	while(1) {
		BSP_waitForCharacter();
	}
	
	return 0;
}

