#include "lib.h"
#include "terminal.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Definitions for Keyboard */
#define DEFAULT_PARA 				0xF6
#define KEYBOARD_ACK 				0xFA
#define KEYBOARD_RESEND 			0xFE
#define L_ORDER						11
#define KEYBOARD_DATA_PORT 			0x60
#define KEYBOARD_IRQ 				0x1
	
/* some define values */
#define LETTER_NUM 					26
#define NUMBER_NUM 					10
#define LOWER_LETTER_OFFSET 		0x61
#define UPPER_LETTER_OFFSET 		0x41
#define NUMBER_OFFSET 				0x30		
#define CHARACTER_BUFFER_SIZE 		128
#define RELEASE_SCANCODE_UPPERBOUND 0xB9
#define RELEASE_SCANCODE_LOWERBOUND 0x81
#define ACK_SCANCODE				0xFA
/* keyboard character scancode */
#define PRESS_ENTER 				0x1C
#define PRESS_BACKSPACE				0x0E
#define PRESS_SPACE 				0x39
#define PRESS_LEFT_CTRL				0x1D
#define RELEASE_LEFT_CTRL			0x9D	
#define PRESS_LEFT_SHIFT			0x2A
#define PRESS_RIGHT_SHIFT			0x36
#define RELEASE_LEFT_SHIFT			0xAA
#define RELEASE_RIGHT_SHIFT			0xB6
#define PRESS_CAPSLOCK				0x3A
#define RELEASE_CAPSLOCK			0xBA
#define PRESS_ALT 					0x38
#define RELEASE_ALT					0xB8

#define PRESS_TAB 					0x0F
#define PRESS_SPACE 				0x39
#define PRESS_EQUAL					0x0D
#define PRESS_HYPHEN				0x0C
#define PRESS_BACKSLASH				0x2B
#define PRESS_SINGLE_QUOTE			0x28
#define PRESS_BACK_TICK				0x29
#define PRESS_COMMA					0x33
#define PRESS_DOT					0x34
#define PRESS_SLASH 				0x35
#define PRESS_SEMICOLON				0x27
#define PRESS_LEFT_SQUARE_BRACKET	0x1A
#define PRESS_RIGHT_SQUARE_BRACKET	0x1B

/* ascii code variables and tables */
#define TAB   						9
#define TILDE   					0x7E
#define BACK_TICK   				0x60
#define UNDERSCOPE   				0x5F
#define HYPHEN   					0x2D
#define PLUS   						0x2B
#define LEFT_CURLY_BRACKET   		0x7B
#define RIGHT_CURLY_BRACKET   		0x7D
#define EQUAL   					0x3D
#define LEFT_SQUARE_BRACKET   		0x5B
#define RIGHT_SQUARE_BRACKET 		0x5D
#define VERTICAL_SLASH   			0x7C
#define COLON   					0x3A
#define SEMICOLON   				0x3B
#define DOUBLE_QUOTE   				0x22
#define SINGLE_QUOTE   				0x27
#define LESS		   				0x3C
#define GREATER		   				0x3E
#define COMMA   					0x2C
#define DOT   						0x2E
#define QUESTION   					0x3F
#define SLASH   					0x2F
#define BACKSLASH   				0x5C
/*
static const uint8_t *special_character[50][3];
special_character[0][0] = {0x0F, 0x09};			// Tab
special_character[1][0] = {0x39, 0x20};			// Space
special_character[2][0] = {0x0D, 0x3D, 0x2B};	// Equal, Plus
special_character[3][0] = {0x0C, 0x2D, 0x5F};	// Dash, Underscope
special_character[4][0] = {0x2B, 0x5C, 0x7C};	// Backslash, Vertical slash
special_character[5][0] = {0x0D, 0x3D, 0x2B};	// Equal, Plus
special_character[6][0] = {0x0D, 0x3D, 0x2B};	// Equal, Plus
special_character[7][0] = {0x0D, 0x3D, 0x2B};	// Equal, Plus
*/
static const uint8_t letter_code[LETTER_NUM] = {	0x1E, 0x30, 0x2E, 0x20,
													0x12, 0x21, 0x22, 0x23,
													0x17, 0x24, 0x25, 0x26,
													0x32, 0x31, 0x18, 0x19,
													0x10, 0x13, 0x1F, 0x14,
													0x16, 0x2F, 0x11, 0x2D,
													0x15, 0x2C};
static const uint8_t number_code[NUMBER_NUM] = {	0x0B, 0x02, 0x03, 0x04, 0x05,
													0x06, 0x07, 0x08, 0x09, 0x0A};

static const uint8_t sign_asciicode[NUMBER_NUM] = {	0x29, 0x21, 0x40, 0x23, 0x24,
													0x25, 0x5E, 0x26, 0x2A, 0x28};
/* initialize_keyboard
 *
 * Description: Initializes the Keyboard
 * Inputs: none
 * Outputs: none
 * Side Effects:
 */
extern void initialize_keyboard();

/* keyboard interrupt asm wrapper */
extern void keyboard_handler ();

/* deal with output of keyboard */
extern void keyboard_output_dealer (uint8_t c);

#endif /* _KEYBOARD_H */
