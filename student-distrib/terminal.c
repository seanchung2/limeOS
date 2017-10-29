#include "terminal.h"

#define NUM_COLS        80
#define NUM_ROWS        25

uint8_t terminal_buf[NUM_COLS*NUM_ROWS];
uint8_t terminal_index = 0;

/*
 * int terminal_read(int32_t fd, const int8_t* buf, int32_t nbytes)
 * read the input from the keyboard and store in terminal buffer
 * input: 	fd - the index of file
 * 			buf- the input from keyboard
 * 			nbytes - how many bytes to read
 * output: the number had copied
 * side effect: as description
 */
int terminal_read(int32_t fd, const int8_t* buf, int32_t nbytes)
{
	int i;

	if(buf == NULL || nbytes > NUM_COLS*NUM_ROWS)
	{
		return -1;
	}

	for(i=0;i<nbytes;i++)
	{
		terminal_buf[i] = buf[i];
	}
	terminal_index = nbytes;
	return nbytes;
}

/*
 * int terminal_write(int32_t fd, int32_t nbytes,int enter_flag)
 * show the terminal buffer on the screen
 * input:   fd - the index of file
 *			nbytes - how many bytes to write
 * 			enter_flag - check if enter is pressed 
 * output: success -> return the number put into screen
 *		   fail	   -> return -1
 * side effect: as description
 */
int terminal_write(int32_t fd, int32_t nbytes,int enter_flag)
{
	int i;
	int write_index;
	/* index out of bound */
	if(nbytes > NUM_COLS*NUM_ROWS)
	{
		return -1;
	}

	/* enter pressed */
	if(enter_flag == 1)
	{
		if(check_out_of_bound() == SCROLL_ENTER_PRESSED)
		{
			scroll_screen();
		}
		putc('\n');
		return 1;
	}

	if (nbytes > terminal_index)
		write_index = terminal_index;
	else
		write_index = nbytes;

	/* normal charaters */
	for(i=0; i<write_index; i++)
	{
		if(check_out_of_bound() == SCROLL_LAST_LETTER)
		{
			scroll_screen();
		}
		putc(terminal_buf[i]);
	}
	terminal_index = 0;
	return write_index;
}

/*
 * int terminal_open(const uint8_t* filename)
 * do nothing right now
 * input: filename - name to be open
 * output: none
 */
int terminal_open(const uint8_t* filename)
{
	return 0;
}

/*
 * int terminal_close(int32_t fd)
 * do nothing right now
 * input: fd - index to be closed
 * output: none
 */
int terminal_close(int32_t fd)
{
	return 0;
}
