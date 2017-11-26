#include "filesystem.h"

/* holds the adress for the start of the filesystem */
static uint32_t fs_start;

/* initial current pid */
int32_t current_pid = 0;

/* temporary dentry table */
static int dentry_table_flags[FILE_TABLE_SIZE];

/* read_dentry_by_name
 *
 * INPUTS:	fname - string that represents a file
 * 			dentry - destination to copy dentry to
 * OUTPUTS:	-1 - could not find file with given name
 *			0 - find successful
 * SIDE EFFECTS:	copys the requested dentry structure
 *					to the memory pointed to by dentry
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)  {
	uint32_t name_length = strlen((int8_t*)fname);
	if(name_length > MAX_NAME_LENGTH)  {
		name_length = MAX_NAME_LENGTH;
	}
	int dentry_count = *((int*)fs_start);
	dentry_t* current = (dentry_t*)(fs_start + DENTRY_SIZE);
	int match = 1;
	int i;
	int j;
	int k;

	for(i = 0; i < dentry_count; i++, current++)  {
		match = 1;
		for(j = 0; j < name_length; j++)  {
			if(current->file_name[j] != fname[j])  {
				match = 0;
				break;
			}
		}
		if(match == 1 && j < MAX_NAME_LENGTH)  {
			if(current->file_name[j] != '\0')  {
				match = 0;
			}
		}
		if(match == 1)  {
			for(k = 0; k < MAX_NAME_LENGTH; k++)  {
				dentry->file_name[k] = current->file_name[k];
			}
			dentry->file_type = current->file_type;
			dentry->inode_number = current->inode_number;
			return 0;
		}
	}
	return -1;
}

/* read_dentry_by_index
 *
 * INPUTS:	index - number of dentry inside dentry list
 * 			dentry - destination to copy dentry to
 * OUTPUTS:	-1 - index is outside availible dentries
 *			0 - find successful
 * SIDE EFFECTS:	copys the requested dentry structure
 *					to the memory pointed to by dentry
 */
int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry)  {
	int dentry_count = *((int*)fs_start);
	dentry_t* current = (dentry_t*)(fs_start + DENTRY_SIZE);
	int i;

	if(index > dentry_count)  {
		return -1;
	}

	current = (dentry_t*)(fs_start + ((index + 1) * DENTRY_SIZE));
	for(i = 0; i < MAX_NAME_LENGTH; i++)  {
		dentry->file_name[i] = current->file_name[i];
	}
	dentry->file_type = current->file_type;
	dentry->inode_number = current->inode_number;
	return 0;
}

/* read_data
 *
 * INPUTS:	inode - index number for inode to use
 *			offset - character to start copying from in file
 *			buf - pointer to buffer to copy to
 *			length - number of bytes to copy
 * OUTPUTS:	either the number of bits that were copied, or -1
 *			if an error occured
 * SIDE EFFECTS:	copys the requested number of bytes starting
 *					from the given offset to the given buffer
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)  {
	inode_t* inode_ptr = (inode_t*)(fs_start + ((1 + inode) * FOUR_KB));
	uint32_t inode_count = *((int*)(fs_start + 4));
	uint32_t copied_count = 0;
	uint32_t data_block_index = 0;
	uint32_t byte_number;
	uint8_t* data_block_address;

	/* check if offset is too large for file */
	if(offset >= inode_ptr->length)  {
		return 0;
	}

	/* determine what offset to start at in which data block */
	byte_number = offset;
	while(byte_number >= FOUR_KB)  {
		byte_number -= FOUR_KB;
		data_block_index++;
	}

	/* initialize the data_block_address */
	data_block_address = (uint8_t*)(fs_start + ((inode_count + 1 + inode_ptr->data_block_numbers[data_block_index]) * FOUR_KB));

	/* copy data from data blocks to given buffer */
	while(copied_count < length)  {
		buf[copied_count] = data_block_address[byte_number];
		copied_count++;
		byte_number++;
		offset++;
		if(byte_number >= FOUR_KB)  {
			byte_number = 0;
			data_block_index++;
			data_block_address = (uint8_t*)(fs_start + ((inode_count + 1 + inode_ptr->data_block_numbers[data_block_index]) * FOUR_KB));
		}
		if(offset == inode_ptr->length)  {
			break;
		}
	}
	return copied_count;
}

