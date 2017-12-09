
#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "paging.h"


#define CHARACTER_BUFFER_SIZE 		128
#define VIDEO_MEMORY				0xB8000
uint8_t command_buf[3][CHARACTER_BUFFER_SIZE];

extern int backup_mem_addr[3];  

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

extern void terminal_switch(int new_tty);

#endif /* _TERMINAL_H */
