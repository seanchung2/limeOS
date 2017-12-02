#include "terminal.h"

#define NUM_COLS        80
#define NUM_ROWS        25

volatile uint8_t t_enter_flag[3];
int terminal_num = 0;
/*
 * void set_t_enter_flag()
 * set t_enter_flag to 1
 * input: 	none
 * output: none
 * side effect: as description
 */
void set_t_enter_flag()
{
	t_enter_flag[get_tty()] = 1;
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
	int local_t_num = get_tty();
	t_enter_flag[local_t_num] = 0;
	(command_buf)[local_t_num][0] = '\0';

	while(!t_enter_flag[local_t_num]);

	while(1)
	{
		((int8_t*)buf)[i] = (command_buf)[local_t_num][i];
		
		if((command_buf)[local_t_num][i] == '\n')
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
