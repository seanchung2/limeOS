#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "i8259.h"
#include "sched.h"

/*
Programmable Interval Timer: A system clock is used to accurately schedule a new process
and allow the current task to wait for a certain period of time.
*/

#define CHANNNEL_0			0x40
#define CHANNNEL_1 			0X41
#define CHANNNEL_2 			0x42
#define COMMAND_REGISTER	0x43
#define SET_LOW				0xFF
#define PIT_IRQ				0
#define HIGH_BYTE_SET		8

#define DEFAULT_FREQ		1193180
#define DIVIDER				100

//CNTR: 00b
//RW: 11b - lobyte/hibyte -- Need a 16-bit value
//Operating Mode: 3 -  Square Wave Mode
//BCD: 0b
#define RESULTANT_BYTE		0x36 //Byte to be set in COMMAND_REGISTER

/* initialize_PIT
 *
 * Description: Initialize the PIT
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void intitalize_PIT();

/* PIT_handler
 *
 * Description: 
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void PIT_handler();

#endif
