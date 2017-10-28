/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
//0xff - Active Low  Implementation
static uint8_t master_mask = ACT_LOW; /* IRQs 0-7  */
static uint8_t slave_mask = ACT_LOW;  /* IRQs 8-15 */

/* i8259_init
 *
 * Description: Initialize the 8259 PIC
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void i8259_init(void) {
	
	//Masking all interrupts
	outb(ACT_LOW, MASTER_8259_PORT + 1);
	outb(ACT_LOW, SLAVE_8259_PORT + 1);

	//Initialization Sequence  -- Uses ICW1
	outb(ICW1, MASTER_8259_PORT);//Master initialization
	outb(ICW1, SLAVE_8259_PORT);//Slave initialization

	//Setting the offset -- Uses ICW2
	outb(ICW2_MASTER, MASTER_8259_PORT + 1);
	outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);

	//Setting the master-slave relationship -- Uses ICW3
	outb(ICW3_MASTER, MASTER_8259_PORT + 1);
	outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);

	//setting the protocols and EOI signal
	outb(ICW4, MASTER_8259_PORT + 1);
	outb(ICW4, SLAVE_8259_PORT + 1);

	//SAVING THE MASKS
	//master_mask = inb(MASTER_8259_PORT + 1);
	//slave_mask = inb(SLAVE_8259_PORT + 1);
}

/* enable_irq
 *
 * Description: Enable (unmask) the specified IRQ
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void enable_irq(uint32_t irq_num) {
	uint16_t port;

	if(irq_num < 8){
		port = MASTER_8259_PORT + 1;
		master_mask = master_mask & ~(1 << irq_num);
		outb(master_mask, port);
	}
	else{
		port = SLAVE_8259_PORT + 1;
		slave_mask = slave_mask & ~(1 << (irq_num - 8));
		outb(slave_mask, port);
	}
}

/* disable_irq
 *
 * Description: Disable (mask) the specified IRQ
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void disable_irq(uint32_t irq_num) {
	uint16_t port;

	if(irq_num < 8){
		port = MASTER_8259_PORT + 1;
		master_mask = master_mask | (1 << irq_num);//~ using this operator as we have mask the IRQ
		outb(master_mask, port);
	}
	else{
		port = SLAVE_8259_PORT + 1;
		slave_mask = slave_mask | (1 << (irq_num - 8));//~ using this operator as we have mask the IRQ
		outb(slave_mask, port);
	}
}

/* send_eoi
 *
 * Description: Send end-of-interrupt signal for the specified IRQ
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void send_eoi(uint32_t irq_num) {
	if (irq_num >= 8){ //It means that the command is from slave. Therefore, we have to issue
		outb(EOI | (irq_num - 8), SLAVE_8259_PORT);				//command to both PIC Chips
		outb(EOI | 2, MASTER_8259_PORT);
	}
	else  {
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
}
