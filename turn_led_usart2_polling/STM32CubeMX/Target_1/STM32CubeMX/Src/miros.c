#include <stdint.h>

#include "miros.h"
#include "cmsis_armclang.h"
#include "qassert.h"

Q_DEFINE_THIS_FILE

static inline uint32_t align_down(uint32_t address, uint32_t align);
static inline uint32_t align_up(uint32_t address, uint32_t align);
static inline uint32_t highestPriority(uint32_t readyMask);

OSThread * volatile OS_current; // pointer to current thread (current background proccess)
OSThread * volatile OS_next; // pointer to next thread (next background proccess)

OSThread *OS_threads[32 + 1]; // array of threads started
uint32_t OS_readySet; // bitmask of threads that are ready to run
uint32_t OS_delayedSet; // bitmask of threads that are blocked

//#define GET_HIGHEST_PRIORITY(x) (32U - _clz(x))

OSThread idleThread;
void main_idleThread(void) {
	while(1) {
		OS_onIdle(); // callback when cpu moves to idle thread
	}
}

void OS_init(void *idleStackMemory, uint32_t idleStackSize) {
		OSThread_start(
		&idleThread,
		0,
		&main_idleThread,
		idleStackMemory, idleStackSize);
	// set the PendSV interrupt priority to the lowest level 
	// ex: (1111 1111 << 16). This is to cover all possible bits in different mcu.
	// In the case of our stm32c031c6 mcu, only the two most significant bits are used 
	// to set priority (1100 0000) = 0xC0
	*(uint32_t volatile *)0xE000ED20 |= 0xFF << 16;
}

void OS_run(void) {
	// callback function to configure and start interrupts (defined in application, not in RTOS file)
	OS_onStartup();
	
	__disable_irq();
	OS_schedule();
	__enable_irq();
	
	// control should never return here because control is given to other threads containing infinite loops
	Q_ERROR();
}

void OS_schedule(void) {
	if(OS_readySet == 0U) OS_next = OS_threads[0];
	else {
		uint8_t index = highestPriority(OS_readySet);
		OS_next = OS_threads[index];
	}
	
	// trigger pendSV to allow context switch
	if(OS_next != OS_current) {
		*(uint32_t volatile *)0xE000ED04 |= (1U << 28);
	}
}

void OSThread_start(
	OSThread *me,
	uint8_t priority,
	OSThreadHandler threadHandler,
	void *stackMemory, uint32_t stackSize) {
		// ensure that priority is within range of size of os_threads and that it is not already used
		Q_REQUIRE(priority < Q_DIM(OS_threads) 
			&& (OS_threads[priority] == (OSThread *)0));
		
		/* 
		on arm cortex M cpu, stack grows from higher to lower addresses, 
		so we need to start the pointer at the end of the stack memory 
		(i.e stack memory address + stack size)
		*/
		uint32_t topStack = (uint32_t)stackMemory + stackSize ;
		uint32_t *sp = (uint32_t *)align_down(topStack, 8);
		
		*(--sp) = (1U << 24); // setting the thumb state of the program status register (xPSR)
		*(--sp) = (uint32_t)threadHandler; //Pointer counter (PC) containing the memory address of the background function to switch context to
		*(--sp) = 0x0000000EU; // (LR) R14
		*(--sp) = 0x0000000CU; // R12
		*(--sp) = 0x00000003U; // R3
		*(--sp) = 0x00000002U; // R2
		*(--sp) = 0x00000001U; // R1
		*(--sp) = 0x00000000U; // R0
		// additional fake registers r4-r11
		*(--sp) = 0x0000000BU; // R11
		*(--sp) = 0x0000000AU; // R10
		*(--sp) = 0x00000009U; // R9
		*(--sp) = 0x00000008U; // R8
		*(--sp) = 0x00000007U; // R7
		*(--sp) = 0x00000006U; // R6
		*(--sp) = 0x00000005U; // R5
		*(--sp) = 0x00000004U; // R4
		
		me->sp = sp;
		
		uint32_t *alignedStackLimit = (uint32_t *)align_up((uint32_t)stackMemory, 8);
		
		while(sp > alignedStackLimit) {
			*(--sp) = 0xDEADBEEFU;
		}
		
		// register thread with the OS
		OS_threads[priority] = me;
		me->priority = priority;
		
		// make threads ready to run, except for idle thread
		if(priority > 0U) OS_readySet |= (1U << (priority - 1U));
		
}
	
void OS_delay(uint32_t ticks) {
	__disable_irq();
	
	// the idle thread (threads[0]) is not allowed to block
	Q_REQUIRE(OS_current != OS_threads[0]); 
	
	// setting timeout of thread to block (current thread running)
	OS_current->timeoutCounter = ticks;
	
	uint32_t bit = (1U << (OS_current->priority - 1U));
	//updating thread as not ready to run
	OS_readySet &= ~bit;
	//adding thread to delayed set 
	OS_delayedSet |= bit;
	// moving context away from blocked thread
	OS_schedule(); 
	__enable_irq();
}

