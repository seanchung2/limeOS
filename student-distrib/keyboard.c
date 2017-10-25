#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"

/* store the status of shift/ctrl */
int shift_flag = 0;
int ctrl_flag = 0;
int capslock_flag = 0;
int release_caps = 1;

/* store the input characters */
int buf_index = 0;
uint8_t char_buf[CHARACTER_BUFFER_SIZE];

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
	if(c == PRESS_ENTER)  {
		putc('\n');
		found = 1;
	}

	/* if the scancode is "press_shift" */
	if (c == PRESS_LEFT_SHIFT || c == PRESS_RIGHT_SHIFT){
		shift_flag = 1;
	}

	/* if the scancode is "release_shift" */
	if (c == RELEASE_LEFT_SHIFT || c == RELEASE_RIGHT_SHIFT){
		shift_flag = 0;
	}

	/* if the scancode is "press_ctrl" */
	if (c == PRESS_LEFT_CTRL){
		ctrl_flag = 1;
	}

	/* if the scancode is "release_ctrl" */
	if (c == RELEASE_LEFT_CTRL){
		ctrl_flag = 0;
	}

	/* if the scancode is "backspace" */
	if (c == PRESS_BACKSPACE)
	{
		backspace_pressed();
		return;
	}

	/* if the scancode is "press_capslock" */
	if (c == PRESS_CAPSLOCK)
	{
		if (release_caps == 1){
			capslock_flag ^= 1;
			release_caps = 0;
		}
	}

	/* if the scancode is "release_capslock" */
	if (c == RELEASE_CAPSLOCK)
	{
		release_caps = 1;
	}
	
	/* if the scancode is "PRESS_SPACE" */
	if (c == PRESS_SPACE)
	{
		putc(' ');
		found = ' ';
	}

	/* check if the scancode is part of letters */
	if(found < 0)
	{
		for(i=0; i<LETTER_NUM;i++)
		{
			if(c == letter_code[i])
			{
				/* if CTRL+L is pressed, clean the screen and reset the cursor position */
				if (ctrl_flag ==1 && c == letter_code[L_ORDER]){
					clear();
					reset_screen();
					return;
				}

				/* shift is pressed 	-> upper case */
				if( (shift_flag ^ capslock_flag) == 1){
					putc((UPPER_LETTER_OFFSET+i));
				}
				/* shift is not pressed -> lower case */
				else{
					putc((LOWER_LETTER_OFFSET+i));
				}

				/* go to "type_tester" to check if the key is part of saved keys for test */
				//type_tester((char)(LETTER_OFFSET+i));
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
				/* shift is pressed 	-> sign */
				if(shift_flag == 1){
					putc(sign_asciicode[i]);
				}
				/* shift is not pressed -> number */
				else{
					putc(NUMBER_OFFSET+i);
				}

				/* go to "type_tester" to check if the key is part of saved keys for test */
				//type_tester((char)(NUMBER_OFFSET+i));
				found = i;
				break;
			}
		}
	}
}
