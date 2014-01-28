#include <stdio.h>
#include <string.h>
#include "Utility.h"
#include "Record.h"

int compare_record_title(const void* data_ptr1, const void* data_ptr2)
{
    return strcmp(get_Record_title((struct Record *)data_ptr1), get_Record_title((struct Record *)data_ptr2));
}

void print_record_item (void *data_ptr)
{
    print_Record((const struct Record *) data_ptr);
}

void save_record_item(void* data_ptr, void* arg_ptr)
{
    save_Record((struct Record *)data_ptr, (FILE *)arg_ptr);
}



int compare_string_with_record(const void* arg_ptr, const void* data_ptr)
{
    return strcmp((char *) arg_ptr, get_Record_title((struct Record *)data_ptr));
}

void discard_input_remainder(void)
{
    while (getchar() != '\n') {
        ;
    }
}

void discard_file_input_remainder(FILE *input_file)
{
    while (fgetc(input_file) != '\n') {
        ;
    }
}


