#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "int_handler.h"
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
	puts("RTC TEST COMPLETE");
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

	int i;
	uint8_t test[10];
	dentry_t test_dentry;
	uint32_t inode_index;
	uint8_t test_name[10] = "sigtest\0";

	read_dentry_by_name(test_name, &test_dentry);
	inode_index = test_dentry.inode_number;

	read_data(inode_index, 0, test, 9);

	for(i = 0; i < 10; i++)  {
		printf("%d \n", (int)test[i]);
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
	int result = PASS;
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
				result = FAIL;
		}
		else
		{
			if(strncmp(name, name_file[i][0], strlen(name)) != 0)
				result = FAIL;
		}
	}
	return result;
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
	int result = PASS;
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
				result = FAIL;
		}
		else
		{
			if(strncmp(name, name_file[i][0], strlen(name)) != 0)
				result = FAIL;
		}
	}

	/* test if it will return -1 when the name is not any file we have now */
	name_file[0][0] = "ece391IsTooExhauting\0";
	if(read_dentry_by_name((uint8_t*)name_file[0][0], &test) != -1)
		result = FAIL;

	return result;
}

/* read_file_test
 * 
 * Test the read_file function
 * Inputs: None
 * Side Effects: Displays file contents onthe screen
 * Coverage: filesystem.c
 */
void read_file_test()  {
	int32_t test_fd;
	uint8_t test_buf[51];
	uint8_t test_name[34] = "sigtest\0";

	test_fd = open_file(test_name);
	read_file(test_fd, test_buf, 50);

//	terminal_read((int8_t*)test_buf);
//	terminal_write(0);

	close_file(0);
}

void close_file_test()  {

}

void open_directory_test()  {

}

void read_directory_test()  {
	uint8_t test_name[34] = ".\0";
	int test_fd;

	test_fd = open_directory(test_name);
	read_directory(test_fd, 0, 0);
}

void close_directory_test()  {

}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());

	// launch your tests here
	//TEST_OUTPUT("syscall_test",syscall_test());
	//TEST_OUTPUT("paging_test_kernel", paging_test_kernel());
	//TEST_OUTPUT("paging_test_vidmem", paging_test_vidmem());
	//TEST_OUTPUT("paging_value_test", paging_value_test());
	//RTC_test();
	//copy_by_index_test();
	//copy_by_fname_test();
	//read_data_test();
	//read_directory_test();
	TEST_OUTPUT("Read Dentry by Index Test", read_dentry_by_index_Test());
	TEST_OUTPUT("Read Dentry by Name Test", read_dentry_by_name_Test());
	//TEST_OUTPUT("Read Dentry by Index Test", read_dentry_by_index_Test());
	//TEST_OUTPUT("open_file_test", open_file_test());
	read_file_test();
}
