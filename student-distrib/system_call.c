#include "system_call.h"

/* array of possible pid's */
int32_t pid_flags[MAX_PID];

/* Do nothing, return 0 */
int32_t null_func()  {
	return 0;
}

/*The first 4 bytes of the file represent a magic number that identies the file as an exeutable.*/
const int8_t magic_number[4] = {0x7f, 0x45, 0x4c, 0x46};

/*
 * jump table:
 * open function
 * read function	
 * write function
 * close function
 */
int32_t stdin_table[4] = { (uint32_t)(null_func),
							(uint32_t)(terminal_read),
							(uint32_t)(null_func),
							(uint32_t)(null_func) };

int32_t stdout_table[4] = {(uint32_t)(null_func),
							(uint32_t)(null_func),
							(uint32_t)(terminal_write),
							(uint32_t)(null_func) };

int32_t rtc_table[4] = { 	(uint32_t)(open_RTC),
							(uint32_t)(read_RTC),
							(uint32_t)(write_RTC),
							(uint32_t)(close_RTC) };

int32_t file_table[4] = { 	(uint32_t)(open_file),
							(uint32_t)(read_file),
							(uint32_t)(write_file),
							(uint32_t)(close_file) };

int32_t directory_table[4] = { (uint32_t)(open_directory),
								(uint32_t)(read_directory),
								(uint32_t)(write_directory),
								(uint32_t)(close_directory) };

/*
* setup_PCB ()
* initialize a new PCB for a new process
* input: none
* output: none
* side effect: as description
*/
pcb_t* setup_PCB (int32_t new_pid)
{
	int i;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (new_pid+1) * EIGHT_KB);

	/* initialize the fd_entry */
	for(i=0; i<MAX_FD_NUM; i++)
	{
		pcb->fd_entry[i].operations_pointer = NULL;
		pcb->fd_entry[i].inode_index = -1;
		pcb->fd_entry[i].file_position = -1;
		pcb->fd_entry[i].flags = FREE;
	}
	/* initial the rest fields in PCB */
	pcb->process_id = new_pid;
	pcb->parent_id = current_pid;
	pcb->child_id = -1;

	/* automatically setup stdin and stdout */
	pcb->fd_entry[STDIN].operations_pointer = stdin_table;
	pcb->fd_entry[STDIN].flags = IN_USE;
	pcb->fd_entry[STDOUT].operations_pointer = stdout_table;
	pcb->fd_entry[STDOUT].flags = IN_USE;

	return pcb;
}
/*
* halt (uint8_t status)
* hanlder for system call "halt"
* input: none
* output: none
* side effect: as description
*/
int32_t halt (uint8_t status)
{
	halt_256((uint32_t) status);
	//return a 32-bit value to parent's program
	return 0;
}

