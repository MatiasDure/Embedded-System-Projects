#include "bsp.h"
#include "delay.h"
#include "stm32c031xx.h"

int main(void) {
	BSP_Init();
	
	while(1) {
		BSP_Run();
	//	__WFI();
	}
	
	return 0;
}
