#include "system_call.h"

int user_vid_mem_addr[3] = {0x06400000, 0x06400000+0x1000, 0x06400000+2*0x1000};
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
int32_t (*stdin_table[4])() = {	null_func,
							terminal_read,
							null_func,
							null_func
						};

int32_t (*stdout_table[4])() = {	null_func,
							null_func,
							terminal_write,
							null_func
						};

int32_t (*rtc_table[4])() = { 	open_RTC,
							read_RTC,
							write_RTC,
							close_RTC
						};

int32_t (*file_table[4])() = { 	open_file,
							read_file,
							write_file,
							close_file };

int32_t (*directory_table[4])() = { 	open_directory,
								read_directory,
								write_directory,
								close_directory };


/* setup_PCB (int32_t new_pid)
 *
 * Description: Setup the PCB
 * Inputs: new_pid - New Process ID
 * Outputs: None
 * Return: Initialized PCB
 * Side Effects: As description
 */
pcb_t* setup_PCB (int32_t new_pid)
{
	int i,j;
	/* fetch the pcb in current process */
	if(new_pid <0 || new_pid > MAX_PID)
		return (pcb_t*)-1;

	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (new_pid+1) * EIGHT_KB);

	/* initialize the fd_entry */
	for(i=0; i<MAX_FD_NUM; i++)
	{
		/* free the current PCB block */
		for(j=0;j<4;j++)
			pcb->fd_entry[i].operations_pointer[j] = NULL;
		pcb->fd_entry[i].inode_index = -1;
		pcb->fd_entry[i].file_position = -1;
		pcb->fd_entry[i].flags = FREE;
	}
	/* initial the rest fields in PCB */
	pcb->process_id = new_pid;
	pcb->parent_id = current_pid;
	pcb->parent_esp = 0;
	pcb->parent_ebp = 0;
	pcb->return_value = 0;
	pcb->tty = terminal_num;

	/* automatically setup stdin and stdout */
	for(i=0; i<4; i++)
		pcb->fd_entry[STDIN].operations_pointer[i] = stdin_table[i];
	pcb->fd_entry[STDIN].flags = IN_USE;
	for(i=0; i<4; i++)
		pcb->fd_entry[STDOUT].operations_pointer[i] = stdout_table[i];
	pcb->fd_entry[STDOUT].flags = IN_USE;

	return pcb;
}

/* halt
 *
 * Description: Handler for system call "halt"
 *				'halt' system call, terminates a process, returning the specified value to its parent process.
 * Inputs: status - 8 bit value
 * Outputs: None
 * Return: 0
 * Side Effects: As description
 */
int32_t halt (uint8_t status)
{
	halt_256((uint32_t) status);
	//return a 32-bit value to parent's program
	return 0;
}

/*
* halt_256
*
* Description: Helper function for the handler for system call "halt'
* Input: status - 32 bit value of 'status', expanded value of 8 bit 'status'
* Output: None
* Return: 0 (never actually reaches return instruction)
* Side effect: As description
*/
int32_t halt_256(uint32_t status){
	pcb_t* pcb_halt = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB *(current_pid + 1));				//current PCB, the one to be halted
	pcb_t* pcb_parent = (pcb_t*)(KERNEL_BOT_ADDR - EIGHT_KB * (pcb_halt->parent_id + 1));	//parent PCB
	
	pcb_parent->return_value = status;//updating the return value (status) to parent

	//close any relevant FDs
	uint32_t i;
	for(i = 0; i < MAX_FD_NUM; i++){
		close(i);
	}

	pid_flags[current_pid] = 0;
	//if the last shell, it will execute another shell
	if(current_pid == pcb_halt->parent_id)  {
		execute((uint8_t*)"shell");
	}
	current_pid = pcb_halt->parent_id;

	execute_pid[get_tty()] = current_pid;

	//restore parent paging
	page_table_t* PDT = (page_table_t*)PDT_addr;
	PDT->entries[PROGRAM_PDT_INDEX] = (KERNEL_BOT_ADDR + (FOUR_MB*current_pid)) | PROGRAM_PROPERTIES;

	//flush TLBs
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

	asm volatile (	"movl %0, %%esp;"
					"movl %1, %%ebp;"
						:
						: "g" (pcb_halt->parent_esp),
						  "g" (pcb_halt->parent_ebp)
					);

	asm volatile(	"jmp execute_return;"
					);

	return 0;

}

