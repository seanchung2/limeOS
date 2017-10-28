#include "terminal.h"

#define NUM_COLS        80
#define NUM_ROWS        25

uint8_t terminal_buf[NUM_COLS*NUM_ROWS];
uint32_t terminal_index = 0;

int terminal_read(const int8_t * buf)
{
	int i;
	int size = strlen(buf);

	if(buf == NULL)
	{
		return -1;
	}

	for(i=0;i<size;i++)
	{
		terminal_buf[i] = buf[i];
	}
	terminal_index = size;
	return size;
}


int terminal_write(int enter_flag)
{
	int i;
	int ret;

	/* index out of bound */
	if(terminal_index > NUM_COLS*NUM_ROWS)
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

	/* normal charaters */
	for(i=0; i<terminal_index; i++)
	{
		if(check_out_of_bound() == SCROLL_LAST_LETTER)
		{
			scroll_screen();
		}
		putc(terminal_buf[i]);
	}

	ret = terminal_index;
	terminal_index = 0;
	return ret;
}