/* set_fs_start
 *
 * INPUTS:	mod - pointer to the module that holds the file systen
 * OUTPUTS:	none
 * SIDE EFFECTS:	sets fs_start with the address of the start of
 *					the file system
 */

void set_fs_start(module_t* mod)  {
	int i;
	int32_t dentry_count;
	int32_t inode_count;

	fs_start = mod->mod_start;
	dentry_count = *((int32_t*)fs_start);
	inode_count = *((int32_t*)(fs_start + 4));

	printf("Dentry Count: %d \n", dentry_count);
	printf("Inode Count: %d \n", inode_count);

	for(i = 0; i < FILE_TABLE_SIZE; i++)  {
		dentry_table_flags[i] = 0;
	}
}

/* open_file
 *
 * INPUTS:	filename - string that represents a file
 * OUTPUTS:	1
 * SIDE EFFECTS: none
 */
int32_t open_file(const uint8_t* filename)  {
	return 1;
}

/* read_file
 *
 * INPUTS:	fd - file descriptor to read
 *			buf - buffer to write data into
 *			nbytes - number of bytes to copy
 * OUTPUTS:	number of bytes copied
 * SIDE EFFECTS:	copys data from the requested file
 *					to the given buffer
 */
int32_t read_file(int32_t fd, void* buf, int32_t nbytes)  {
	int ret;
	pcb_t* pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);
	uint32_t inode_index = pcb->fd_entry[fd].inode_index;
	ret = read_data(inode_index, pcb->fd_entry[fd].file_position, (uint8_t*)buf, nbytes);
	pcb->fd_entry[fd].file_position+= ret;
	return ret;
}

/* write_file
 *
 * INPUTS:	fd - file descriptor to read
 *			buf - buffer containing data to write to file
 *			nbytes - number of bytes to copy
 * OUTPUTS:	-1 (read only files)
 * SIDE EFFECTS:	None
 */
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes)  {
	return -1;
}

/* close_file
 *
 * INPUTS:	fd - file descriptor to read
 * OUTPUTS: 0
 * SIDE EFFECTS:	None
 */
int32_t close_file(int32_t fd)  {
	return 0;
}

/* open_directory
 *
 * INPUTS:	filename - name of directory to open
 * OUTPUTS:	0
 * SIDE EFFECTS:	None
 */
int32_t open_directory(const uint8_t* filename)  {
	return 0;
}

/* read_directory
 *
 * INPUTS:	fd - file descriptor for directory to read
 *			buf - buffer to copy next entry's name into
 *			nbytes - ignored
 * OUTPUTS:	number of bytes written to buf
 *			0 - all entries have been read
 * SIDE EFFECTS:	copies name of next entry to buf
 */
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes)  {
	pcb_t* PCB = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);
	int directory_position = PCB->fd_entry[fd].file_position;
	int dentry_count = *((int*)(fs_start));
	dentry_t current;
	int i;
	uint8_t* char_ptr;

	if(directory_position >= dentry_count)  {
		return 0;
	}

	if(read_dentry_by_index(directory_position, &current) == -1)  {
		return -1;
	}

	char_ptr = (uint8_t*)&current;
	for(i = 0; i < MAX_NAME_LENGTH; i++)  {
		if(char_ptr[i] == '\0')  {
			break;
		}
		((uint8_t*)buf)[i] = char_ptr[i];
	}
	PCB->fd_entry[fd].file_position ++;
	return i;
}


/* write_directory
 *
 * INPUTS:	fd - ignored
 *			buf - ignored
 *			nbytes - ignored
 * OUTPUTS:	-1 (Read Only)
 * SIDE EFFECTS:	None
 */
int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes)  {
	return -1;
}

/* close_directory
 *
 * INPUTS:	fd - file descriptor for directory to close
 * OUTPUTS:	0
 * SIDE EFFECTS:	None
 */
int32_t close_directory(int32_t fd)  {
	return 0;
}

/* test_set_flags
 *
 * INPUTS:	index - index in dentry_table_flags to change
 *			value - number to change flag to
 * OUTPUTS:	None
 * SIDE EFFECTS:	sets flag for given index to a given value
 */
void test_set_flags(int index, int value)  {
	dentry_table_flags[index] = value;
}

/* test_read_flags
 *
 * INPUTS:	index - index in dentry_table_flags to read
 * OUTPUTS:	value of flag at given index in dentry_table_flags
 * SIDE EFFECTS:	None
 */
int test_read_flags(int index)  {
	return dentry_table_flags[index];
}