/*
* execute
* Description: Handler for system call "execute"
*				'execute' system call attempts to load and execute a new program, handing off the processor to the new program until it terminates.
* Input: command - A space separated sequence of words
* Output: None
* Return:
*	-1: If command can not be executed
*	256: If the program dies by an exception
*	0 to 255: If the program halts, as given by the halt()
* Side Effect: As description
*/
int32_t execute (const uint8_t* command){	
	//Null check of cmd
	cli();
	if(command == NULL){
		sti();
		return -1;
	}

	//Variable Initialization
	uint8_t local_arg[MAX_LENGTH_ARG];		//Stores the command after the first space is encountered
	uint8_t file_name[MAX_NAME_LENGTH];		//Stores the command before the first space is encountered
	uint8_t buffer[BUF_SIZE];				//Buffer of size 4, to store 4 bytes

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

	//If no space at all in the command
	if(spaces == 0){
		file_name[i] = '\0';
	}

	//Executable check
	dentry_t program;
	if(read_dentry_by_name((uint8_t*) file_name, &program) == -1){
		sti();
		return -1;
	}

	if(read_data(program.inode_number, 0, buffer, BUF_SIZE) == -1){
		sti();
		return -1;
	}

	//String comparison between buf and magic numbers to make sure that file is executable or not
	if(strncmp((int8_t*)buffer, (int8_t*)magic_number, BUF_SIZE) != 0){
		sti();
		return -1;
	}

	//Paging
	page_table_t* PDT = (page_table_t*)PDT_addr;
	int32_t new_pid;
	//Searching for available pid
	for(i = 0; i < MAX_PID; i++)  {
		if(pid_flags[i] == 0)  {
			break;
		}
	}
	//If no pid available
	if(i >= MAX_PID)  {
		sti();
		return -1;
	}
	pid_flags[i] = 1;
	new_pid = i;


	PDT->entries[PROGRAM_PDT_INDEX] = (KERNEL_BOT_ADDR + (FOUR_MB*i)) | PROGRAM_PROPERTIES;

	//Flush TLBs
	asm volatile (	"movl %%CR3, %%eax;"	
					"andl $0xFFF, %%eax;"
					"addl %0, %%eax;"
					"movl %%eax, %%CR3;"
						:
						: "r" (PDT_addr)
						: "eax", "cc"
					);

	//Getting the entry point
	uint8_t entry_point[BUF_SIZE];//First instruction’s address
	read_data(program.inode_number, ENTRY_POINT_LOCATION, entry_point, BUF_SIZE);
	
	//Copying the entire file into memory starting at virtual address LOAD_ADDR
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
	uint32_t reg_ebp; 
	asm volatile (	"movl %%esp, %0;"
					"movl %%ebp, %1;"
						: "=g" (reg_esp),
						  "=g" (reg_ebp)
					);
	new_process->parent_esp = reg_esp;
	new_process->parent_ebp = reg_ebp;

	new_process->parent_esp0 = tss.esp0;
	tss.ss0 = KERNEL_DS;
	tss.esp0 = (KERNEL_BOT_ADDR - ((new_pid) * EIGHT_KB)) - 1;
	new_process->kernel_stack = (KERNEL_BOT_ADDR - ((new_pid) * EIGHT_KB)) - 1;

	/* copy args to pcb */
	if(local_arg_start_flag)
		strcpy((int8_t*)new_process->args,(int8_t*)local_arg);

	/* setup IRET context */
	uint32_t target_instruction = *((uint32_t*)entry_point);
	uint32_t code_segment = USER_CS;
	uint32_t stack_pointer = VIRTUAL_BLOCK_BOTTOM - 1;
	uint32_t stack_segment = USER_DS;

	execute_pid[terminal_num] = current_pid;
	sti();
	asm volatile (	"pushl %3;"
					"pushl %2;"
					"pushfl;"
					"pushl %1;"
					"pushl %0;"
					"iret;"
					"execute_return:;"
						: 
						: "g" (target_instruction),
						  "g" (code_segment),
						  "g" (stack_pointer),
						  "g" (stack_segment)
				);
	pcb_t* PCB = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);
	int32_t return_val = PCB->return_value;	

	return return_val;
}

