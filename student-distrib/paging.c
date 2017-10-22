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

	// Page Descriptor Table struct
	page_table_t PDT __attribute__(aligned(FOUR_MB));

	// Video Memory Page Table struct
	page_table_t PT0 __attribute__(aligned(FOUR_MB));

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