#include "sched.h"

static int terminals_initialized = 0;

void task_switch()  {
//	int i;

	if(pid_flags[0] == FREE)  {
		printf("starting terminal 0\n");
		current_pid = 0;
		terminal_num = 0;
		send_eoi(0);
		sti();
		execute((uint8_t*)"shell");
	}

	if(pid_flags[1] == FREE)  {
		printf("starting terminal 1\n");
		current_pid = 1;
		terminal_num = 1;
		send_eoi(0);
		sti();
		execute((uint8_t*)"shell");
	}

	if(pid_flags[2] == FREE)  {
		printf("starting terminal 2\n");
		current_pid = 2;
		terminal_num = 2;
		send_eoi(0);
		sti();
		terminals_initialized = 1;
		execute((uint8_t*)"shell");
	}

	if(terminals_initialized)  {
		terminal_num = 0;
		terminals_initialized = 0;
	}
/*
	for(i = 1; i <= MAX_PID; i++)  {
		if(pid_flags[(i+current_pid) % MAX_PID] == IN_USE)  {
			pcb_t* pcb_current = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB *(current_pid + 1));
			pcb_t* pcb_next = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB * (i + 1));
		}

			uint32_t reg_esp;
			uint32_t reg_ebp; 
			asm volatile (	"movl %%esp, %0;"
							"movl %%ebp, %1;"
								: "=g" (reg_esp),
								  "=g" (reg_ebp)
							);
			pcb_current->
*/
}