/*
* read
* Description: Hanlder for system call "read"
*				'read' system call reads data from the keyboard,a file, device (RTC), or directory.
* Input: 
*		fd: File descriptor
*		buf: Buffer which will be filled out in each read function
*		nbyte: How many bytes we want
* Output: None
* Return: number of bytes copied or -1 for error happened
* Side Effect: As description
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
	int ret;
	//int tty_save;

	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* fd must be between 0-7 */
	if(fd >= MAX_FD_NUM || fd < 0 || fd == 1)
		return -1;

	/* if the fd is not in used, then return -1 */
	if (pcb->fd_entry[fd].flags == FREE)
		return -1;

	//tty_save = terminal_num;
	//terminal_num = get_tty();

	/* call the file's read function */
	ret = (*pcb->fd_entry[fd].operations_pointer[READ])(fd,buf,nbytes);

	//terminal_num = tty_save;

	return ret;
}

/*
* write 
* Description: Hanlder for system call "write"
*				'write' system call writes data to the terminal or to a device (RTC)
* Input: 
*		fd: File descriptor
*		buf: Buffer which will provide data each write function need
*		nbyte: How many bytes we want
* Output: None
* Return: Number of bytes written or -1 for error happened
* Side Effect: As description
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
	int ret;
	int tty_save;

	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	if(buf == NULL)
		return -1;

	/* fd must be between 0-7 */
	if(fd >= MAX_FD_NUM || fd < 0 || fd == 0)
		return -1;
	/* if the fd is not in used, then return -1 */
	if (pcb->fd_entry[fd].flags == FREE)
		return -1;

	cli();
	tty_save = terminal_num;
	terminal_num = get_tty();

	/* call the file's write function */
	ret = (*pcb->fd_entry[fd].operations_pointer[WRITE])(fd,buf,nbytes);

	terminal_num = tty_save;
	sti();

	return ret;
}

/* open
 * 
 * DESCRIPTION:	opens a given file and creates a file entry for
 *				it in the current PCB
 * INPUT: 	filename - pointer to a buffer containing desired
 *					   file's name 
 * OUTPUT: 	fd index for the opened file
 *			-1 - error
 * SIDE EFFECT:	sets up a file entry in the current PCB for the
 *				file
 */
int32_t open (const uint8_t* filename)
{
	dentry_t dentry;
	int i,j;
	int ret;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	if(filename == NULL)
		return -1;

	/* if filename is "stdin" */
	if( strncmp((int8_t*)filename, (int8_t*)"stdin", STDIN_LEN) == 0)
	{
		for(i=0; i<4; i++)
			pcb->fd_entry[STDIN].operations_pointer[i] = stdin_table[i];
		pcb->fd_entry[STDIN].flags = IN_USE;
		return STDIN;
	}

	/* if filename is "stdout" */
	if( strncmp((int8_t*)filename, (int8_t*)"stdout", STDOUT_LEN) == 0)
	{
		for(i=0; i<4; i++)
			pcb->fd_entry[STDOUT].operations_pointer[i] = stdout_table[i];
		pcb->fd_entry[STDOUT].flags = IN_USE;
		return STDOUT;
	}

	/* if filename is not found, return -1 */
	if (read_dentry_by_name(filename, &dentry) < 0)
		return -1;

	/* find a vacant block in PCB to store the opened file */
	for(i=2; i<MAX_FD_NUM; i++)
	{
		/* if found a vacant block */
		if( pcb->fd_entry[i].flags == 0 )
		{
			/* check what kind of file type the new file is,
				then initializing the operation ptr */
			switch(dentry.file_type){
				case FILE_TYPE_RTC:
					for(j=0; j<4; j++)
						pcb->fd_entry[i].operations_pointer[j] = rtc_table[j];
					break;
				case FILE_TYPE_DIR:
					for(j=0; j<4; j++)
						pcb->fd_entry[i].operations_pointer[j] = directory_table[j];
					break;
				case FILE_TYPE_FILE:
					for(j=0; j<4; j++)
						pcb->fd_entry[i].operations_pointer[j] = file_table[j];
					break;

				/* if the file type is unknown, return -1 */	
				default:
					return -1;
			}

			/* initialize the field in fd_entry */
			pcb->fd_entry[i].inode_index = dentry.inode_number;
			pcb->fd_entry[i].file_position = 0;
			pcb->fd_entry[i].flags = IN_USE;

			/* call the file's open function */
			ret = (*pcb->fd_entry[i].operations_pointer[OPEN])(filename);
			if (ret < 0)
				return -1;

			/* return the fd number */
			return i;
		}
	}

	/* if no vacant block found */
	return -1;
}

