#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "system_wrapper.h"
#include "i8259.h"
#include "paging.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesystem.h"


#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* 
 * type_tester(char c)
 * test the exceptions, rtc, and keyboard decided
 *				by input "c" 
 * Inputs: c: character sent by int_handler.c
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: idt.c, int_handler.c
 */
void type_tester(char c)  {
	/* if receive '0', trigger devide by zero exception */
	if(c == '0')  {
		printf("\n");
		divide_zero_test();
	}

	/* if receive 'n', trigger dereference null exception */
	if(c == 'n')  {
		printf("\n");
		deref_null_test();
	}

	/* if receive 'r', toggle the RTC */
	if(c == 'r')  {
		if(RTC_STATUS == 0) {
			RTC_STATUS = 1;
		}
		else  {
			RTC_STATUS = 0;
		}
	}
}

/* Divide by 0 test
 *
 * Attempts to divide by zero
 * Inputs: none
 * Outputs: none
 * Side Effects: freezes kernel
 * Coverage: IDT exception handler
 * Files: idt.c
 */
void divide_zero_test()  {
	TEST_HEADER;

	int i = 1;
	int j = 0;

	int k = i/j;
	k = 1;
}

/* Dereference NULL test
 *
 * Attempts to dereference NULL
 * Inputs: none
 * Outputs: none
 * Side Effects: freezes kernel
 * Coverage: IDT exception handler
 * Files: idt.c
 */
void deref_null_test()  {
	TEST_HEADER;

	int* ref = NULL;
	int i;

	i = *(ref);
}

/* System Call test
 *
 * Attempts to call system call
 * Inputs: none
 * Outputs: PASS/FAIL
 * Side Effects: none
 * Coverage: IDT system call handler
 * Files: int_handler.c
 */
int syscall_test()
{
	TEST_HEADER;
	int result = PASS;
	asm volatile("int $0x80");
	return result;
}

/* Paging test kernel
 *
 * Writes into valid kernek memory and reads from same
 * address to check if paging is working corecctly
 * Input: none
 * Output: PASS/FAIL
 * Side Effects: none
 * Coverage: Pages that are defined in memory
 * Files: paging.c
 */
int paging_test_kernel()  {
	TEST_HEADER;

	int test = 0;
	int* test_ptr = (int*)0x401000;
	int init_value = *test_ptr;
	*test_ptr = 25;
	test = *test_ptr;
	if(test != 25)  {
		*test_ptr = init_value;
		return FAIL;
	}
	*test_ptr = init_value;
	return PASS;
}

/* Paging test video
 *
 * Writes into valid video memory and reads from same
 * address to check if paging is working corecctly
 * Input: none
 * Output: PASS/FAIL
 * Side Effects: none
 * Coverage: Pages that are defined in memory
 * Files: paging.c
 */
int paging_test_vidmem()  {
	TEST_HEADER;

	int test = 0;
	int* test_ptr = (int*)0xB8010;
	int init_value = *test_ptr;
	*test_ptr = 25;
	test = *test_ptr;
	if(test != 25)  {
		*test_ptr = init_value;
		return FAIL;
	}
	*test_ptr = init_value;
	return PASS;
}


/* paging_value_test
 *
 * Test whether the paging table has been initialized
 * Input: none
 * Output: PASS/FAIL
 * Side Effects: none
 * Coverage: PDT[0], PDT[1], PT0[184]
 * Files: paging.c
 */
int paging_value_test()  {
	TEST_HEADER;
	int *ptr = (int*)PDT_addr;
	if( ptr[0] == 0)  {
		return FAIL;
	}
	if( ptr[1] == 0)  {
		return FAIL;
	}

	ptr = (int*)PT0_addr;
	if( ptr[VID_MEM_INDEX] == 0)  {
		return FAIL;
	}
	return PASS;
}

/* Checkpoint 2 tests */

/* RTC_test
 * 
 * Test changing RTC rate
 * Inputs: None
 * Side Effects: None
 * Coverage: rtc.h
 */
