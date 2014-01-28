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
    fprintf((FILE *)arg_ptr, "%s\n", get_Record_title((struct Record *)data_ptr));
}

void save_record_item_full(void* data_ptr, void* arg_ptr)
{
    save_Record((struct Record *)data_ptr, arg_ptr);
}


int compare_string_with_record(const void* arg_ptr, const void* data_ptr)
{
    return strcmp((char *) arg_ptr, get_Record_title((struct Record *)data_ptr));
}
