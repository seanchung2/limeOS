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

// test exceptions
void type_tester(char c);

//test for RTC
void RTC_test();

//test terminal
int terminal_read_Test();
int terminal_write_Test();

//test file system
int read_dentry_by_index_Test();
int read_dentry_by_name_Test();
int read_data_test();

int open_file_test();
void read_file_test();
int write_file_test();
int close_file_test();

void open_directory_test();
int read_directory_test();
int write_directory_test();
void close_directory_test();

#endif /* TESTS_H */
