#ifndef _DELAY_
#define _DELAY_

typedef void (*delayType)(uint32_t);

typedef enum {
	NOT_DELAYED,
	DELAYED
} DelayState;

typedef struct {
	uint32_t lastTimestamp;
	DelayState state;
} Delay_TypeDef;

void DelayService_init(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void delay_none(uint32_t time);
void newDelay_ms(uint32_t ms);


uint32_t getSysTickCounter(void);
uint8_t hasDelayElapsed(uint32_t timer, uint32_t delay);

#endif