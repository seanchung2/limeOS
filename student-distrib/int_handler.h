#include "lib.h"

#ifndef _INT_HANDLER_H
#define _INT_HANDLER_H

/* define some values */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_IRQ 0x1
#define LETTER_NUM 26
#define NUMBER_NUM 10
#define LETTER_OFFSET 0x61
#define NUMBER_OFFSET 0x30				

/* System call interrupt asm wrapper */
extern void sys_handler();

/* keyboard interrupt asm wrapper */
extern void keyboard_handler ();

/* deal with output of keyboard */
extern void keyboard_output_dealer (uint8_t c);

#endif /* _INT_HANDLER_H */
