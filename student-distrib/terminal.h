#include "lib.h"

#ifndef _TERMINAL_H
#define _TERMINAL_H

#define CHARACTER_BUFFER_SIZE 		128
uint8_t command_buf[CHARACTER_BUFFER_SIZE];

/* set the t_enter_flag to 1 */
extern void set_t_enter_flag();

/* output the buffer */
extern int terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes);

/* write the buffer into terminal buffer */
extern int terminal_read(int32_t fd, int8_t* buf);

/* do nothing for now */
extern int terminal_open(const uint8_t* filename);

/* do nothing for now */
extern int terminal_close(int32_t fd);

#endif /* _TERMINAL_H */
