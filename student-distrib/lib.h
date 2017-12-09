/* lib.h - Defines for useful library functions
 * vim:ts=4 noexpandtab
 */

#ifndef _LIB_H
#define _LIB_H

#include "types.h"
#include "i8259.h"


/* self-defined variables */
extern int RTC_STATUS;              // for test
#define SCROLL_ENTER_PRESSED    1   // for shifting the screen
#define SCROLL_LAST_LETTER      2   // for shifting the screen
#define VIDEO           0xB8000
#define VID_BACKUP_1    0xB9000
#define VID_BACKUP_2    0xBA000
#define VID_BACKUP_3    0xBB000
#define NUM_COLS        80
#define NUM_ROWS        25
#define ATTRIB          0x7
#define CUR_LPORT       0xF
#define CUR_HPORT       0xE
#define VGA_INDEX_REG   0x3D4
#define VGA_DATA_REG    0x3D5
#define KERNEL_BOT_ADDR     0x800000
#define MAX_LENGTH_ARG          128
#define EIGHT_KB            0x2000

/* current terminal number */
extern int terminal_num;
extern int32_t current_pid;
extern char* video_mem[3];
extern int screen_x[3];
extern int screen_y[3];
extern int execute_pid[3];

int32_t printf(int8_t *format, ...);
void putc(uint8_t c);
int32_t puts(int8_t *s);
int8_t *itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t *strrev(int8_t* s);
uint32_t strlen(const int8_t* s);
void clear();

void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strcpy(int8_t* dest, const int8_t*src);
int8_t* strncpy(int8_t* dest, const int8_t*src, uint32_t n);

/* added by limeOS */
void test_interrupts(void);
void reset_screen();
void backspace_pressed();
void update_cursor(int x, int y);
int check_out_of_bound();
void scroll_screen();
uint8_t get_tty();

/* Userspace address-check functions */
int32_t bad_userspace_addr(const void* addr, int32_t len);
int32_t safe_strncpy(int8_t* dest, const int8_t* src, int32_t n);

/* entry in file array of PCB */
typedef struct fd_entry  {
    int32_t (*operations_pointer[4])();
    int32_t inode_index;
    int32_t file_position;
    int32_t flags;
} fd_entry_t;

/* structure for PCB */
typedef struct process_control_block  {
    fd_entry_t fd_entry[8];
    uint32_t process_id;                // pid of this pcb
    uint32_t parent_id;                 // pid of the parent process of this pcb 
    uint32_t parent_esp;                // parent's esp (used to restore esp when halt this process)
    uint32_t parent_ebp;                // parent's ebp (used to restore esp when halt this process)
    uint32_t return_value;              // used to store the return value
    uint32_t parent_esp0;               // parent's esp0 (used to restore esp when halt this process)
    uint8_t args[MAX_LENGTH_ARG];       // store the arguments from the terminal input
    uint32_t sched_ebp;                 // used to store/restore ebp using in task switching 
    uint32_t sched_esp;                 // used to store/restore esp using in task switching
    uint32_t sched_esp0;                // used to store/restore esp0 using in task switching
    uint8_t tty;                        // the terminal number currently on the screen
} pcb_t;

/* Port read functions */
/* Inb reads a byte and returns its value as a zero-extended 32-bit
 * unsigned int */
static inline uint32_t inb(port) {
    uint32_t val;
    asm volatile ("             \n\
            xorl %0, %0         \n\
            inb  (%w1), %b0     \n\
            "
            : "=a"(val)
            : "d"(port)
            : "memory"
    );
    return val;
}

/* Reads two bytes from two consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them zero-extended
 * */
static inline uint32_t inw(port) {
    uint32_t val;
    asm volatile ("             \n\
            xorl %0, %0         \n\
            inw  (%w1), %w0     \n\
            "
            : "=a"(val)
            : "d"(port)
            : "memory"
    );
    return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(port) {
    uint32_t val;
    asm volatile ("inl (%w1), %0"
            : "=a"(val)
            : "d"(port)
            : "memory"
    );
    return val;
}

/* Writes a byte to a port */
#define outb(data, port)                \
do {                                    \
    asm volatile ("outb %b1, (%w0)"     \
            :                           \
            : "d"(port), "a"(data)      \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                \
do {                                    \
    asm volatile ("outw %w1, (%w0)"     \
            :                           \
            : "d"(port), "a"(data)      \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                \
do {                                    \
    asm volatile ("outl %l1, (%w0)"     \
            :                           \
            : "d"(port), "a"(data)      \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                           \
do {                                    \
    asm volatile ("cli"                 \
            :                           \
            :                           \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Save flags and then clear interrupt flag
 * Saves the EFLAGS register into the variable "flags", and then
 * disables interrupts on this processor */
#define cli_and_save(flags)             \
do {                                    \
    asm volatile ("                   \n\
            pushfl                    \n\
            popl %0                   \n\
            cli                       \n\
            "                           \
            : "=r"(flags)               \
            :                           \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                           \
do {                                    \
    asm volatile ("sti"                 \
            :                           \
            :                           \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Restore flags
 * Puts the value in "flags" into the EFLAGS register.  Most often used
 * after a cli_and_save_flags(flags) */
#define restore_flags(flags)            \
do {                                    \
    asm volatile ("                   \n\
            pushl %0                  \n\
            popfl                     \n\
            "                           \
            :                           \
            : "r"(flags)                \
            : "memory", "cc"            \
    );                                  \
} while (0)

#endif /* _LIB_H */
