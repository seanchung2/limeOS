#include "paging.h"

// Page Descriptor Table struct
static page_table_t PDT __attribute__( (aligned(FOUR_KB)) );

// Video Memory Page Table struct
static page_table_t PT0 __attribute__( (aligned(FOUR_KB)) );

/*
 * init_paging()
 * Inputs: none
 * Outputs: none
 * Retrun Value: none
 * Side Effects: initializes paging
 */
void init_paging()  {
	int i;
	int PDT_addr = (int)&PDT;
	int PT0_addr = (int)&PT0;

	printf("Initializing Paging... \n");
	// Initialize all entries to zero
	for(i = 0; i < PAGE_SIZE; i++)  {
		PDT.entries[i] = 0x2;
		PT0.entries[i] = (i*FOUR_KB) | 0x2;
	}

	// Initialize the kernal entry
	PDT.entries[1] = KERNEL_ENTRY;

	// Initialize Video Memory's page table entry in PDT
	PDT.entries[0] = PT0_addr | 3;

	// Initialize Video Memory's entry in PT0
	PT0.entries[VID_MEM_INDEX] |= 3;

	// Set top bits of CR3 to address of Page Descriptor Table
	asm volatile (	"movl %0, %%eax;"
					"movl %%eax, %%CR3;"
						:
						: "r" (PDT_addr)
						: "eax", "cc", "memory"
						);

	// Set Page Size Extension in CR4
	asm volatile (	"movl %%CR4, %%eax;"
					"orl $0x10, %%eax;"
					"movl %%eax, %%CR4;"
					:
					: 
					: "eax", "cc", "memory"
					);

	// Set PG and PE in CR0
	asm volatile (	"movl %%CR0, %%eax;"
					"orl $0x80000001, %%eax;"
					"movl %%eax, %%CR0;"
					:
					: 
					: "eax", "cc", "memory"
					);
}
