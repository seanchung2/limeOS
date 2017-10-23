#ifndef TESTS_H
#define TESTS_H

// test launcher
void launch_tests();

// test exceptions
void divide_zero_test();
void deref_null_test();
int syscall_test();

// test paging
int paging_test_kernel();
int paging_test_vidmem();

void type_tester(char c);

#endif /* TESTS_H */
