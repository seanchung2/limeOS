
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

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


void init_idt()
{
	int i;
	lidt(idt_desc_ptr);
	
	for (i=0;i<NUM_VEC;i++)
	{
		idt[i].present = 0x1;				//set to present
		idt[i].dpl =0x0;					//at level 0
		idt[i].reserved0 = 0x0;				//reserved0
		idt[i].size = 0x1;					//not sure 1 for 32bits or 0 for 16bits
		idt[i].reserved1 = 0x1;				//reserved1
		idt[i].reserved2 = 0x1;				//reserved2
		idt[i].reserved3 = 0x0;				//reserved3
		idt[i].reserved4 = 0x0;				//reserved4
		idt[i].seg_selector = KERNEL_CS;
	
	
		/* if it is system call, set to user space */
		if (i == SYSTEM_CALL_VEC_NUM)
		{	
			/* Change the privilege level */
			idt[i].dpl = 0x3;				
		}
	}
	
	SET_IDT_ENTRY(idt[0], DE);
	SET_IDT_ENTRY(idt[1], DB);
	SET_IDT_ENTRY(idt[2], NMI);
	SET_IDT_ENTRY(idt[3], BP);
	SET_IDT_ENTRY(idt[4], OF);
	SET_IDT_ENTRY(idt[5], BR);
	SET_IDT_ENTRY(idt[6], UD);
	SET_IDT_ENTRY(idt[7], NM);
	SET_IDT_ENTRY(idt[8], DF);
	SET_IDT_ENTRY(idt[9], CSO);
	SET_IDT_ENTRY(idt[10], TS);
	SET_IDT_ENTRY(idt[11], NP);
	SET_IDT_ENTRY(idt[12], SS);
	SET_IDT_ENTRY(idt[13], GP);
	SET_IDT_ENTRY(idt[14], PF);
	SET_IDT_ENTRY(idt[16], MF);
	SET_IDT_ENTRY(idt[17], AC);
	SET_IDT_ENTRY(idt[18], MC);
	SET_IDT_ENTRY(idt[19], XF);
	
}