#include "keyboard.h"


/* store the status of shift/ctrl/capslock/enter/alt/terminal_read/add_to_buffer */
uint8_t shift_flag = 0;
uint8_t ctrl_flag = 0;
uint8_t capslock_flag = 0;
uint8_t release_caps = 1;
uint8_t enter_flag = 0;
uint8_t alt_flag = 0;
uint8_t terminal_read_flag = 0;
uint8_t isAdd = 0; 
/* store the input characters */
volatile int buf_index[3] = {-1,-1,-1};
uint8_t char_buf[CHARACTER_BUFFER_SIZE];

/* 
 * initialize_keyboard()
 * Description: Initializes the Keyboard
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void initialize_keyboard(){
	outb(DEFAULT_PARA, KEYBOARD_DATA_PORT);
	uint8_t check = 0x00;
	while(check != KEYBOARD_ACK){
		check = inb(KEYBOARD_DATA_PORT);
		if(check == KEYBOARD_RESEND){
			outb(DEFAULT_PARA, KEYBOARD_DATA_PORT);
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
    c = inb(KEYBOARD_DATA_PORT);

    /* deal with the received character */
    keyboard_output_dealer(c);
    
    /* call terminal write to write buffer to screen */
    if (isAdd == 1)
    {
    	terminal_write(0,(int8_t*)(char_buf+buf_index[terminal_num]) , 1);
    	isAdd = 0;
    }
    if (enter_flag == 1)
    {	
    	buf_index[terminal_num]=-1;
    	enter_flag = 0;
    }


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

	switch(c)
	{
	/* if the scancode is "enter" */
		case PRESS_ENTER:
			enter_flag = 1; 
			char_buf[++buf_index[terminal_num]] = '\n';

			if(command_buf[terminal_num][0] =='\0')
				for(i=0;i<CHARACTER_BUFFER_SIZE;i++)
				{
					command_buf[terminal_num][i] = char_buf[i]; 
					if(char_buf[i] == '\n')
					{
						set_t_enter_flag(terminal_num);
						break;
					}
				}

			isAdd = 1;
			break;

	/* if the scancode is "press_shift" */
		case PRESS_LEFT_SHIFT:
		case PRESS_RIGHT_SHIFT:
			 shift_flag = 1;
			 break;

	/* if the scancode is "release_shift" */
		case RELEASE_LEFT_SHIFT:
		case RELEASE_RIGHT_SHIFT:
			 shift_flag = 0;
			 break;

	/* if the scancode is "press_ctrl" */
		case PRESS_LEFT_CTRL:
			 ctrl_flag = 1;
			 break;

	/* if the scancode is "release_ctrl" */
		case RELEASE_LEFT_CTRL:
			 ctrl_flag = 0;
			 break;

	/* if the scancode is "press_alt" */
		case PRESS_ALT:
			 alt_flag = 1;
			 break;

	/* if the scancode is "release_alt" */
		case RELEASE_ALT:
			 alt_flag = 0;
			 break;

	/* if the scancode is "backspace" */
		case PRESS_BACKSPACE:
			 /* check if the buffer is empty */
			 if(buf_index[terminal_num] > -1)
			 {
		 		buf_index[terminal_num]--;
				backspace_pressed();
			 }
			 break;

	/* if the scancode is "press_capslock" */
		case PRESS_CAPSLOCK:
			 /* check the capslock state */
			 if (release_caps == 1) {
				capslock_flag ^= 1;
				release_caps = 0;
			 }
			 break;

	/* if the scancode is "release_capslock" */
		case RELEASE_CAPSLOCK:
			 release_caps = 1;
			 break;

		default:
			defaultKeyPressed(c);
	}

}

/*
 * defaultKeyPressed (uint8_t c)
 * deal with normal characters
 * input: c, character detected by the keyboard
 * output: none
 * side effect: put charater into buffer
 */
void defaultKeyPressed (uint8_t c)
{
	int index = 2*capslock_flag + 1*shift_flag;

	/* if alt+F1/F2/F3 is pressed, change to each terminal */
	if(alt_flag ==1 && c >= KEY_F1 && c <= KEY_F3)
	{
		terminal_switch((c-KEY_F1));
		return;
	}

	/* if the scancode is nothing to do with normal characters, do nothing */
	if(c>TABLE_SIZE)
		return;

	/* if CTRL+L is pressed, clean the screen and reset the cursor position */
	if (ctrl_flag ==1 && (scancodeTable[index][c] == 'l' || scancodeTable[index][c] == 'L')){
		clear(terminal_num);
		reset_screen();
		buf_index[terminal_num] = -1;
		return;
	}

	/* find the character for this scancode */
	if(scancodeTable[index][c] != '\0')
	{
		if(buf_index[terminal_num] < CHARACTER_BUFFER_SIZE-2)
		{	
			isAdd = 1;
			char_buf[++buf_index[terminal_num]] = scancodeTable[index][c];
		}
	}
	else
		isAdd = 0;
}
