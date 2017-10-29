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
	uint8_t test_index = 1;
	uint32_t test_type = 0;
	uint32_t test_inode = 0;

	read_dentry_by_index(test_index, &test);
	test_type = test.file_type;
	test_inode = test.inode_number;
	puts((int8_t*)&test);
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

	test_name[0] = 'v';
	test_name[1] = 'e';
	test_name[2] = 'r';
	test_name[3] = 'y';
	test_name[4] = 'l';
	test_name[5] = 'a';
	test_name[6] = 'r';
	test_name[7] = 'g';
	test_name[8] = 'e';
	test_name[8] = 't';
	test_name[8] = 'e';
	test_name[8] = 'x';
	test_name[8] = 't';
	test_name[8] = 'w';
	test_name[8] = 'i';
	test_name[8] = 't';
	test_name[8] = 'h';
	test_name[8] = 'v';
	test_name[8] = 'e';
	test_name[8] = 'r';
	test_name[8] = 'y';
	test_name[8] = 'l';
	test_name[8] = 'o';
	test_name[8] = 'n';
	test_name[8] = 'g';
	test_name[8] = 'n';
	test_name[8] = 'a';
	test_name[8] = 'm';
	test_name[8] = 'e';
	test_name[8] = '.';
	test_name[8] = 't';
	test_name[8] = 'x';
	test_name[8] = 't';
	test_name[8] = '\0';

	read_dentry_by_name(test_name, &test);
	test_type = test.file_type;
	test_inode = test.inode_number;
	puts((int8_t*)&test);
	printf("\nType: %d", test_type);
	printf("\nINode: %d", test_inode);
	putc('\n');
	return 0;
} 

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
	copy_by_index_test();
	copy_by_fname_test();
	read_data_test();
}
