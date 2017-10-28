#include "lib.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Definitions for Keyboard*/
#define DEFAULT_PARA 		0xF6
#define KEYBOARD_ACK 		0xFA
#define KEYBOARD_RESEND 	0xFE
#define KEYBOARD_PORT 		0x60
#define L_ORDER				11

/* define some values */
#define KEYBOARD_DATA_PORT 		0x60
#define KEYBOARD_IRQ 			0x1
#define LETTER_NUM 				26
#define NUMBER_NUM 				10
#define LOWER_LETTER_OFFSET 	0x61
#define UPPER_LETTER_OFFSET 	0x41
#define NUMBER_OFFSET 			0x30		
#define CHARACTER_BUFFER_SIZE 	128		
#define PRESS_ENTER 			0x1C
#define PRESS_BACKSPACE			0x0E
#define PRESS_SPACE 			0x39
#define PRESS_LEFT_CTRL			0x1D
//#define PRESS_RIGHT_CTRL		0xE0
#define RELEASE_LEFT_CTRL		0x9D
//#define RELEASE_RIGHT_CTRL		
#define PRESS_LEFT_SHIFT		0x2A
#define PRESS_RIGHT_SHIFT		0x36
#define RELEASE_LEFT_SHIFT		0xAA
#define RELEASE_RIGHT_SHIFT		0xB6
#define PRESS_CAPSLOCK			0x3A
#define RELEASE_CAPSLOCK		0xBA
#define PRESS_SPACE 			0x39


/* some code will be read from keyboard data code */
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

/* output the buffer */
extern int terminal_write();

#endif /* _INT_HANDLER_H */
