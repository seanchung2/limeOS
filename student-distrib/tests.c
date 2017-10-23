#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "int_handler.h"

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

/* Overflow test
 *
 * Attempts to make an integer overflow
 * Inputs: none
 * Outputs: PASS/FAIL
 * Side Effects: none
 * Coverage: IDT exception handler
 * Files: idt.c
 */
/*int overflow_test()  {
	TEST_HEADER;

	int result = PASS;
	//int i = 100+ 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
	//i = 1;
	return result;
}*/

/* System Call test
 *
 * Attempts to call system call
 * Inputs: none
 * Outputs: PASS/FAIL
 * Side Effects: none
 * Coverage: IDT system call handler
 * Files: idt.c
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


/* keyboard_handler_letter_test
 *
 * Check if the interpret from scancodes is correct
 * Input: none
 * Output: PASS/FAIL
 * Side Effects: none
 * Coverage: keyboard handler
 * Files: int_handler.c
 */
int keyboard_handler_letter_test()  {
	TEST_HEADER;

	int result = FAIL;
	uint8_t c = 0;

	cli();
	printf("Type \"a\":");
    do {
        if(inb(KEYBOARD_DATA_PORT) != c) {
            c = inb(KEYBOARD_DATA_PORT);
            if(c > 0)
                break;
        }
    } while(1);
	if (c == letter_code[0])
		result = PASS;
	sti();

	return result;
}

/* keyboard_handler_num_test
 *
 * Check if the interpret from scancodes is correct
 * Input: none
 * Output: PASS/FAIL
 * Side Effects: none
 * Coverage: keyboard handler
 * Files: int_handler.c
 */
int keyboard_handler_num_test()  {
	TEST_HEADER;

	int result = FAIL;
	uint8_t c = 0;

	cli();
	printf("Type \"0\":");
    do {
        if(inb(KEYBOARD_DATA_PORT) != c) {
            c = inb(KEYBOARD_DATA_PORT);
            if(c > 0)
                break;
        }
    } while(1);
	if (c == number_code[0])
		result = PASS;
	sti();

	return result;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());

	// launch your tests here
	TEST_OUTPUT("syscall_test",syscall_test());
	TEST_OUTPUT("paging_test_kernel", paging_test_kernel());
	TEST_OUTPUT("paging_test_vidmem", paging_test_vidmem());
	TEST_OUTPUT("keyboard_handler_letter_test", keyboard_handler_letter_test());
	TEST_OUTPUT("keyboard_handler_num_test", keyboard_handler_num_test());
	deref_null_test();
	//divide_zero_test();

}
