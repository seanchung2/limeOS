#include "filesystem.h"

/* holds the adress for the start of the filesystem */
static uint32_t fs_start;

/* temporary dentry table */
static dentry_t dentry_table[8];
static int dentry_table_flags[8];

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
	if(offset > inode_ptr->length)  {
		return -1;
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

	/* add a null character to the end of the string and return the number of copied characters */
	buf[copied_count] = '\0';
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

	for(i = 0; i < 8; i++)  {
		dentry_table_flags[i] = 0;
	}
}

int32_t open_file(const uint8_t* filename)  {
	int i = 0;

	while(dentry_table_flags[i] != 0 && i < 8)  {
		i++;
	}

	if(i >= 8)  {
		return -1;
	}

	read_dentry_by_name(filename, &(dentry_table[i]));
	dentry_table_flags[i] = 1;

	return i;
}

int32_t read_file(int32_t fd, void* buf, int32_t nbytes)  {
	uint32_t inode_index = dentry_table[fd].inode_number;
	return read_data(inode_index, 0, (uint8_t*)buf, nbytes);
}

int32_t write_file(int32_t fd, const void* buf, int32_t nbytes)  {
	return -1;
}

int32_t close_file(int32_t fd)  {
	if(fd < 0 || fd > 7)  {
		return -1;
	}
	dentry_table_flags[fd] = 0;
	return 0;
}

int32_t open_directory(const uint8_t* filename)  {
	int i = 0;

	while(dentry_table_flags[i] != 0 && i < 8)  {
		i++;
	}

	if(i >= 8)  {
		return -1;
	}

	read_dentry_by_name(filename, &(dentry_table[i]));
	dentry_table_flags[i] = 1;

	return i;
}

int32_t read_directory(int32_t fd, void* buf, int32_t nbytes)  {
	int dentry_count = *((int*)(fs_start));
	dentry_t current;
	int i;
	int j;
	uint8_t* char_ptr;

	for(i = 0; i < dentry_count; i++)  {
		j = 0;
		read_dentry_by_index(i, &current);
		char_ptr = (uint8_t*)&current;
		printf("File Name: ");
		while(*char_ptr != '\0' && j < 32)  {
			putc(*char_ptr);
			char_ptr++;
			j++;
		}
		putc('\n');
	}
	return 0;
}

int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes)  {
	return -1;
}

int32_t close_directory(int32_t fd)  {
	if(fd < 0 || fd > 7)  {
		return -1;
	}
	dentry_table_flags[fd] = 0;
	return 0;
}