void RTC_test(){
	open_RTC(0);

	int i,j;
	char output;
	int freq = 2;
	int* ptr = &freq;

	for(i=0;i<10;i++)
	{
		for(j=0;j<20;j++)
		{
			output = ('0'+i);
			putc(output);
			read_RTC(0,0,0);
		}

		freq <<= 1;
		write_RTC(0, (void*) ptr, 4);
		putc('\n');
	}

	close_RTC(0);
	for(i=0;i<20;i++)
	{

		putc(output);
		read_RTC(0,0,0);
	}
	puts("\nRTC TEST COMPLETE\n");
}


/* read_data_test
 * 
 * Test the read_data function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int read_data_test()  {
	TEST_HEADER;

	current_pid=0;
	setup_PCB(0);

	int i;
	uint8_t test[186];
	dentry_t test_dentry;
	uint32_t inode_index;
	uint8_t test_name[11] = "frame1.txt\0";

	read_dentry_by_name(test_name, &test_dentry);
	inode_index = test_dentry.inode_number;

	read_data(inode_index, 0, test, 186);

	for(i = 0; i < 186; i++)  {
		putc((int8_t)test[i]);
	}

	putc('\n');
	
	return 0;
}

/* open_file_test
 * 
 * Test the open_data function
 * Inputs: None
 * Side Effects: opens 8 files
 * Coverage: filesystem.c
 */
int open_file_test()  {
	TEST_HEADER;

	int32_t test_fd1 = -1;
	int32_t test_fd2 = -1;
	uint8_t test_name1[10] = "ls\0";
	uint8_t test_name2[10] = "shell\0";

	test_fd1 = open_file(test_name1);
	test_fd2 = open_file(test_name2);

	if(test_fd1 < 0 || test_fd1 > 8)  {
		return FAIL;
	}
	if(test_fd2 != (test_fd1 + 1))  {
		return FAIL;
	}

	open_file(test_name1);
	open_file(test_name1);
	open_file(test_name1);
	open_file(test_name1);
	open_file(test_name1);
	open_file(test_name1);

	test_fd1 = open_file(test_name1);

	if(test_fd1 != -1)  {
		return FAIL;
	}

	return PASS;
}

/* copy_by_fname_test
 * 
 * Test the function "read_dentry_by_index" by checking the name
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem.c
 */
int read_dentry_by_index_Test()  {
	TEST_HEADER;

	const uint8_t test_num = 17;
	const uint8_t test_size = 35;
	const uint8_t max_length = 32;
	dentry_t test;
	int8_t name[test_size];
	int i = 0;
	int8_t* name_file[test_num][test_size];
		name_file[0][0] = ".\0";										//0
		name_file[1][0] = "sigtest\0";									//1
		name_file[2][0] = "shell\0";									//2
		name_file[3][0] = "grep\0";										//3
		name_file[4][0] = "syserr\0";									//4
		name_file[5][0] = "rtc\0";										//5
		name_file[6][0] = "fish\0";										//6
		name_file[7][0] = "counter\0";									//7
		name_file[8][0] = "pingpong\0";									//8
		name_file[9][0] = "cat\0";										//9
		name_file[10][0] = "frame0.txt\0";								//10
		name_file[11][0] = "verylargetextwithverylongname.txt\0";		//11
		name_file[12][0] = "ls\0";										//12
		name_file[13][0] = "testprint\0";								//13
		name_file[14][0] = "created.txt\0";								//14
		name_file[15][0] = "frame1.txt\0";								//15
		name_file[16][0] = "hello\0";									//16

	for(i=0; i<test_num; i++)
	{
		read_dentry_by_index(i, &test);

		/* copy the name of the current file */
		strcpy(name, (int8_t*)&test);

		/* check if the name is the same as expectation */
		if(strlen(name) > max_length)
		{
			if(strncmp(name, name_file[i][0], max_length) != 0)
				return FAIL;
		}
		else
		{
			if(strncmp(name, name_file[i][0], strlen(name)) != 0)
				return FAIL;
		}
	}
	return PASS;
}

/* read_dentry_by_name_Test
 * 
 * Test the function "read_dentry_by_name" by checking the name
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem.c
 */
