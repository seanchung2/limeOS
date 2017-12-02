#include "lib.h"

#ifndef _TERMINAL_H
#define _TERMINAL_H

#define CHARACTER_BUFFER_SIZE 		128
uint8_t command_buf[3][CHARACTER_BUFFER_SIZE];

/* current terminal number */
extern int terminal_num;

/* set the t_enter_flag to 1 */
extern void set_t_enter_flag();

/* output the buffer */
extern int32_t terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes);

/* write the buffer into terminal buffer */
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

/* do nothing for now */
extern int32_t terminal_open(const uint8_t* filename);

/* do nothing for now */
extern int32_t terminal_close(int32_t fd);

/* set the terminal number */
extern void set_terminal_num(int i);

#endif /* _TERMINAL_H */
