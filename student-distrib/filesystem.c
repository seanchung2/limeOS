#include "filesystem.h"

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
	int dentry_count = *((int*)FS_START);
	dentry_t* current = (dentry_t*)(FS_START + DENTRY_SIZE);
	int match = 1;
	int i;
	int j;

	for(i = 0; i < dentry_count; i++, current++)  {
		match = 1;
		for(j = 0; j < name_length; j++)  {
			if(current->file_name[j] != fname[j])  {
				match = 0;
				break;
			}
		}
		if(match == 1)  {
			*dentry = *current;
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
	int dentry_count = *((int*)FS_START);
	dentry_t* current = (dentry_t*)(FS_START + DENTRY_SIZE);
	int i;

	for(i = 0; i < dentry_count; i++, current++)  {
		if(current->inode_number == index)  {
			*dentry = *current;
			return 0;
		}
	}
	return -1;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)  {
	inode_t* inode_ptr = (inode_t*)(FS_START + ((1 + inode) * FOUR_KB));
	uint32_t inode_count = *((int*)(FS_START + 4));
	uint32_t copied_count;
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
	data_block_address = (uint8_t*)(FS_START + ((inode_count + 1 + inode_ptr->data_block_numbers[data_block_index]) * FOUR_KB));

	/* copy data from data blocks to given buffer */
	while(copied_count < length)  {
		buf[copied_count] = data_block_address[byte_number];
		copied_count++;
		byte_number++;
		offset++;
		if(byte_number >= ONE_KB)  {
			byte_number = 0;
			data_block_index++;
			data_block_address = (uint8_t*)(FS_START + ((inode_count + 1 + inode_ptr->data_block_numbers[data_block_index]) * FOUR_KB));
		}
		if(offset == inode_ptr->length)  {
			break;
		}
	}

	/* add a null character to the end of the string and return the number of copied characters */
	buf[copied_count] = '\0';
	return copied_count;
}
