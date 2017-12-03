#include "terminal.h"

#define NUM_COLS        80
#define NUM_ROWS        25

volatile uint8_t t_enter_flag[3];
int backup_mem_addr[3] = {0xB9000, 0xBA000, 0xBB000};

/*
 * void set_t_enter_flag()
 * set t_enter_flag to 1
 * input: 	none
 * output: none
 * side effect: as description
 */
void set_t_enter_flag()
{
	t_enter_flag[terminal_num] = 1;
}
/*
 * int terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * read the input from the keyboard and store in terminal buffer
 * input: 	fd - the index of file
 * 			buf- the input from keyboard
 * output: the number had copied
 * side effect: as description
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
{
	if(buf == NULL)
		return -1;

	int i = 0;
	t_enter_flag[terminal_num] = 0;
	(command_buf)[terminal_num][0] = '\0';

	while(!t_enter_flag[terminal_num]);

	while(1)
	{
		((int8_t*)buf)[i] = (command_buf)[terminal_num][i];
		
		if((command_buf)[terminal_num][i] == '\n')
			break;
		i++;
	}

	return i + 1;
}

/*
 * int terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes)
 * show the terminal buffer on the screen
 * input:   fd - the index of file
 			buf- the input from keyboard
 *			nbytes - how many bytes to write
 * output: success -> return the number put into screen
 *		   fail	   -> return -1
 * side effect: as description
 */
int32_t terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes)
{
	int i;

	if(buf == NULL)
		return -1;

	/* normal charaters */
	for(i=0; i<nbytes; i++)
	{
		if(check_out_of_bound() == SCROLL_ENTER_PRESSED && buf[i] == '\n')
		{
			scroll_screen();
		}
		if(check_out_of_bound() == SCROLL_LAST_LETTER)
		{
			scroll_screen();
		}
		putc(buf[i]);
	}
	return i;
}

/*
 * int terminal_open(const uint8_t* filename)
 * do nothing right now
 * input: filename - name to be open
 * output: none
 */
int32_t terminal_open(const uint8_t* filename)
{
	return 0;
}

/*
 * int terminal_close(int32_t fd)
 * do nothing right now
 * input: fd - index to be closed
 * output: none
 */
int32_t terminal_close(int32_t fd)
{
	return 0;
}

/*
 * void set_terminal_num(int i)
 * change the terminal number
 * input: i - terminal number
 * output: none
 */
void set_terminal_num(int i)
{
	terminal_num = i;
}

/*
*/
void terminal_switch(int new_tty)
{
	page_table_t* PT1 = (page_table_t*)PT1_addr;

	/* copy video memory to backup */
	memcpy((void*)backup_mem_addr[terminal_num], (void *)VIDEO_MEMORY, FOUR_KB);
	/* copy backup to video memory */
	memcpy((void*)VIDEO_MEMORY, (void *)backup_mem_addr[new_tty], FOUR_KB);

	video_mem[terminal_num] = (char*)backup_mem_addr[terminal_num];
	video_mem[new_tty] = (char*)VIDEO_MEMORY;

	PT1->entries[terminal_num] = VID_MEM_BACKUP[terminal_num];
	PT1->entries[new_tty] = USER_VID_MEM_ENTRY;

	terminal_num = new_tty;

	asm volatile (	"movl %%CR3, %%eax;"	
					"andl $0xFFF, %%eax;"
					"addl %0, %%eax;"
					"movl %%eax, %%CR3;"
						:
						: "r" (PDT_addr)
						: "eax", "cc"
						);

	update_cursor(screen_x[terminal_num], screen_y[terminal_num]);
}
