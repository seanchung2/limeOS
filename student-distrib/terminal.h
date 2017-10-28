#include "lib.h"

#ifndef _TERMINAL_H
#define _TERMINAL_H

/* output the buffer */
extern int terminal_write(int enter_flag);

/* write the buffer into terminal buffer */
extern int terminal_read(const int8_t *);

#endif /* _TERMINAL_H */
