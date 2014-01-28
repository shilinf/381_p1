#ifndef Utility_h
#define Utility_h

#define RECORD_TITLE_SIZE 64
#define COLLECTION_NAME_SIZE 16
#define RECORD_MEDIUM_SIZE 8


int compare_record_title(const void* data_ptr1, const void* data_ptr2);

void print_record_item (void *data_ptr);

void save_record_item(void* data_ptr, void* arg_ptr);

int compare_string_with_record(const void* arg_ptr, const void* data_ptr);

void discard_input_remainder(void);

void discard_file_input_remainder(FILE *input_file);



#endif
