#include "lib.h"

#ifndef _TERMINAL_H
#define _TERMINAL_H

/* output the buffer */
extern int terminal_write(int32_t fd, int32_t nbytes,int enter_flag);

/* write the buffer into terminal buffer */
extern int terminal_read(int32_t fd, const int8_t* buf, int32_t nbytes);

/* do nothing for now */
extern int terminal_open(const uint8_t* filename);

/* do nothing for now */
extern int terminal_close(int32_t fd);

#endif /* _TERMINAL_H */