int read_dentry_by_name_Test()  {
	TEST_HEADER;

	const uint8_t test_num = 17;
	const uint8_t test_size = 35;
	const uint8_t max_length = 32;
	dentry_t test;
	int8_t name[test_size];
	int i = 0;
	int8_t* name_file[test_num][test_size];
		name_file[0][0] = ".\0";										//0
		name_file[1][0] = "sigtest\0";									//1
		name_file[2][0] = "shell\0";									//2
		name_file[3][0] = "grep\0";										//3
		name_file[4][0] = "syserr\0";									//4
		name_file[5][0] = "rtc\0";										//5
		name_file[6][0] = "fish\0";										//6
		name_file[7][0] = "counter\0";									//7
		name_file[8][0] = "pingpong\0";									//8
		name_file[9][0] = "cat\0";										//9
		name_file[10][0] = "frame0.txt\0";								//10
		name_file[11][0] = "verylargetextwithverylongname.txt\0";		//11
		name_file[12][0] = "ls\0";										//12
		name_file[13][0] = "testprint\0";								//13
		name_file[14][0] = "created.txt\0";								//14
		name_file[15][0] = "frame1.txt\0";								//15
		name_file[16][0] = "hello\0";

	for(i=0;i<test_num;i++)
	{
		read_dentry_by_name((uint8_t*)name_file[i][0], &test);

		/* copy the name of the current file */
		strcpy(name, (int8_t*)&test);

		/* check if the name is the same as expectation */
		if(strlen(name) > max_length)
		{
			if(strncmp(name, name_file[i][0], max_length) != 0)
				return FAIL;
		}
		else
		{
			if(strncmp(name, name_file[i][0], strlen(name)) != 0)
				return FAIL;
		}
	}

	/* test if it will return -1 when the name is not any file we have now */
	name_file[0][0] = "ece391IsTooExhauting\0";
	if(read_dentry_by_name((uint8_t*)name_file[0][0], &test) != -1)
		return FAIL;

	return PASS;
}

/* terminal_read_Test
 * 
 * Test the function "terminal_read"
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal.c
 */
int terminal_read_Test()
{
	TEST_HEADER;

	int8_t buf[128];
	int byte;
	int count = 0;

	byte = terminal_read(0, buf, 0);
	while(buf[count] != '\n')
		count++;

	if(count != byte)
		return FAIL;

	if(terminal_read(0, NULL, 0) != -1)
		return FAIL;

	return PASS;
}

/* terminal_write_Test
 * 
 * Test the function "terminal_write_Test"
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal.c
 */
int terminal_write_Test()
{
	TEST_HEADER;

	int8_t* test = "sgohosdhfgo;hosdh;fgohsodgoh;sohghsohgfohds?\n";

	if(terminal_write(0,test,strlen(test)) != strlen(test))
		return FAIL;

	if( terminal_write(0,test,strlen(test)+1000) != strlen(test)+1000)
		return FAIL;

	return PASS;
}

/* terminal_open_Test
 * 
 * Test the function "terminal_open_Test"
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal.c
 */
int terminal_open_Test()
{
	TEST_HEADER;

	if(terminal_open(0) != 0)
		return FAIL;

	return PASS;
}

/* terminal_close_Test
 * 
 * Test the function "terminal_close_Test"
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal.c
 */
int terminal_close_Test()
{
	TEST_HEADER;

	if(terminal_close(0) != 0)
		return FAIL;
	
	return PASS;
}

/* read_file_test
 * 
 * Test the read_file function
 * Inputs: None
 * Side Effects: Displays file contents on the screen
 * Coverage: filesystem.c
 */
void read_file_test()  {
	TEST_HEADER;

	current_pid=0;
	setup_PCB(0);

	int32_t test_fd;
	uint8_t test_buf[1024];
	uint8_t test_name[34] = "cat";

	test_fd = open(test_name);
	
	while(0 != read_file(test_fd, test_buf, 1024))  {
		terminal_write(0, (int8_t*)test_buf, 1024);
	}

	putc('\n');

	close_file(test_fd);
}

/* write_file_test
 * 
 * Test the write_file function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int write_file_test()  {
	TEST_HEADER;

	if(write_file(0, 0, 0) != -1)  {
		return FAIL;
	}
	return PASS;
}

int close_file_test()  {
	TEST_HEADER;

	int i;

	for(i = 0; i < 8; i++)  {
		close_file(i);
	}

	for(i = 0; i < 8; i++)  {
		if(test_read_flags(i) != 0)  {
			return FAIL;
		}
	}
	return PASS;
}

/* open_directory_test
 * 
 * Test the open_directory function
 * Inputs: None
 * Side Effects: Sets all entries in
 				 dentry_table_flags to 0
 * Coverage: filesystem.c
 */
