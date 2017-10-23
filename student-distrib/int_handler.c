#include "int_handler.h"
#include "lib.h"
#include "i8259.h"

/* some code will be read from keyboard data code */
static uint8_t letter_code[LETTER_NUM] = {	0x1E, 0x30, 0x2E, 0x20,
											0x12, 0x21, 0x22, 0x23,
											0x17, 0x24, 0x25, 0x26,
											0x32, 0x31, 0x18, 0x19,
											0x10, 0x13, 0x1F, 0x14,
											0x16, 0x2F, 0x11, 0x2D,
											0x15, 0x2C};
static uint8_t number_code[NUMBER_NUM] = {	0x0B, 0x02, 0x03, 0x04, 0x05,
											0x06, 0x07, 0x08, 0x09, 0x0A};

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
	uint8_t c = 0;

	/* get the data from keyboard data port */
	cli();
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
    while(1)  {}
}

/*
* keyboard_output_dealer()
* deal with output of keyboard
* input: c, character detected by the keyboard
* output: none
* side effect: print out the received character
*/
void keyboard_output_dealer (uint8_t c)
{
	int i;
	int found = -1;
	if(c == 0x1C)  {
		putc('\n');
	}
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