/* close
 * 
 * DESCRIPTION:	closes the file at the given fd index inside the
 *				current PCB
 * INPUT: 	fd - index of desired file in PCB 
 * OUTPUT: 	0 - file successfully closed
 *			-1 - error
 * SIDE EFFECT:	sets the flag field for the fd entry of the given
 *				index inside the current PCB to 0
 */
int32_t close (int32_t fd)
{
	int ret,i;
	/* fetch the pcb in current process */
	pcb_t * pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* if user try to close stdin/stdout or an invalid fd, return -1 */
	if (fd <2 || fd >= MAX_FD_NUM)
		return -1;
	
	/* if the fd is not in used, then return -1 */
	if (pcb->fd_entry[fd].flags == FREE)
		return -1;

	/* call the file's close function */
	ret = (*(pcb->fd_entry[fd].operations_pointer)[CLOSE])(fd);
	if (ret < 0)
		return -1;

	/* free the current PCB block */
	for(i=0;i<4;i++)
		pcb->fd_entry[fd].operations_pointer[i] = NULL;
	pcb->fd_entry[fd].inode_index = -1;
	pcb->fd_entry[fd].file_position = -1;
	pcb->fd_entry[fd].flags = FREE;

	return 0;
}

/*
 * getargs (uint8_t * buf , int32_t nbytes)
 * handle system call "getargs"
 * input: none
 * output: 
 * side effect: as description
 */
int32_t getargs (uint8_t *buf, int32_t nbytes)
{
	pcb_t *pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	/* if user buf is null, return -1 */
	if (buf==NULL)
		return -1;
	/* if input arguements length is 0, return -1 */
	if (strlen((int8_t*)pcb->args)==0)
		return -1;

	strncpy((int8_t*)buf, (int8_t*)pcb->args, (uint32_t)nbytes);
	(pcb->args)[0] = '\0';
	return 0;
}

/*
 * vidmap (uint8_t** screen_start)
 * handle system call "vidmap"
 * input: none
 * output: 0 success; -1 failure
 * side effect: as description
 */
int32_t vidmap (uint8_t** screen_start)
{
	//Checking validity and error handling
	if((uint32_t) screen_start < VIRTUAL_BLOCK_TOP || (uint32_t) screen_start > VIRTUAL_BLOCK_BOTTOM){
		return -1;
	}

	uint8_t * destination_mem = (uint8_t*)user_vid_mem_addr[terminal_num];//Pre setting virtual address
	*screen_start = destination_mem;

	return 0;
}

/*
 * set_handler (int32_t signum, void* handler_address)
 * handle system call "set_handler"
 * input: none
 * output: return -1 for not supporting set_handler
 * side effect: as description
 */
int32_t set_handler (int32_t signum, void* handler_address){
	return -1;
}

/*
 * sigreturn (void)
 * handle system call "sigreturn"
 * input: none
 * output: return -1 for not supporting set_handler
 * side effect: as description
 */
int32_t sigreturn (void){
	return -1;
}
