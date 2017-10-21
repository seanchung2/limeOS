#ifndef _IDT_H
#define _IDT_H

/* Define variables */
#define SYSTEM_CALL_VEC_NUM 0x80 

/* Setup a macro handler */
#define EXECPTION_HANDLER (exeception_name,msg_print)	\
void exeception_name()									\
{														\
	printf("%s\n",#msg_print); 							\
	while(1);											\
}

/* Define the handler */												
EXCEPTION_HANDLER(DE,"Divide Error!!!");
EXCEPTION_HANDLER(DB,"RESERVED!!!");
EXCEPTION_HANDLER(NMI,"Nonmaskable Interrupt Exception!!!");
EXCEPTION_HANDLER(BP,"Breakpoint Exception!!!");
EXCEPTION_HANDLER(OF,"Overflow Exception!!!");
EXCEPTION_HANDLER(BR,"BOUND Range Exceeded Exception!!!");
EXCEPTION_HANDLER(UD,"Invalid Opcode Exception!!!");
EXCEPTION_HANDLER(NM,"Device Not Available Exception!!!");
EXCEPTION_HANDLER(DF,"Double Fault Exception!!!");
EXCEPTION_HANDLER(CSO,"Coprocessor Segment Overrun Exception!!!");
EXCEPTION_HANDLER(TS,"Invalid TSS Exception!!!");
EXCEPTION_HANDLER(NP,"Segment Not Present!!!");
EXCEPTION_HANDLER(SS,"Stack-Segment Fault Exception!!!");
EXCEPTION_HANDLER(GP,"General Protection Exception!!!");
EXCEPTION_HANDLER(PF,"Page Fault Exception!!!");
EXCEPTION_HANDLER(MF,"Floating Point Error Exception!!!");
EXCEPTION_HANDLER(AC,"Alignment Check Exception!!!");
EXCEPTION_HANDLER(MC,"Machine Check Exception!!!");
EXCEPTION_HANDLER(XF,"SIMD Floating-Point Exception!!!");

/* initialize the IDT */
extern void init_idt();

#endif