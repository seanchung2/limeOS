#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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

<<<<<<< HEAD
=======

>>>>>>> 3021f7a486356d595f10413c0a64c1e7afc378ae
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
<<<<<<< HEAD
=======

>>>>>>> 3021f7a486356d595f10413c0a64c1e7afc378ae
/* Overflow test
 *
 * Attempts to make an integer overflow
 * Inputs: none
 * Outputs: PASS/FAIL
 * Side Effects: none
 * Coverage: IDT exception handler
 * Files: idt.c
 */
int overflow_test()  {
	TEST_HEADER;

	int result = PASS;
	//int i = 100+ 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
	//i = 1;
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
	//TEST_OUTPUT("overflow_test", overflow_test());
	//divide_zero_test();
	//deref_null_test();
}
