#include "lib.h"
#include "filesystem.h"

#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H

#define MAX_LENGTH_ARG 128

/* handle system call "halt" */
int32_t halt (uint8_t status);

/* handle system call "execute" */
int32_t execute (const uint8_t* command);

/* handle system call "read" */
int32_t read (int32_t fd, void* buf, int32_t nbytes);

/* handle system call "write" */
int32_t write (int32_t fd, const void* buf, int32_t nbytes);

/* handle system call "open" */
int32_t open (const uint8_t* filename);

/* handle system call "close" */
int32_t close (int32_t fd);

#endif /* _SYSTEM_CALL_H */
