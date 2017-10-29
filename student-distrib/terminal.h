#include "lib.h"

#ifndef _TERMINAL_H
#define _TERMINAL_H

/* output the buffer */
extern int terminal_write(int enter_flag);

/* write the buffer into terminal buffer */
extern int terminal_read(const int8_t *);

/* do nothing for now */
extern int terminal_open();

/* do nothing for now */
extern int terminal_close();

#endif /* _TERMINAL_H */
