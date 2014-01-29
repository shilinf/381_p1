#include <stdio.h>
#include <stdlib.h>
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

int compare_string_with_record(const void* arg_ptr, const void* data_ptr)
{
    return strcmp((char *) arg_ptr, get_Record_title((struct Record *)data_ptr));
}

void *malloc_guard(size_t memory_size)
{
    void *new_memory = malloc(memory_size);
    if (new_memory == NULL) {
        fprintf(stderr, "Memory allocation failed!");
        exit(EXIT_FAILURE);
    }
    return new_memory;
}

