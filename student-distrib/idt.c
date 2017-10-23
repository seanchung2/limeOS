#include "idt.h"

/* init_idt
 *
 * Set the gates and load IDT with correct vectors
 * Inputs: none
 * Outputs: none
 * Side Effects: load IDT
 */
void init_idt()
{
	int i;

	// load IDT
	lidt(idt_desc_ptr);
	
	/* Defined the inturrpt vectors correspond to exception */
	/* default to "interrput gate" */
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
			idt[i].reserved3 = 0x0;

		/* if it is system call, set to user space, should be trap */
		if (i == SYSTEM_CALL_VEC_NUM)
		{	
			idt[i].reserved3 = 0x1;
			/* Change the privilege level */
			idt[i].dpl = 0x3;				
		}

		/* Sets runtime parameters for IDT entries */
		SET_IDT_ENTRY(idt[i], undefined_interrupt);
	}
	
	/* Sets runtime parameters for IDT entries */
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
	SET_IDT_ENTRY(idt[15], Resr);
	SET_IDT_ENTRY(idt[16], MF);
	SET_IDT_ENTRY(idt[17], AC);
	SET_IDT_ENTRY(idt[18], MC);
	SET_IDT_ENTRY(idt[19], XF);
	
	/* set system call handler to the entry */
	SET_IDT_ENTRY(idt[SYSTEM_CALL_VEC_NUM], sys_handler);

	/* set keyboard handler to the entry */
	SET_IDT_ENTRY(idt[KEYBOARD_VEC_NUM], k_handler);

	/* set rtc handler to the entry */
	SET_IDT_ENTRY(idt[RTC_VEC_NUM], R_handler);
	
}

/*
 * k_handler
 * a wrapper to map idt table to keyboard_handler
 * Inputs: none
 * Outputs: none
 */
void k_handler()
{
	asm volatile (	"pushl	%ebp;"
					"movl	%ebp,%esp;"
					"pushl	%ebx;"	
					"pushl	%esi;"
					"pushl	%edi;"
						"pushl	%eax;"
						"pushl	%ecx;"
						"pushl	%edx;"
							"call 	keyboard_handler;"
						"popl	%edx;"
						"popl	%ecx;"
						"popl	%eax;"
					"popl	%edi;"
					"popl	%esi;"
					"popl	%ebx;"
					"leave;"
					"iret;"
				);
}

/* 
 * R_handler
 * a wrapper to map idt table to RTC_handler
 * Inputs: none
 * Outputs: none
 */
void R_handler()
{
	asm volatile (	"pushl	%ebp;"
					"movl	%ebp,%esp;"
					"pushl	%ebx;"	
					"pushl	%esi;"
					"pushl	%edi;"
						"pushl	%eax;"
						"pushl	%ecx;"
						"pushl	%edx;"
							"call 	test_interrupts;"
						"popl	%edx;"
						"popl	%ecx;"
						"popl	%eax;"
					"popl	%edi;"
					"popl	%esi;"
					"popl	%ebx;"
					"leave;"
					"iret;"
									);
}