int open_directory_test()  {
	TEST_HEADER;

	int i;
	int test_fd;
	uint8_t test_name[10] = ".\0";

	for(i = 0; i < 8; i++)  {
		test_set_flags(i, 0);
	}

	for(i = 0; i < 8; i++)  {
		test_fd = open_directory(test_name);
	}

	if(test_fd != 7)  {
		return FAIL;
	}

	test_fd = open_directory(test_name);

	if(test_fd != -1)  {
		return FAIL;
	}

	for(i = 0; i < 8; i++)  {
		test_set_flags(i, 0);
	}
	return PASS;
}

/* read_directory_test
 * 
 * Test the read_directory function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int read_directory_test()  {
	TEST_HEADER;

	uint8_t test_name[34] = ".\0";
	int test_fd;
	int error_check;
	int i;

	test_fd = open_directory(test_name);
	for(i = 0; i < 17; i++)  {
		error_check = read_directory(test_fd, 0, 0);
		if(error_check == -1)  {
			return FAIL;
		}
	}

	error_check = read_directory(test_fd, 0, 0);
	if(error_check != -1)  {
		return FAIL;
	}

		close_directory(test_fd);

	return PASS;
}

/* write_directory_test
 * 
 * Test the write_directory function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int write_directory_test()  {
	TEST_HEADER;

	if(write_directory(0, 0, 0) != -1)  {
		return FAIL;
	}

	return PASS;
}

/* close_directory_test
 * 
 * Test the close_directory function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int close_directory_test()  {
	TEST_HEADER;

	int i;

	for(i = 0; i < 8; i++)  {
		test_set_flags(i, 1);
	}

	for(i = 0; i < 8; i++)  {
		close_directory(i);
	}

	for(i = 0; i < 8; i++)  {
		if(test_read_flags(i) != 0)  {
			return FAIL;
		}
	}

	return PASS;
}

/* Checkpoint 3 tests */

/* setup_PCB_test
 * 
 * Test the setup_PCB function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int setup_PCB_test()  {
	TEST_HEADER;

	if(setup_PCB(-1) != (pcb_t*)-1)
		return FAIL;
	if(setup_PCB(100) != (pcb_t*)-1)
		return FAIL;
	return PASS;
}

/* execute_test
 * 
 * Test the execute function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int execute_test()  {
	TEST_HEADER;

	if(execute(NULL) != -1)
		return FAIL;
	if(execute((uint8_t*)"LFKALKF") != -1)
		return FAIL;
	if(execute((uint8_t*)"frame0.txt") != -1)
		return FAIL;
	return PASS;
}

/* read_test
 * 
 * Test the read function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int read_test()  {
	TEST_HEADER;

	uint8_t buf[10]="AAAAAAAAAA";
	current_pid=0;
	setup_PCB(0);

	if(read(0,NULL,10) != -1)
		return FAIL;
	if(read(100,buf,10) != -1)
		return FAIL;
	if(read(-1,buf,10) != -1)
		return FAIL;
	if(read(1,buf,10) != -1)
		return FAIL;

	//test for FREE
	if(read(5,buf,10) != -1)
		return FAIL;
	return PASS;
}

/* write_test
 * 
 * Test the write function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int write_test()  {
	TEST_HEADER;

	uint8_t buf[10];
	current_pid=0;
	setup_PCB(0);

	if(write(1,NULL,10) != -1)
		return FAIL;
	if(write(100,buf,10) != -1)
		return FAIL;
	if(write(-1,buf,10) != -1)
		return FAIL;
	if(write(0,buf,10) != -1)
		return FAIL;

	//test for FREE
	if(write(5,buf,10) != -1)
		return FAIL;
	return PASS;
}

/* open_test
 * 
 * Test the open function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int open_test()  {
	TEST_HEADER;

	current_pid=0;
	setup_PCB(0);

	if(open(NULL) != -1)
		return FAIL;
	if(open((uint8_t*)"ABCDEFG") != -1)
		return FAIL;

	return PASS;
}

/* close_test
 * 
 * Test the open function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int close_test()  {
	TEST_HEADER;

	current_pid=0;
	setup_PCB(0);

	if(close(0) != -1)
		return FAIL;
	if(close(1) != -1)
		return FAIL;
	if(close(8) != -1)
		return FAIL;
	if(close(5) != -1)
		return FAIL;

	return PASS;
}

/* Checkpoint 4 tests */
/* getargs_test
 * 
 * Test the getargs function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int getargs_test() {
	TEST_HEADER;

	int8_t *buf = NULL, buf2[10];
	pcb_t *pcb = (pcb_t *)(KERNEL_BOT_ADDR - (current_pid+1) * EIGHT_KB);

	current_pid=0;
	setup_PCB(0);

	if(getargs((uint8_t *)buf, 0) != -1)
		return FAIL;
	if(getargs((uint8_t *)buf2, 10) != -1)
		return FAIL;

	strcpy((int8_t*)pcb->args, (int8_t*)"abcedf");
	if(getargs((uint8_t *)buf2, 10) != 0)
		return FAIL;
	
	if(strncmp(buf2, (int8_t*)"abcedf", 6) !=0)
		return FAIL;

	return PASS;
}

/* vidmap_test
 * 
 * Test the vidmap function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int vidmap_test() {
	TEST_HEADER;

	uint8_t** screen_start = NULL;

	if(vidmap(screen_start) != -1)
		return FAIL;

	return PASS;
}
/* set_handler_test
 * 
 * Test the set_handler function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int set_handler_test() {
	TEST_HEADER;

	int32_t signum;
	uint8_t* ptr;

	if(set_handler(signum, ptr) != -1)
		return FAIL;

	return PASS;
}
/* sigreturn_test
 * 
 * Test the sigreturn function
 * Inputs: None
 * Side Effects: None
 * Coverage: system_call.c
 */