void OS_tick(void) {
	uint32_t workingSet = OS_delayedSet;
	
	while(workingSet != 0U) {
		OSThread *thread = OS_threads[highestPriority(workingSet)];
		Q_ASSERT((thread != (OSThread *)0) && (thread->timeoutCounter != 0U));
		
		uint32_t bit = (1U << (thread->priority - 1U));
		--thread->timeoutCounter;
		if(thread->timeoutCounter == 0U) {
			OS_delayedSet &= ~bit;
			OS_readySet |= bit;
		}
		
		workingSet &= ~bit;
	}
}
	
/* inline assembly syntax for Compiler 6 (ARMCLANG) */
__attribute__ ((naked))
void PendSV_Handler(void) {
__asm volatile (
    /* __disable_irq(); */
    "  CPSID         I                 \n"

    /* if (OS_curr != (OSThread *)0) { */
    "  LDR           r1,=OS_current       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  CMP           r1,#0             \n"
    "  BEQ           context_restore    \n"

    /*     push registers r4-r11 on the stack */
#if (__ARM_ARCH == 6)               // if ARMv6-M...
    "  SUB           sp,sp,#(8*4)     \n" // make room for 8 registers r4-r11
    "  MOV           r0,sp            \n" // r0 := temporary stack pointer
    "  STMIA         r0!,{r4-r7}      \n" // save the low registers
    "  MOV           r4,r8            \n" // move the high registers to low registers...
    "  MOV           r5,r9            \n"
    "  MOV           r6,r10           \n"
    "  MOV           r7,r11           \n"
    "  STMIA         r0!,{r4-r7}      \n" // save the high registers
#else                               // ARMv7-M or higher
    "  PUSH          {r4-r11}          \n"
#endif                              // ARMv7-M or higher

    /*     OS_curr->sp = sp; */
    "  LDR           r1,=OS_current       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  MOV           r0,sp             \n"
    "  STR           r0,[r1,#0x00]     \n"
    /* } */

    "context_restore:                   \n"
    /* sp = OS_next->sp; */
    "  LDR           r1,=OS_next       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  LDR           r0,[r1,#0x00]     \n"
    "  MOV           sp,r0             \n"

    /* OS_curr = OS_next; */
    "  LDR           r1,=OS_next       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  LDR           r2,=OS_current       \n"
    "  STR           r1,[r2,#0x00]     \n"

    /* pop registers r4-r11 */
#if (__ARM_ARCH == 6)               // if ARMv6-M...
    "  MOV           r0,sp             \n" // r0 := top of stack
    "  MOV           r2,r0             \n"
    "  ADDS          r2,r2,#(4*4)      \n" // point r2 to the 4 high registers r7-r11
    "  LDMIA         r2!,{r4-r7}       \n" // pop the 4 high registers into low registers
    "  MOV           r8,r4             \n" // move low registers into high registers
    "  MOV           r9,r5             \n"
    "  MOV           r10,r6            \n"
    "  MOV           r11,r7            \n"
    "  LDMIA         r0!,{r4-r7}       \n" // pop the low registers
    "  ADD           sp,sp,#(8*4)      \n" // remove 8 registers from the stack
#else                               // ARMv7-M or higher
    "  POP           {r4-r11}          \n"
#endif                              // ARMv7-M or higher

    /* __enable_irq(); */
    "  CPSIE         I                 \n"

    /* return to the next thread */
    "  BX            lr                \n"
    );
}

static inline uint32_t align_down(uint32_t address, uint32_t align) {
	/*
		clearing the least significant n bits aligns to 2^n.
		ex: to align the address to multiple of 8:
		2^3 = 8, so we clear the 3 LSB: 1101 (13) -> 1000 (8)
	*/
	return address & ~(align - 1U);
}

static inline uint32_t align_up(uint32_t address, uint32_t align) {
	/*
		clearing the least significant n bits aligns to 2^n.
		ex: to align the address to multiple of 8 and rounding it up:
		address = A (1010) and align = 8 (1000)
		(1010 + 0111 <- 7) = 10001 = (17)
		10001 & ~(0111 -> 8-1U) = 10000 (16) (we rounded up A (10) to 10000 (16))
	*/
	return (address + (align - 1)) & ~(align - 1U);
}

static inline uint32_t highestPriority(uint32_t readyMask) {
	uint32_t temp_priority = 31U;
	while((readyMask & (1U << temp_priority)) == 0U) {
		--temp_priority;
	}
	
	return temp_priority + 1;
}