#include "lib.h"
#include "terminal.h"
#include "i8259.h"
#include "tests.h"
#include "system_call.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Definitions for Keyboard */
#define DEFAULT_PARA 				0xF6
#define KEYBOARD_ACK 				0xFA
#define KEYBOARD_RESEND 			0xFE
#define KEYBOARD_DATA_PORT 			0x60
#define KEYBOARD_IRQ 				0x1
	
/* some define values */
#define TABLE_SIZE					0x3A
#define NUMBER_OFFSET 				0x30		
#define CHARACTER_BUFFER_SIZE 		128
#define ACK_SCANCODE				0xFA
/* keyboard function scancode */
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

const static uint8_t scancodeTable[4][TABLE_SIZE+1] = {
	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'},

	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V', 
	 'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'},

	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V', 
	 'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'},

	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'}
};

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

/* helper function for "keyboard_output_dealer" */
void defaultKeyPressed (uint8_t c);

#endif /* _KEYBOARD_H */
