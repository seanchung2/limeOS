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

/* copy_by_index_test
 * 
 * Test the copy dentry by index function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int copy_by_index_test()  {
	TEST_HEADER;

	dentry_t test;
	uint8_t test_index = 17;
	uint32_t test_type = 0;
	uint32_t test_inode = 0;
	int i = 0;
	uint8_t* char_ptr;

	read_dentry_by_index(test_index, &test);
	char_ptr = (uint8_t*)&test;
	test_type = test.file_type;
	test_inode = test.inode_number;
	while(*char_ptr != '\0' && i < 32)  {
		putc(*char_ptr);
		char_ptr++;
		i++;
	}
	printf("\nType: %d", test_type); 
	printf("\nINode: %d", test_inode);
	putc('\n');
	return 0;
}

/* copy_by_fname_test
 * 
 * Test the copy dentry by name function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int copy_by_fname_test()  {
	TEST_HEADER;

	dentry_t test;
	uint8_t test_name[34];
	uint32_t test_type = 0;
	uint32_t test_inode = 0;
	int i = 0;
	uint8_t* char_ptr;

	test_name[0] = 's';
	test_name[1] = 'h';
	test_name[2] = 'e';
	test_name[3] = 'l';
	test_name[4] = 'l';
	test_name[5] = '\0';
	test_name[6] = 'r';
	test_name[7] = 'g';
	test_name[8] = 'e';
	test_name[9] = 't';
	test_name[10] = 'e';
	test_name[11] = 'x';
	test_name[12] = 't';
	test_name[13] = 'w';
	test_name[14] = 'i';
	test_name[15] = 't';
	test_name[16] = 'h';
	test_name[17] = 'v';
	test_name[18] = 'e';
	test_name[19] = 'r';
	test_name[20] = 'y';
	test_name[21] = 'l';
	test_name[22] = 'o';
	test_name[23] = 'n';
	test_name[24] = 'g';
	test_name[25] = 'n';
	test_name[26] = 'a';
	test_name[27] = 'm';
	test_name[28] = 'e';
	test_name[29] = '.';
	test_name[30] = 't';
	test_name[31] = 'x';
	test_name[32] = 't';
	test_name[33] = '\0';

	read_dentry_by_name(test_name, &test);
	test_type = test.file_type;
	test_inode = test.inode_number;
	char_ptr = (uint8_t*)&test;
	while(*char_ptr != '\0' && i < 32)  {
		putc(*char_ptr);
		char_ptr++;
		i++;
	}
	printf("\nType: %d", test_type);
	printf("\nINode: %d", test_inode);
	putc('\n');
	return 0;
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
	uint8_t test_name[10];

	test_name[0] = 's';
	test_name[1] = 'i';
	test_name[2] = 'g';
	test_name[3] = 't';
	test_name[4] = 'e';
	test_name[5] = 's';
	test_name[6] = 't';
	test_name[7] = '\0';
	test_name[8] = 'e';
	test_name[9] = '\0';

	read_dentry_by_name(test_name, &test_dentry);
	inode_index = test_dentry.inode_number;

	read_data(inode_index, 0, test, 9);

	for(i = 0; i < 10; i++)  {
		printf("%d \n", (int)test[i]);
	}
	putc('\n');
	return 0;
}

/* copy_by_fname_test
 * 
 * Test the copy dentry by name function
 * Inputs: None
 * Side Effects: None
 * Coverage: filesystem.c
 */
int testCopyByIndex()  {
	TEST_HEADER;

	const uint8_t test_num = 17;
	const uint8_t test_size = 35;
	dentry_t test;
	int result = PASS;
	uint32_t test_type = 0;
	uint32_t test_inode = 0;
	int8_t name[test_size];
	int i = 0;
	uint8_t* char_ptr;
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
		name_file[16][0] = "fello\0";									//16

	for(i=0; i<test_num; i++)
	{
		read_dentry_by_index(i, &test);

		/* copy the name of the current file */
		strcpy(name, (int8_t*)&test);

		/* check if the name is the same as expectation */
		if(strlen(name) > 32)
		{
			if(strncmp(name, name_file[i][0], 32) != 0)
				result = FAIL;
		}
		else
		{
			if(strncmp(name, name_file[i][0], strlen(name)) != 0)
				result = FAIL;
		}

		char_ptr = (uint8_t*)&test;
		test_type = test.file_type;
		test_inode = test.inode_number;
	}
	
/*	while(*char_ptr != '\0' && i < 32)  {
		putc(*char_ptr);
		char_ptr++;
		i++;
	}
	
	puts(name);
	printf("\nType: %d", test_type); 
	printf("\nINode: %d", test_inode);
	putc('\n');*/
	return result;
}
void open_file_test()  {

}

void read_file_test()  {

}

void close_file_test()  {

}

void open_directory_test()  {

}

void read_directory_test()  {
	uint8_t test_name[10];
	test_name[0] = '.';
	test_name[1] = '\0';
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
	TEST_OUTPUT("Copy by Index Test", testCopyByIndex());
}
