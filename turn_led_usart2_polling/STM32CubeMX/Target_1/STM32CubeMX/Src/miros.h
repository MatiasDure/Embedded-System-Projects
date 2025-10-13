#ifndef __MIROS_H__
#define __MIROS_H__

// Thread Control Block (TCB)
typedef struct {
	void *sp; // stack pointer
	uint32_t timeoutCounter; //delay counter
	uint8_t priority; // thread priority
} OSThread;

typedef void (*OSThreadHandler)();
void OS_init(void *idleStackMemory, uint32_t idleStackSize);

void OS_schedule(void);
void OS_run(void);
void OS_tick(void);
void OS_delay(uint32_t ticks);

// callbacks
void OS_onStartup(void);
void OS_onIdle(void);

void OSThread_start(
	OSThread *me,
	uint8_t priority,
	OSThreadHandler threadHandler,
	void *stackMemory, uint32_t stackSize);

#endif //__MIROS_H__