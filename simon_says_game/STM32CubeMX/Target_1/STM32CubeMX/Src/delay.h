#ifndef _DELAY_
#define _DELAY_

typedef void (*delayType)(uint32_t);

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void delay_none(uint32_t time);

#endif