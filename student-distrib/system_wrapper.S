
#define ASM     1
#include "x86_desc.h"

.global sys_handler

# jump table to system call
syscall_jumptable:
	.long 0x0
	.long halt
	.long execute
	.long read
	.long write
	.long open
	.long close

#
# sys_handler()
# hanlder for system call
# input: none
# output: none
# side effect: as description
#
sys_handler:
	pushl 	%ebp		#push all the regs (except for eax) into stack
	pushl	%edi
	pushl	%esi
	pushl 	%edx
	pushl	%ecx
	pushl	%ebx
	pushfl				#push all the flags into stack

	pushl	%edx			#arg3
	pushl 	%ecx			#arg2
	pushl 	%ebx			#arg1

	cmpl 	$1 , %eax 		#check if eax < 1 
	jl		error_eax
	cmpl 	$10 , %eax 	#check if eax > 10
	jg		error_eax

	movl 	syscall_jumptable(,%eax,4) , %eax
	call 	*%eax

restore:
	popfl
	popl	%ebx
	popl	%ecx
	popl	%edi
	popl	%esi
	popl	%edi
	popl	%ebp
	iret

error_eax:
	movl 	$-1 , %eax
	jmp 	restore
