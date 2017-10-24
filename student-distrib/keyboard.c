#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"


/* initialize_keyboard
 *
 * Description: Initializes the Keyboard
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void initialize_keyboard(){
	outb(DEFAULT_PARA, KEYBOARD_PORT);
	uint8_t check = 0x00;
	while(check != KEYBOARD_ACK){
		check = inb(KEYBOARD_PORT);
		if(check == KEYBOARD_RESEND){
			outb(DEFAULT_PARA, KEYBOARD_PORT);
		}
	}
	enable_irq(1);
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

	/* if the scancode is "enter" */
	if(c == 0x1C)  {
		putc('\n');
		found = 1;
	}

	/* check if the scancode is part of lower letters */
	if(found < 0)
	{
		for(i=0; i<LETTER_NUM;i++)
		{
			if(c == letter_code[i])
			{
				putc((LETTER_OFFSET+i));
				/* go to "type_tester" to check if the key is part of saved keys for test */
				type_tester((char)(LETTER_OFFSET+i));
				found = i;
				break;
			}
		}
	}

	/* check if the scancode is part of numbers */
	if(found < 0)
	{
		for(i=0; i<NUMBER_NUM;i++)
		{
			if(c == number_code[i])
			{
				putc(NUMBER_OFFSET+i);
				/* go to "type_tester" to check if the key is part of saved keys for test */
				type_tester((char)(NUMBER_OFFSET+i));
				found = i;
				break;
			}
		}
	}
}