int32_t halt_256(uint32_t status){
	pcb_t* pcb_halt = KERNEL_BOT_ADDR - EIGHT_KB *(current_pid + 1);//current PCB
	//May need to look into parent-child flag thing... kind of a parent-child relationship

	//restore parent data
	pcb_t* pcb_parent = KERNEL_BOT_ADDR - EIGHT_KB * (pcb_halt->parent_id + 1);//parent PCB ????
	pcb_parent->return_value = status;


	//restore parent paging
	page_table_t* PDT = (page_table_t*)PDT_addr;
	PDT->entries[PROGRAM_PDT_INDEX] = (KERNEL_BOT_ADDR + (FOUR_MB*(pcb_halt->parent_id))) | PROGRAM_PROPERTIES;

	//close any relevant FDs
	uint32_t i;
	for(i = 0; i < 8; i++){
		pcb_halt->fd_entry[i].flags = 0;
	}


	/*flush TLBs */
	asm volatile (	"movl %%CR3, %%eax;"	
					"andl $0xFFF, %%eax;"
					"addl %0, %%eax;"
					"movl %%eax, %%CR3;"
						:
						: "r" (PDT_addr)
						: "eax", "cc"
					);


	tss.ss0 = KERNEL_DS;
	tss.esp0 = pcb_halt->parent_esp0;

	uint32_t target_instruction = pcb_halt->return_instruction + 1;
	uint32_t code_segment = USER_CS;
	uint32_t stack_pointer = VIRTUAL_BLOCK_BOTTOM - 4;//Need to change this! ????
	uint32_t stack_segment = USER_DS;

	asm volatile (	"pushl %4;"
					"pushl %3;"
					"pushfl;"
					"pushl %2;"
					"pushl %1;"
					"movl %%eip, %0"
					"iret"
						: "=g" (PCB->return_instruction)
						: "g" (target_instruction),
						  "g" (code_segment),
						  "g" (stack_pointer),
						  "g" (stack_segment)
				);

	return 0;

}
/*
* execute (const uint8_t* command)
* hanlder for system call "execute"
* input: none
* output: none
* return:
*	-1: If command can not be executed
*	256: If the program dies by an exception
	0 to 255: If the program halts, as given by the halt()
* side effect: as description
*/
int32_t execute (const uint8_t* command){
	
	//null check of cmd
	if(command == NULL){
		return -1;
	}
	//Variable Initialization
	uint8_t file_name[MAX_NAME_LENGTH];
	uint8_t local_arg[MAX_LENGTH_ARG];
	uint8_t buffer[BUF_SIZE];

	uint32_t file_length = 0;
	uint32_t spaces = 0;
	uint32_t i = 0;
	uint32_t end_of_file_name = 0;
	uint32_t local_arg_start_flag = 0;
	//Parsing
	while(command[i] != '\0'){
		if(command[i] != ' ' && spaces == 0){
			file_name[i] = command[i];
		}
		else{
			end_of_file_name++;
			if(command[i] == ' ' && local_arg_start_flag == 0){
				spaces++;
			}
			if(end_of_file_name == 1){
				file_length = i;
				file_name[i] = '\0';
				end_of_file_name++;//Temp fix... Just to be on a safer side
			}
			if(command[i] != ' ' || local_arg_start_flag == 1){
				local_arg_start_flag = 1;
				local_arg[i - (file_length + spaces)] = command[i];
			}
		}
		i++;
	}

	local_arg[i - (file_length + spaces)] = '\0';
	if(spaces == 0){
		file_name[i] = '\0';
	}

	//Executable check
	
	/*read_file check*/
	dentry_t program;
	if(read_dentry_by_name((uint8_t*) file_name, &program) == -1){
		return -1;
	}

	if(read_data(program.inode_number, 0, buffer, 4) == -1){
		return -1;
	}

	/*string comparison between buf and magic numbers to make sure that file is executable or not*/
	if(strncmp((int8_t*)buffer, (int8_t*)magic_number, 4) != 0){
		return -1;
	}

	//Paging
	page_table_t* PDT = (page_table_t*)PDT_addr;
	int32_t new_pid;
	for(i = 0; i < MAX_PID; i++)  {
		if(pid_flags[i] == 0)  {
			break;
		}
	}

	if(i >= MAX_PID)  {
		return -1;
	}
	pid_flags[i] = 1;
	new_pid = i;


	PDT->entries[PROGRAM_PDT_INDEX] = (KERNEL_BOT_ADDR + (FOUR_MB*i)) | PROGRAM_PROPERTIES;

	/*flush TLBs */
	asm volatile (	"movl %%CR3, %%eax;"	
					"andl $0xFFF, %%eax;"
					"addl %0, %%eax;"
					"movl %%eax, %%CR3;"
						:
						: "r" (PDT_addr)
						: "eax", "cc"
					);

	/*Getting the entry point*/
	uint8_t entry_point[BUF_SIZE];//first instruction’s address
	read_data(program.inode_number, 24, entry_point, 4);
	
	/*Copying the entire file into meemory starting at virtual address LOAD_ADDR*/
	uint32_t j;
	uint32_t c;

	i = 0;
	while((c = read_data(program.inode_number, i*BUF_SIZE, buffer, BUF_SIZE)) > 0){
		for(j = 0; j < c; j++)  {
			*((uint8_t*)(LOAD_ADDR + (i*BUF_SIZE)) + j) = buffer[j];
		}
		i++;
	}

	pcb_t *new_process = setup_PCB(new_pid);
	current_pid = new_pid;

	uint32_t reg_esp;//Kernel stack pointer stored 
	asm volatile (	"movl %%esp, %0;"
						: "=g" (reg_esp)
					);
	new_process->parent_esp0 = reg_esp;

	tss.ss0 = KERNEL_DS;
	tss.esp0 = (KERNEL_BOT_ADDR - ((new_pid) * EIGHT_KB)) - 4;


	/* setup IRET context */
	uint32_t target_instruction = *((uint32_t*)entry_point);
	uint32_t code_segment = USER_CS;
	uint32_t stack_pointer = VIRTUAL_BLOCK_BOTTOM - 4;
	uint32_t stack_segment = USER_DS;

	asm volatile (	"pushl %4;"
					"pushl %3;"
					"pushfl;"
					"pushl %2;"
					"pushl %1;"
					"movl %%eip, %0"
					"iret"
						: "=g" (PCB->return_instruction)
						: "g" (target_instruction),
						  "g" (code_segment),
						  "g" (stack_pointer),
						  "g" (stack_segment)
				);

	return 0;
}

/*
* read (int32_t fd, void* buf, int32_t nbytes)
* hanlder for system call "read"
* input: 
*		fd: file descriptor
*		buf: buffer which will be filled out in each read function
*		nbyte: how many bytes we want
* output: number of bytes copied or -1 for error happened
* side effect: as description
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
	int ret;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* fd must be between 0-7 */
	if(fd >= MAX_FD_NUM || fd < 0)
		return -1;

	/* call the file's read function */
	asm volatile("pushl	%4;"
				 "pushl	%3;"
				 "pushl	%2;"
				 "call  *%1;"
		 		 "movl 	%%eax,%0;"
		 		 "addl	$12,%%esp;"
		 		 : "=r"(ret)
		 		 : "g" (pcb->fd_entry[fd].operations_pointer[READ]),
		 		   "g" (fd),
		 		   "g" (buf),
		 		   "g" (nbytes)
		 		 : "eax", "cc");
	return ret;
}

