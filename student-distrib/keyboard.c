#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"

/* store the status of shift/ctrl */
uint8_t shift_flag = 0;
uint8_t ctrl_flag = 0;
uint8_t capslock_flag = 0;
uint8_t release_caps = 1;
uint8_t enter_flag = 0;
uint8_t alt_flag = 0;
uint8_t terminal_read_flag = 0;
uint8_t add_or_not = 0; 
/* store the input characters */
volatile int buf_index = -1;
//volatile int terminal_index = -1;
uint8_t char_buf[CHARACTER_BUFFER_SIZE];

/* initialize_keyboard
 *
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

    char_buf[buf_index+1] = '\0';
    if (terminal_read_flag && add_or_not)
    	terminal_read((int8_t*)(char_buf+buf_index));
    terminal_read_flag = 0;
    add_or_not = 0;
    /* call terminal write to write buffer to screen */
    terminal_write(enter_flag);
    enter_flag = 0;

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
	if(c == PRESS_ENTER) { buf_index = -1; enter_flag = 1; found = 1; }

	/* if the scancode is "press_shift" */
	if (c == PRESS_LEFT_SHIFT || c == PRESS_RIGHT_SHIFT) { shift_flag = 1; found = 1; }

	/* if the scancode is "release_shift" */
	if (c == RELEASE_LEFT_SHIFT || c == RELEASE_RIGHT_SHIFT) { shift_flag = 0; found = 1; }

	/* if the scancode is "press_ctrl" */
	if (c == PRESS_LEFT_CTRL) { ctrl_flag = 1; found = 1; return;}

	/* if the scancode is "release_ctrl" */
	if (c == RELEASE_LEFT_CTRL){ ctrl_flag = 0; found = 1; return;}

	/* if the scancode is "press_alt" */
	if (c == PRESS_ALT) {  alt_flag = 1; found = 1; }

	/* if the scancode is "release_alt" */
	if (c == RELEASE_ALT){  alt_flag = 0; found = 1; }

	/* if the scancode is "backspace" */
	if (c == PRESS_BACKSPACE) {
		if(buf_index > -1)
		{
			buf_index--;
			backspace_pressed();
		}
		return;
	}

	/* if the scancode is "press_capslock" */
	if (c == PRESS_CAPSLOCK) {
		found = 1;
		terminal_read_flag = 0;
		if (release_caps == 1) {
			capslock_flag ^= 1;
			release_caps = 0;
		}
	}

	/* if the scancode is "release_capslock" */
	if (c == RELEASE_CAPSLOCK) { terminal_read_flag = 0; release_caps = 1; found = 1; }
	
	/* if the scancode is "PRESS_SPACE" */
	if (c == PRESS_SPACE) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2)
		{
			add_or_not = 1;
			char_buf[++buf_index] = ' ';
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_TAB" */
	if (c == PRESS_TAB) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2)
		{
			add_or_not = 1;
			char_buf[++buf_index] = TAB;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_EQUAL" */
	if (c == PRESS_EQUAL) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = PLUS;
			else
				char_buf[++buf_index] = EQUAL;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_HYPHEN" */
	if (c == PRESS_HYPHEN) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = UNDERSCOPE;
			else
				char_buf[++buf_index] = HYPHEN;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_BACKSLASH" */
	if (c == PRESS_BACKSLASH) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = VERTICAL_SLASH;
			else
				char_buf[++buf_index] = BACKSLASH;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_SINGLE_QUOTE" */
	if (c == PRESS_SINGLE_QUOTE) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = DOUBLE_QUOTE;
			else
				char_buf[++buf_index] = SINGLE_QUOTE;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_BACK_TICK" */
	if (c == PRESS_BACK_TICK) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = TILDE;
			else
				char_buf[++buf_index] = BACK_TICK;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_COMMA" */
	if (c == PRESS_COMMA) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = LESS;
			else
				char_buf[++buf_index] = COMMA;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_DOT" */
	if (c == PRESS_DOT) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = GREATER;
			else
				char_buf[++buf_index] = DOT;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_SLASH" */
	if (c == PRESS_SLASH) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = QUESTION;
			else
				char_buf[++buf_index] = SLASH;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_SEMICOLON" */
	if (c == PRESS_SEMICOLON) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = COLON;
			else
				char_buf[++buf_index] = SEMICOLON;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_LEFT_SQUARE_BRACKET" */
	if (c == PRESS_LEFT_SQUARE_BRACKET) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = LEFT_CURLY_BRACKET;
			else
				char_buf[++buf_index] = LEFT_SQUARE_BRACKET;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_RIGHT_SQUARE_BRACKET" */
	if (c == PRESS_RIGHT_SQUARE_BRACKET) {
		terminal_read_flag = 1;
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			if(shift_flag)
				char_buf[++buf_index] = RIGHT_CURLY_BRACKET;
			else
				char_buf[++buf_index] = RIGHT_SQUARE_BRACKET;
			add_or_not = 1;
		}
		found = ' ';
	}

	/* check if the scancode is part of letters */
	if(found < 0)
	{
		for(i=0; i<LETTER_NUM; i++)
		{
			if(c == letter_code[i])
			{
				/* if CTRL+L is pressed, clean the screen and reset the cursor position */
				if (ctrl_flag ==1 && c == letter_code[L_ORDER]){
					clear();
					reset_screen();
					buf_index = -1;
					return;
				}

				/* shift is pressed 	-> upper case */
				if( (shift_flag ^ capslock_flag) == 1){
					if(buf_index < CHARACTER_BUFFER_SIZE-2)
					{
						add_or_not = 1;
						char_buf[++buf_index] = UPPER_LETTER_OFFSET+i;
					}
				}
				/* shift is not pressed -> lower case */
				else{
					if(buf_index < CHARACTER_BUFFER_SIZE-2)
					{
						add_or_not = 1;
						char_buf[++buf_index] = LOWER_LETTER_OFFSET+i;
					}
				}
				found = i;
				terminal_read_flag = 1;
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
					if(buf_index < CHARACTER_BUFFER_SIZE-2)
					{
						add_or_not = 1;
						char_buf[++buf_index] = sign_asciicode[i];
					}
				}
				/* shift is not pressed -> number */
				else{
					if(buf_index < CHARACTER_BUFFER_SIZE-2)
					{
						add_or_not = 1;
						char_buf[++buf_index] = NUMBER_OFFSET+i;
					}
				}
				found = i;
				terminal_read_flag = 1;
				break;
			}
		}
	}

	if(found==-1 && (c>RELEASE_SCANCODE_UPPERBOUND || c<RELEASE_SCANCODE_LOWERBOUND) && c!=ACK_SCANCODE)
	{
		terminal_read_flag = 0;
		if(check_out_of_bound() == SCROLL_ENTER_PRESSED)
			scroll_screen();
		puts("UNKNOWN SCANCODE!\n");
		printf("%d\n", c);
	}
}
