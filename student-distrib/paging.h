
#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "lib.h"

#define PAGE_SIZE		1024		//number of entries in a 4KB page
#define FOUR_KB			4096		//number if bytes in 4MB
#define VID_MEM_INDEX	184			//index for video memory in PT0
#define KERNEL_ENTRY	0x400083	//initial PDE for the kernel
#define VID_MEM_ENTRY	0xB8117		//initial PTE for video memory
#define T_ZERO_MASK		0x17		//mask to set the helper bits

extern static int PDT_addr;
extern static int PT0_addr;

// struct for creating page tables
typedef struct page_table_t  {
	int entries[PAGE_SIZE];
} page_table_t;

// function to initialize paging
void init_paging();

#endif
