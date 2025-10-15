.syntax unified
.thumb
.thumb_func
	
.global PendSV_Handler
.type PendSV_Handler, %function

PendSV_Handler:
	CPSID    I
	//if(OS_current != (OSThread *) 0) {
	LDR      r0,=OS_current
	LDR      r0,[r0]
	CMP      r0,#0x00
	BEQ      restore_context

	PUSH 		 {r4-r11}
	//OS_current->sp = sp;
	STR      sp,[r0]
	B        restore_context
	
restore_context
    LDR     r0,=OS_next
    LDR     r1,[r0]           
    LDR     sp,[r1]          
    LDR     r0,=OS_current
    STR     r1,[r0]           
    POP     {r4-r11}          
	
	CPSIE    I
	BX			 lr