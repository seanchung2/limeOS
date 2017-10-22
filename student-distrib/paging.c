#include "paging.h"

/*
 * init_paging()
 * Inputs: none
 * Outputs: none
 * Retrun Value: none
 * Side Effects: innitializes paging
 */
void init_paging()  {
	int i;

	// Initialize all entries to zero
	for(i = 0; i < PAGE_SIZE; i++)  {
		PDT.entries[i] = 0;
		PT0.entries[i] = 0;
	}

	// Initialize the kernal entry
	PDT.entries[1] = KERNEL_ENTRY;

	// Initialize Video Memory's page table entry in PDT
	PDT.entries[0] = &PT0 | T_ZERO_MASK;

	// Initialize Video Memory's entry in PT0
	PT0.entries[VID_MEM_INDEX] = VID_MEM_ENTRY;

	// Set top bits of CR3 to address of Page Descriptor Table
	asm volatile (	"andl $0xFFF, %%CR3 \n\t"
					"addl %1, %%CR3"
					:
					: "r" &PDT
					:
					);
	// Set Page Size Extension in CR3
	asm volatile (	"orl $0x10, %%CR3");

	// Set PG and PE in CR0
	asm volatile (	"orl $0x80000001, %%CR0");
}
