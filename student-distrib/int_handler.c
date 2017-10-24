#include "int_handler.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"

/*
* sys_handler()
* hanlder for system call
* input: none
* output: none
* side effect: as description
*/
void sys_handler ()
{
	printf("System Call !!!!!! \n");
}

