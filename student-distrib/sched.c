#include "sched.h"

static int terminals_initialized = 0;

void task_switch()  {
	int i;
	uint32_t reg_esp;
	uint32_t reg_ebp;

	pcb_t* pcb_current = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB *(current_pid + 1));
	asm volatile (	"movl %%esp, %0;"
					"movl %%ebp, %1;"
						: "=g" (reg_esp),
						  "=g" (reg_ebp)
					);
	pcb_current->sched_ebp = reg_ebp;
	pcb_current->sched_esp = reg_esp;
	pcb_current->sched_esp0 = tss.esp0;

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

	for(i = 1; i <= MAX_PID; i++)  {
		if(pid_flags[(i+current_pid) % MAX_PID] == IN_USE)  {
			current_pid = (current_pid + i) % MAX_PID;
			pcb_t* pcb_next = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB * (current_pid + 1));

			reg_ebp = pcb_next->sched_ebp;
			reg_esp = pcb_next->sched_esp;
			asm volatile (	"movl %0, %%esp;"
							"movl %1, %%ebp;"
								:
								: "g" (reg_esp),
								  "g" (reg_ebp)
							);

			tss.ss0 = KERNEL_DS;
			tss.esp0 = pcb_next->sched_esp0;

			page_table_t* PDT = (page_table_t*)PDT_addr;
			PDT->entries[PROGRAM_PDT_INDEX] = (KERNEL_BOT_ADDR + (FOUR_MB*current_pid)) | PROGRAM_PROPERTIES;

			asm volatile (	"movl %%CR3, %%eax;"	
							"andl $0xFFF, %%eax;"
							"addl %0, %%eax;"
							"movl %%eax, %%CR3;"
								:
								: "r" (PDT_addr)
								: "eax", "cc"
							);

			sti();
			return;
		}
	}
}
