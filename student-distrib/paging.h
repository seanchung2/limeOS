
#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "lib.h"

#define PAGE_SIZE			1024		//number of entries in a 4KB page
#define FOUR_KB				4096		//number if bytes in 4MB
#define VID_MEM_INDEX		184			//index for video memory in PT0
#define KERNEL_ENTRY		0x400083	//initial PDE for the kernel
#define VID_MEM_ENTRY		0xB8117		//initial PTE for video memory
#define T_ZERO_MASK			0x17		//mask to set the helper bits
#define USER_VID_INDEX		25
#define UVM_MASK			0x7
#define USER_VID_MEM_ENTRY 	0xB8007

// for backup video memory, store in 185*4k, 186*4k, 187*4k
#define KERNEL_VID_MEM_BACKUP_1 	0xB9003
#define KERNEL_VID_MEM_BACKUP_2 	0xBA003
#define KERNEL_VID_MEM_BACKUP_3 	0xBB003
#define VID_MEM_BACKUP_1 	0xB9007
#define VID_MEM_BACKUP_2 	0xBA007
#define VID_MEM_BACKUP_3 	0xBB007
const int VID_MEM_BACKUP[3] = {VID_MEM_BACKUP_1, VID_MEM_BACKUP_2, VID_MEM_BACKUP_3};
extern int PDT_addr;
extern int PT0_addr;
extern int PT1_addr;

// struct for creating page tables
typedef struct page_table_t  {
	int entries[PAGE_SIZE];
} page_table_t;

// function to initialize paging
void init_paging();

#endif
