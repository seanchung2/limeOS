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

//test file system
int copy_by_index_test();
int copy_by_fname_test();
int read_data_test();
void open_file_test();
void read_file_test();
void close_file_test();
void open_directory_test();
void read_directory_test();
void close_directory_test();

#endif /* TESTS_H */
