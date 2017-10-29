#include "filesystem.h"

/* holds the adress for the start of the filesystem */
static uint32_t fs_start;

/* General PCB for MP3.2 testing */
//static PCB_table_t PCB;

/* stdin file descriptor entry */
//static fd_entry_t stdin;

/* stdout file descriptor entry */
//static fd_entry_t stdout;

//PCB.file_descriptors[0] = &stdin;
//PCB.file_descriptors[1] = &stdout;

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
	uint32_t data_block_index;
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
	fs_start = mod->mod_start;
}
