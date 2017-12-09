#include "sched.h"

static int terminals_initialized = 0;
int pit_terminal_num = 0;

/* task_switch
 *
 * Description: Do the task switching
 * Inputs: None
 * Outputs: None
 * Side Effects: Page table changed
 */
void task_switch()  {
	int i;
	uint32_t reg_esp;
	uint32_t reg_ebp;

	pcb_t* pcb_current = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB *(current_pid + 1));

	/* store the esp and ebp first */
	asm volatile (	"movl %%esp, %0;"
					"movl %%ebp, %1;"
						: "=g" (reg_esp),
						  "=g" (reg_ebp)
					);
	pcb_current->sched_ebp = reg_ebp;
	pcb_current->sched_esp = reg_esp;
	pcb_current->sched_esp0 = tss.esp0;

	/* if there is no process in the first terminal, start a shell */
	for(i = 0; i < TERMINAL_COUNT; i++)
		if(pid_flags[i] == FREE)  {
			current_pid = i;
			terminal_num = i;
			send_eoi(PIT_IRQ);
			sti();

			if(i == TERMINAL_COUNT - 1) //last terminal to start
				terminals_initialized = 1;

			execute((uint8_t*)"shell");
		}

	/* if three shells in each terminal has started, change the current terminal to default (first) one */
	if(terminals_initialized)  {
		terminal_num = 0;
		terminals_initialized = 0;
	}

	/* increment the "pit_terminal_num" by one to know which terminal should switch to */
	pit_terminal_num = (pit_terminal_num+1)%TERMINAL_COUNT;

	/**/
	current_pid = execute_pid[pit_terminal_num];
	pcb_t* pcb_next = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB * (current_pid + 1));

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

	tss.ss0 = KERNEL_DS;
	tss.esp0 = pcb_next->sched_esp0;

	reg_ebp = pcb_next->sched_ebp;
	reg_esp = pcb_next->sched_esp;
	asm volatile (	"movl %0, %%esp;"
					"movl %1, %%ebp;"
						:
						: "g" (reg_esp),
						  "g" (reg_ebp)
					);
	update_cursor(screen_x[terminal_num], screen_y[terminal_num]);
	sti();
	return;
}

