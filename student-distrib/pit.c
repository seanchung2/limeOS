#include "pit.h"

/*
Programmable Interval Timer: A system clock is used to accurately schedule a new process
and allow the current task to wait for a certain period of time.
*/

/* initialize_PIT
 *
 * Description: Initialize the PIT
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void intitialize_PIT(){
	int16_t div = DEFAULT_FREQ/DIVIDER;  

	outb(RESULTANT_BYTE, COMMAND_REGISTER);
	outb(div & SET_LOW, CHANNNEL_0);//low-byte set
	outb(div >> HIGH_BYTE_SET, CHANNNEL_0);//high-byte set
	enable_irq(PIT_IRQ);

	return;
}

/* PIT_handler
 *
 * Description: 
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void PIT_handler(){
    send_eoi(PIT_IRQ);
    //IF ANY OF THE TWO OUT OF THREE TERMINALS RUNNING WE NEED TO DO SWITCH(in scheduling)
    return;
}
