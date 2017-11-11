#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"

/* store the status of shift/ctrl/capslock/enter/alt/terminal_read/add_to_buffer */
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
    
    /* call terminal write to write buffer to screen */
    if (add_or_not == 1)
    	terminal_write(0,(int8_t*)(char_buf+buf_index) , 1);
    add_or_not = 0;
    if (enter_flag == 1)
    {	
    	buf_index=-1;
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
	int found = -1;

	/* if the scancode is "enter" */
	if(c == PRESS_ENTER) { 
		enter_flag = 1; 
		char_buf[++buf_index] = '\n';

		if(command_buf[0] =='\0')
			for(i=0;i<CHARACTER_BUFFER_SIZE;i++)
			{
				command_buf[i] = char_buf[i]; 
				if(char_buf[i] == '\n')
				{
					set_t_enter_flag();
					break;
				}
			}
		found = 1; 
		add_or_not = 1; 
	}

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

		/* check if the buffer is empty */
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
		/* check the capslock state */
		if (release_caps == 1) {
			capslock_flag ^= 1;
			release_caps = 0;
		}
	}

	/* if the scancode is "release_capslock" */
	if (c == RELEASE_CAPSLOCK) { release_caps = 1; found = 1; }
	
	/* if the scancode is "PRESS_SPACE" */
	if (c == PRESS_SPACE) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2)
		{
			add_or_not = 1;
			char_buf[++buf_index] = ' ';
		}
		found = ' ';
	}

	/* if the scancode is "PRESS_TAB" */
	if (c == PRESS_TAB) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2)
		{
			add_or_not = 1;
			char_buf[++buf_index] = TAB;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_EQUAL" */
	if (c == PRESS_EQUAL) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> plus */
			if(shift_flag)
				char_buf[++buf_index] = PLUS;
			/* if shift not pressed -> equal */
			else
				char_buf[++buf_index] = EQUAL;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_HYPHEN" */
	if (c == PRESS_HYPHEN) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> underscope */
			if(shift_flag)
				char_buf[++buf_index] = UNDERSCOPE;
			/* if shift not pressed	-> hyphen */
			else
				char_buf[++buf_index] = HYPHEN;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_BACKSLASH" */
	if (c == PRESS_BACKSLASH) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> vertical_slash */
			if(shift_flag)
				char_buf[++buf_index] = VERTICAL_SLASH;
			/* if shift not pressed	-> backslash */
			else
				char_buf[++buf_index] = BACKSLASH;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_SINGLE_QUOTE" */
	if (c == PRESS_SINGLE_QUOTE) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> double_quote */
			if(shift_flag)
				char_buf[++buf_index] = DOUBLE_QUOTE;
			/* if shift not pressed -> single_quote */
			else
				char_buf[++buf_index] = SINGLE_QUOTE;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_BACK_TICK" */
	if (c == PRESS_BACK_TICK) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> tilde */
			if(shift_flag)
				char_buf[++buf_index] = TILDE;
			/* if shift not pressed	-> back_tick */
			else
				char_buf[++buf_index] = BACK_TICK;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_COMMA" */
	if (c == PRESS_COMMA) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> less */
			if(shift_flag)
				char_buf[++buf_index] = LESS;
			/* if shift not pressed	-> comma */
			else
				char_buf[++buf_index] = COMMA;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_DOT" */
	if (c == PRESS_DOT) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> greater */
			if(shift_flag)
				char_buf[++buf_index] = GREATER;
			/* if shift not pressed	-> dot */
			else
				char_buf[++buf_index] = DOT;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_SLASH" */
	if (c == PRESS_SLASH) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> question */
			if(shift_flag)
				char_buf[++buf_index] = QUESTION;
			/* if shift not pressed -> slash */
			else
				char_buf[++buf_index] = SLASH;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_SEMICOLON" */
	if (c == PRESS_SEMICOLON) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> colon */
			if(shift_flag)
				char_buf[++buf_index] = COLON;
			/* if shift not pressed -> semicolon */
			else
				char_buf[++buf_index] = SEMICOLON;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_LEFT_SQUARE_BRACKET" */
	if (c == PRESS_LEFT_SQUARE_BRACKET) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> left_curly_bracket */
			if(shift_flag)
				char_buf[++buf_index] = LEFT_CURLY_BRACKET;
			/* if shift bot pressed -> left_square_bracket */
			else
				char_buf[++buf_index] = LEFT_SQUARE_BRACKET;
			add_or_not = 1;
		}
		found = 1;
	}

	/* if the scancode is "PRESS_RIGHT_SQUARE_BRACKET" */
	if (c == PRESS_RIGHT_SQUARE_BRACKET) {
		if(buf_index < CHARACTER_BUFFER_SIZE-2) {
			/* if shift pressed 	-> right_curly_bracket */
			if(shift_flag)
				char_buf[++buf_index] = RIGHT_CURLY_BRACKET;
			/* if shift bot pressed -> right_square_bracket */
			else
				char_buf[++buf_index] = RIGHT_SQUARE_BRACKET;
			add_or_not = 1;
		}
		found = 1;
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
				break;
			}
		}
	}

	/* if the scandcode is neither recognized code nor release code, print "Unknown Scandcode" */
	if(found==-1 && (c>RELEASE_SCANCODE_UPPERBOUND || c<RELEASE_SCANCODE_LOWERBOUND) && c!=ACK_SCANCODE)
	{
		add_or_not = 0;
	}
}
