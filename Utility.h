#ifndef Utility_h
#define Utility_h


int compare_record_title(const void* data_ptr1, const void* data_ptr2);

void print_record_item (void *data_ptr);

void save_record_item(void* data_ptr, void* arg_ptr);

void save_record_item_full(void* data_ptr, void* arg_ptr);

int compare_string_with_record(const void* arg_ptr, const void* data_ptr);


#endif
