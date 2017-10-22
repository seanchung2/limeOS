
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "idt.h"


// Division Error Handler
void DE()  {
	printf(" Divide Error!!!\n");
	while(1) {}
}

// Reserved Exception Handler
void DB()  {
	printf("RESERVED!!!\n");
	while(1) {}
}

// Non Maskable Interupt Handler
void NMI()  {
	printf("Nonmaskable Interrupt Exception!!!\n");
	while(1) {}
}

// Breakpoint Exception Handler
void BP()  {
	printf("Breakpoint Exception!!!\n");
	while(1) {}
}

// Overflow Exception Handler
void OF()  {
	printf("Overflow Exception!!!\n");
	while(1) {}
}

// Bound Range Exceeded Exception Handler
void BR()  {
	printf("BOUND Range Exceeded Exception!!!\n");
	while(1) {}
}

// Invalid Opcode Exception Handler
void UD()  {
	printf("Invalid Opcode Exception!!!\n");
	while(1) {}
}

// Device Not Available Exception Handler
void NM()  {
	printf("Device Not Available Exception!!!\n");
	while(1) {}
}

// Double Faut Exception Handler
void DF()  {
	printf("Double Fault Exception!!!\n");
	while(1) {}
}

// Coprocessor Segment Overrun Exception Handler
void CSO()  {
	printf("Coprocessor Segment Overrun Exception!!!\n");
	while(1) {}
}

// Invalid TSS Exception Handler
void TS()  {
	printf("Invalid TSS Exception!!!\n");
	while(1) {}
}

// Segment Not Preset Exception Handler
void NP()  {
	printf("Segment Not Present!!!\n");
	while(1) {}
}

// Stack-Segment Fault Exception Handler
void SS()  {
	printf("Stack-Segment Fault Exception!!!\n");
	while(1) {}
}

// General Protetion Exception Handler
void GP()  {
	printf("General Protection Exception!!!\n");
	while(1) {}
}

// Page Fault Exception Handler
void PF()  {
	printf("Page Fault Exception!!!\n");
	while(1) {}
}

// Floating Point Error Exception Handler
void MF()  {
	printf("Floating Point Error Exception!!!\n");
	while(1) {}
}

// Alignment Check Exception Handler
void AC()  {
	printf("Alignment Check Exception!!!\n");
	while(1) {}
}

// Machine Check Exception handler
void MC()  {
	printf("Machine Check Exception!!!\n");
	while(1) {}
}

// SIMD Floatin-Point Exception Handler
void XF()  {
	printf("SIMD Floating Point Exception!!!\n");
	while(1) {}
}

/* init_idt
 *
 * Set the gates and load IDT with correct vectors
 * Inputs: none
 * Outputs: none
 * Side Effects: load IDT
 */
void init_idt()
{
	// load IDT
	lidt(idt_desc_ptr);

	int i;
	
	for (i=0;i<NUM_VEC;i++)
	{
		idt[i].present = 0x1;				//set to present
		idt[i].dpl = 0x0;					//at level 0
		idt[i].reserved0 = 0x0;				//reserved0
		idt[i].size = 0x1;					//not sure 1 for 32bits or 0 for 16bits
		idt[i].reserved1 = 0x1;				//reserved1
		idt[i].reserved2 = 0x1;				//reserved2
		idt[i].reserved3 = 0x0;				//reserved3
		idt[i].reserved4 = 0x0;				//reserved4
		idt[i].seg_selector = KERNEL_CS;
	
		/* Exception defined by Intel should be interrupt */
		if(i < EXCEPTION_DEFINED_BY_INTEL)
			idt[i].reserved3 = 0x1;

		/* if it is system call, set to user space */
		if (i == SYSTEM_CALL_VEC_NUM)
		{	
			idt[i].reserved3 = 0x1;
			/* Change the privilege level */
			idt[i].dpl = 0x3;				
		}
	}
	
	/* Sets runtime parameters for an IDT entry */
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
