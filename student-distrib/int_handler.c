#include "int_handler.h"
#include "lib.h"
#include "i8259.h"

/* some code will be read from keyboard data code */
static char letter_code[LETTER_NUM] = { 0x1C, 0x32, 0x21, 0x23,
										0x24, 0x2B, 0x34, 0x33,
										0x43, 0x3B, 0x42, 0x4B,
										0x3A, 0x31, 0x44, 0x4D, 
										0x15, 0x2D, 0x1B, 0x2C,
										0x3C, 0x2A, 0x1D, 0x22,
										0x35, 0x1A};
static char number_code[NUMBER_NUM] = { 0x45, 0x16, 0x1E, 0x26, 0x25,
										0x2E, 0x36, 0x3D, 0x3E, 0x46};		

/*
* sys_handler()
* hanlder for system call
* input: none
* output: none
* side effect: as description
*/
void sys_handler ()
{
	printf("System Call !!!!!! \n");
}

/*
* keyboard_handler()
* handler for keyboard
* input: none
* output: none
* side effect: as description
*/
void keyboard_handler ()
{
	/* prevent another keyboard or other interrputs from interrputing this handler */
	cli();

	char c = 0;

	/* get the data from keyboard data port */
    do {
        if(inb(KEYBOARD_DATA_PORT) != c) {
            c = inb(KEYBOARD_DATA_PORT);
            if(c > 0)
                break;
        }
    } while(1);

    /* deal with the received character */
    keyboard_output_dealer(c);

    /* Send end-of-interrupt signal for the specified IRQ */
    send_eoi(KEYBOARD_IRQ);
	sti();
}

/*
* keyboard_output_dealer()
* deal with output of keyboard
* input: c, character detected by the keyboard
* output: none
* side effect: print out the received character
*/
void keyboard_output_dealer (char c)
{
	int i;
	int found = -1;

	if(found < 0)
	{
		for(i=0; i<LETTER_NUM;i++)
		{
			if(c == letter_code[i])
			{
				putc((LETTER_OFFSET+i));
				found = i;
				break;
			}
		}
	}

	if(found < 0)
	{
		for(i=0; i<NUMBER_NUM;i++)
		{
			if(c == number_code[i])
			{
				putc(NUMBER_OFFSET+i);
				found = i;
				break;
			}
		}
	}
}
