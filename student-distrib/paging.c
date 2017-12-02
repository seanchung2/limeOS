#include "paging.h"

// Page Descriptor Table struct
static page_table_t PDT __attribute__( (aligned(FOUR_KB)) );

// Video Memory Page Table struct
static page_table_t PT0 __attribute__( (aligned(FOUR_KB)) );

static page_table_t PT1 __attribute__( (aligned(FOUR_KB)) );

/* store directory and table addresses */
int PDT_addr = (int)&PDT;
int PT0_addr = (int)&PT0;
int PT1_addr = (int)&PT1;

/*
 * init_paging()
 * Inputs: none
 * Outputs: none
 * Retrun Value: none
 * Side Effects: initializes paging
 */
void init_paging()  {
	int i;
	
	printf("Initializing Paging... \n");

	// Initialize all entries to zero
	for(i = 0; i < PAGE_SIZE; i++)  {
		PDT.entries[i] = 0;
		PT0.entries[i] = 0;
		PT1.entries[i] = 0;
	}

	// Initialize the kernal entry
	PDT.entries[1] = KERNEL_ENTRY;

	// Initialize Kernel Video Memory's page table entry in PDT
	PDT.entries[0] = PT0_addr | T_ZERO_MASK;

	// Initialize the User Video Memory's page table entry in PDT
	PDT.entries[USER_VID_INDEX] = PT1_addr | UVM_MASK;

	// Initialize Video Memory's entry in PT0
	PT0.entries[VID_MEM_INDEX] = VID_MEM_ENTRY;

	// 3 pages for t1 t2 t3 at 100MB 100MB+4KB 100MB+8KB (virtual memory)
	PT0.entries[VID_MEM_INDEX+1] = KERNEL_VID_MEM_BACKUP_1;
	PT0.entries[VID_MEM_INDEX+2] = KERNEL_VID_MEM_BACKUP_2;
	PT0.entries[VID_MEM_INDEX+3] = KERNEL_VID_MEM_BACKUP_3;
	PT1.entries[0] = USER_VID_MEM_ENTRY;
	PT1.entries[1] = VID_MEM_BACKUP_2;
	PT1.entries[2] = VID_MEM_BACKUP_3;

	// Set top bits of CR3 to address of Page Descriptor Table
	asm volatile (	"movl %%CR3, %%eax;"	
					"andl $0xFFF, %%eax;"
					"addl %0, %%eax;"
					"movl %%eax, %%CR3;"
						:
						: "r" (PDT_addr)
						: "eax", "cc"
						);

	// Set Page Size Extension in CR4
	asm volatile (	"movl %%CR4, %%eax;"
					"orl $0x10, %%eax;"
					"movl %%eax, %%CR4;"
						:
						:
						: "eax", "cc"
						);

	// Set PG and PE in CR0
	asm volatile (	"movl %%CR0, %%eax;"
					"orl $0x80000001, %%eax;"
					"movl %%eax, %%CR0;"
						:
						:
						: "eax", "cc"
						);
}
