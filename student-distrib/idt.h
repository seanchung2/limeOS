#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "idt.h"
#include "system_wrapper.h"
#include "keyboard.h"

/* Define variables */
#define SYSTEM_CALL_VEC_NUM 0x80
#define EXCEPTION_DEFINED_BY_INTEL 0x20 
#define KEYBOARD_VEC_NUM 0x21
#define RTC_VEC_NUM 0x28

/* Define the handler for undefined interrupt
 * Inputs: none
 * Outputs: none
 * Side Effects: print out the "Undefined Interrupt!!!"
 */	
void undefined_interrupt()  {
	printf(" Undefined Interrupt!!!\n");
	while(1) {}
}

/* Define the handler respectively
 * (the following 18 functions are similar 
 * so this function header takes care of all)
 * Inputs: none
 * Outputs: none
 * Side Effects: print out the error message
 */												
// 0 Division Error Handler
void DE()  {
	printf(" Divide Error!!!\n");
	while(1) {}
}

// 1 Reserved Exception Handler
void DB()  {
	printf("RESERVED!!!\n");
	while(1) {}
}

// 2 Non Maskable Interupt Handler
void NMI()  {
	printf("Nonmaskable Interrupt Exception!!!\n");
	while(1) {}
}

// 3 Breakpoint Exception Handler
void BP()  {
	printf("Breakpoint Exception!!!\n");
	while(1) {}
}

// 4 Overflow Exception Handler
void OF()  {
	printf("Overflow Exception!!!\n");
	while(1) {}
}

// 5 Bound Range Exceeded Exception Handler
void BR()  {
	printf("BOUND Range Exceeded Exception!!!\n");
	while(1) {}
}

// 6 Invalid Opcode Exception Handler
void UD()  {
	printf("Invalid Opcode Exception!!!\n");
	while(1) {}
}

// 7 Device Not Available Exception Handler
void NM()  {
	printf("Device Not Available Exception!!!\n");
	while(1) {}
}

// 8 Double Faut Exception Handler
void DF()  {
	printf("Double Fault Exception!!!\n");
	while(1) {}
}

// 9 Coprocessor Segment Overrun Exception Handler
void CSO()  {
	printf("Coprocessor Segment Overrun Exception!!!\n");
	while(1) {}
}

// 10 Invalid TSS Exception Handler
void TS()  {
	printf("Invalid TSS Exception!!!\n");
	while(1) {}
}

// 11 Segment Not Preset Exception Handler
void NP()  {
	printf("Segment Not Present!!!\n");
	while(1) {}
}

// 12 Stack-Segment Fault Exception Handler
void SS()  {
	printf("Stack-Segment Fault Exception!!!\n");
	while(1) {}
}

// 13 General Protetion Exception Handler
void GP()  {
	printf("General Protection Exception!!!\n");
	while(1) {}
}

// 14 Page Fault Exception Handler
void PF()  {
	printf("Page Fault Exception!!!\n");
	while(1) {}
}

// 15 Reserved
void Resr()  {
	printf("Reserved!!!\n");
	while(1) {}
}

// 16 Floating Point Error Exception Handler
void MF()  {
	printf("Floating Point Error Exception!!!\n");
	while(1) {}
}

// 17 Alignment Check Exception Handler
void AC()  {
	printf("Alignment Check Exception!!!\n");
	while(1) {}
}

// 18 Machine Check Exception handler
void MC()  {
	printf("Machine Check Exception!!!\n");
	while(1) {}
}

// 19 SIMD Floatin-Point Exception Handler
void XF()  {
	printf("SIMD Floating Point Exception!!!\n");
	while(1) {}
}

/* a wrapper to map idt table to keyboard_handler */
void k_handler();
/* a wrapper to map idt table to RTC_handler */
void R_handler();

/* initialize the IDT */
extern void init_idt();

#endif