/*
* write (int32_t fd, const void* buf, int32_t nbytes)
* hanlder for system call "write"
* input: 
*		fd: file descriptor
*		buf: buffer which will provide data each write function need
*		nbyte: how many bytes we want
* output: number of bytes written or -1 for error happened
* side effect: as description
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
	int ret;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* fd must be between 0-7 */
	if(fd >= MAX_FD_NUM || fd < 0)
		return -1;
	
	/* call the file's read function */
	asm volatile("pushl	%4;"
				 "pushl	%3;"
				 "pushl	%2;"
				 "call  *%1;"
		 		 "movl 	%%eax, %0;"
		 		 "addl	$12, %%esp;"
		 		 : "=r"(ret)
		 		 : "g" (pcb->fd_entry[fd].operations_pointer[WRITE]),
		 		   "g" (fd),
		 		   "g" (buf),
		 		   "g" (nbytes)
		 		 : "eax", "cc");
	return ret;
}

/*
* open (const uint8_t* filename)
* hanlder for system call "open"
* input: filename is file name 
* output: fd value or -1 for error
* side effect: as description
*/
int32_t open (const uint8_t* filename)
{
	dentry_t* dentry;
	int i;
	int ret;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* if filename is "stdin" */
	if( strncmp((int8_t*)filename, (int8_t*)"stdin", 5) == 0)
	{
		pcb->fd_entry[STDIN].operations_pointer = stdin_table;
		pcb->fd_entry[STDIN].flags = IN_USE;
		return STDIN;
	}

	/* if filename is "stdout" */
	if( strncmp((int8_t*)filename, (int8_t*)"stdout", 6) == 0)
	{
		pcb->fd_entry[STDOUT].operations_pointer = stdout_table;
		pcb->fd_entry[STDOUT].flags = IN_USE;
		return STDOUT;
	}

	/* if filename is not found, return -1 */
	if (read_dentry_by_name(filename, dentry) < 0)
		return -1;

	/* find a vacant block in PCB to store the opened file */
	for(i=2; i<MAX_FD_NUM; i++)
	{
		/* if found a vacant block */
		if( pcb->fd_entry[i].flags == 0 )
		{
			/* check what kind of file type the new file is,
				then initializing the operation ptr */
			switch(dentry->file_type){
				case FILE_TYPE_RTC:
					pcb->fd_entry[i].operations_pointer = rtc_table;
					break;
				case FILE_TYPE_DIR:
					pcb->fd_entry[i].operations_pointer = directory_table;
					break;
				case FILE_TYPE_FILE:
					pcb->fd_entry[i].operations_pointer = file_table;
					break;

				/* if the file type is unknown, return -1 */	
				default:
					return -1;
			}

			/* initialize the field in fd_entry */
			pcb->fd_entry[i].inode_index = dentry->inode_number;
			pcb->fd_entry[i].file_position = 0;
			pcb->fd_entry[i].flags = IN_USE;

			/* call the file's open function */
			asm volatile("pushl	%2;"
						 "call  *%1;"
				 		 "movl 	%%eax,%0;"
				 		 "addl	$4,%%esp;"
				 		 : "=r"(ret)
				 		 : "g" (pcb->fd_entry[i].operations_pointer[OPEN]),
				 		   "g" (filename)
				 		 : "eax", "cc");
			if (ret < 0)
				return -1;

			/* return the fd number */
			return i;
		}
	}

	/* if no vacant block found */
	return -1;
}

/*
* close (int32_t fd)
* hanlder for system call "close"
* input: fd is file descriptor 
* output: 
*			0: successfully closed
*			-1: error
* side effect: as description
*/
int32_t close (int32_t fd)
{
	int ret;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* if user try to close stdin/stdout or an invalid fd, return -1 */
	if (fd <2 || fd >= MAX_FD_NUM)
		return -1;
	
	/* if the fd is not in used, then return -1 */
	if (pcb->fd_entry[fd].flags == FREE)
		return -1;

	/* call the file's close function */
	asm volatile("pushl	%2;"
				 "call  *%1;"
		 		 "movl 	%%eax,%0;"
		 		 "addl	$4,%%esp;"
		 		 : "=r"(ret)
		 		 : "g" (pcb->fd_entry[fd].operations_pointer[CLOSE]),
		 		   "g" (fd)
		 		 : "eax");
	if (ret < 0)
		return -1;

	/* free the current PCB block */
	pcb->fd_entry[fd].operations_pointer = NULL;
	pcb->fd_entry[fd].inode_index = -1;
	pcb->fd_entry[fd].file_position = -1;
	pcb->fd_entry[fd].flags = IN_USE;

	return 0;
}