int sigreturn_test() {
	TEST_HEADER;

	if(sigreturn() != -1)
		return FAIL;

	return PASS;
}

/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());

	// launch your tests here


	/* Checkpoint 1 */
	//TEST_OUTPUT("Syscall Test",syscall_test());
	//TEST_OUTPUT("Paging Test (KERNEL)", paging_test_kernel());
	//TEST_OUTPUT("Pagint Test (VIDMEM)", paging_test_vidmem());
	//TEST_OUTPUT("Paging Value Test", paging_value_test());
	//divide_zero_test();

	/* Checkpoint 2 */
	//RTC_test();

	//TEST_OUTPUT("Terminal Read Test", terminal_read_Test());
	//TEST_OUTPUT("Terminal Write Test", terminal_write_Test());
	//TEST_OUTPUT("Terminal Open Test", terminal_open_Test());
	//TEST_OUTPUT("Terminal Close Test", terminal_close_Test());

	//TEST_OUTPUT("Read Dentry by Index Test", read_dentry_by_index_Test());
	//TEST_OUTPUT("Read Dentry by Name Test", read_dentry_by_name_Test());
	//read_data_test();

	//TEST_OUTPUT("Open File Test", open_file_test());
	//read_file_test();
	//TEST_OUTPUT("Write File Test", write_file_test());
	//TEST_OUTPUT("Close File Test", close_file_test());

	//TEST_OUTPUT("Open Directory Test", open_directory_test());
	//TEST_OUTPUT("Read Directory Test", read_directory_test());
	//TEST_OUTPUT("Write Directory Test", write_directory_test());
	//TEST_OUTPUT("Close Directory Test", close_directory_test());


	/* Checkpoint 3 */
	//TEST_OUTPUT("setup PCB Test", setup_PCB_test());
	//TEST_OUTPUT("execute Test", execute_test());
	//TEST_OUTPUT("read Test", read_test());
	//TEST_OUTPUT("write Test", write_test());
	//TEST_OUTPUT("open Test", open_test());
	//TEST_OUTPUT("close Test", close_test());

	/* Checkpoint 4 */
	//TEST_OUTPUT("getargs Test", getargs_test());
	//TEST_OUTPUT("vidmap Test", vidmap_test());
	//TEST_OUTPUT("set_handler Test", set_handler_test());
	//TEST_OUTPUT("sigreturn Test", sigreturn_test());
}

