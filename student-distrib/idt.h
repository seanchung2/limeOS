#ifndef _IDT_H
#define _IDT_H

/* Define variables */
#define SYSTEM_CALL_VEC_NUM 0x80
#define EXCEPTION_DEFINED_BY_INTEL 0x20 

/* Setup a macro handler */
#define EXECPTION_HANDLER (exeception_name,msg_print)	\
void exeception_name()									\
{														\
	printf("%s\n",#msg_print); 							\
	while(1);											\
}

/* Define the handler respectively
 * (the following 18 functions are similar 
 * so this function header takes care of all)
 * Inputs: none
 * Outputs: none
 * Side Effects: print out the error message
 */												
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

/* initialize the IDT */
extern void init_idt();

#endif
