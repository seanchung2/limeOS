#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include "lib.h"
#include "filesystem.h"
#include "rtc.h"
#include "terminal.h"
#include "paging.h"
#include "x86_desc.h"

#define KERNEL_BOT_ADDR			0x800000
#define FOUR_MB					0x400000
#define LOAD_ADDR				0x08048000
#define USER_VID_MEM_ADD		0x06400000
#define VID 					0xB8000
#define EIGHT_KB				0x2000
#define MAX_FD_NUM				8
#define MAX_LENGTH_ARG 			128
#define BUF_SIZE				4
#define PROGRAM_PDT_INDEX		32
#define PROGRAM_PROPERTIES		0x97
#define MAX_PID					6
#define VIRTUAL_BLOCK_TOP		0x08000000
#define VIRTUAL_BLOCK_BOTTOM	0x08400000
#define ENTRY_POINT_LOCATION	24
#define STDIN_LEN				5
#define STDOUT_LEN				6

extern int32_t pid_flags[MAX_PID];
extern int user_vid_mem_addr[3];
/* for pid status, PCB entries, and jumptable */
enum pidStatus{FREE = 0, IN_USE};
enum jumpTable{OPEN = 0, READ, WRITE, CLOSE};
enum PCB_entry{STDIN = 0, STDOUT};
enum fileType{FILE_TYPE_RTC = 0, FILE_TYPE_DIR, FILE_TYPE_FILE};

/* do nothing, just for padding */
int32_t null_func();

/* initialize the new PCB */
pcb_t* setup_PCB (int32_t new_pid);

/* handle system call "halt" */
int32_t halt (uint8_t status);
int32_t halt_256(uint32_t status);

/* helper function to handle 256 from exception handlers */
int32_t halt_256(uint32_t status);

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

/* handle system call "getargs" */
int32_t getargs (uint8_t * buf , int32_t nbytes);

/* handle system call "vidmap" */
int32_t vidmap (uint8_t** screen_start);

/* handle system call "set_handler" */
int32_t set_handler (int32_t signum, void* handler_address);

/* handle system call "sigreturn" */
int32_t sigreturn (void);

#endif /* _SYSTEM_CALL_H */
