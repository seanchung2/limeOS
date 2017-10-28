#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "lib.h"

#define MAX_NAME_LENGTH 32
#define RESERVED_LENGTH 24
#define DATA_BLOCK_COUNT 1023
#define FS_START 
#define DENTRY_SIZE 64
#define FOUR_KB 4096
#define ONE_KB 1024

/* directory entry structure */
typedef struct dentry_t  {
	int8_t file_name[MAX_NAME_LENGTH];
	int32_t file_type;
	int32_t inode_number;
	int8_t reserved[RESERVED_LENGTH];
} dentry_t;

/* inode structure */
typedef struct inode_t  {
	int32_t length;
	int32_t data_block_numbers[DATA_BLOCK_COUNT];
} inode_t;

/* entry in file array of PCB */
typedef struct FD_entry_t  {
	int32_t operations_pointer;
	int32_t inode_index;
	int32_t file_position;
	int32_t flags;
} FD_entry_t;

/* functions to find specific directory entry by name or index */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/* function to retrieve data from given file */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif /* _FILESYSTEM_H */