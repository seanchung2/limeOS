#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "lib.h"
#include "multiboot.h"

#define MAX_NAME_LENGTH 	32
#define RESERVED_LENGTH 	24
#define DATA_BLOCK_COUNT 	1023
#define DENTRY_SIZE 		64
#define FOUR_KB 			4096
#define ONE_KB 				1024
#define FILE_TABLE_SIZE 	8
#define INODE_OFFSET 		40
#define KERNEL_BOT_ADDR		0x800000
#define EIGHT_KB			0x2000
#define MAX_LENGTH_ARG 			128

extern int32_t current_pid;

/* directory entry structure */
typedef struct dentry  {
	int8_t file_name[MAX_NAME_LENGTH];
	int32_t file_type;
	int32_t inode_number;
	int8_t reserved[RESERVED_LENGTH];
} dentry_t;

/* inode structure */
typedef struct inode  {
	int32_t length;
	int32_t data_block_numbers[DATA_BLOCK_COUNT];
} inode_t;

/* entry in file array of PCB */
typedef struct fd_entry  {
	int32_t (*operations_pointer[4])();
	int32_t inode_index;
	int32_t file_position;
	int32_t flags;
} fd_entry_t;

/* structure for PCB */
typedef struct process_control_block  {
	fd_entry_t fd_entry[8];
	uint32_t process_id;
	uint32_t parent_id;
	uint32_t child_id;
	uint32_t parent_esp;
	uint32_t parent_ebp;
	uint32_t return_value;
	uint32_t return_instruction;
	uint32_t kernel_stack;
	uint32_t parent_esp0;
	uint8_t args[MAX_LENGTH_ARG]; 
} pcb_t;

/* function to set memory address for the start of the filesystem */
void set_fs_start(module_t* mod);

/* functions to find specific directory entry by name or index */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/* function to retrieve data from given file */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* functions to operate on normal files */
int32_t open_file(const uint8_t* filename);
int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes);
int32_t close_file(int32_t fd);

/* functions to operate on directory files */
int32_t open_directory(const uint8_t* filename);
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes);
int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes);
int32_t close_directory(int32_t fd);

/* helper functions to set and read flags from tests.c */
void test_set_flags(int index, int value);
int test_read_flags(int index);

#endif /* _FILESYSTEM_H */
