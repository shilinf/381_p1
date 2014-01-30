#ifndef Utility_h
#define Utility_h

#include <string.h>

#define RECORD_TITLE_SIZE 64
#define COLLECTION_NAME_SIZE 16
#define RECORD_MEDIUM_SIZE 8

/* compare functions for OC
 Compare two records' title */
int compare_Record_title(const void* data_ptr1, const void* data_ptr2);

/* compare functions for OC
 Compare string with record's name*/
int compare_string_with_Record(const void* arg_ptr, const void* data_ptr);

/* implementation for OC_apply_fp_t
 Print record to console */
void print_Record_item (void *data_ptr);

/* check whether malloc succeed */
void *malloc_guard(size_t memory_size);


#endif
