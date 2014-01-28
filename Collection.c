#include <stdlib.h>
#include <string.h>
#include "Collection.h"
#include "Ordered_container.h"
#include "Utility.h"
#include "Record.h"
#include "p1_globals.h"


/* a Collection contains a pointer to a C-string name and a container
that holds pointers to Records - the members. */
struct Collection {
	char* name;
	struct Ordered_container* members; 
};

void save_record_name(void* data_ptr, void* arg_ptr);


struct Collection* create_Collection(const char* name)
{
    struct Collection *new_collection = malloc(sizeof(struct Collection));
    new_collection->name = malloc(strlen(name) + 1);
    g_string_memory += strlen(name) + 1;
    strcpy(new_collection->name, name);
    new_collection->members = OC_create_container(compare_record_title);
    return new_collection;
}

void destroy_Collection(struct Collection* collection_ptr)
{
    g_string_memory -= strlen(collection_ptr->name) + 1;
    free(collection_ptr->name);
    OC_destroy_container(collection_ptr->members);
    free(collection_ptr);
}

const char* get_Collection_name(const struct Collection* collection_ptr)
{
    return collection_ptr->name;
}

int Collection_empty(const struct Collection* collection_ptr)
{
    return OC_empty(collection_ptr->members);
}

int add_Collection_member(struct Collection* collection_ptr, const struct Record* record_ptr)
{
    if(OC_find_item(collection_ptr->members, record_ptr) == NULL) {
        OC_insert(collection_ptr->members, (void *)record_ptr);
        return 0;
    }
    else
        return 1;
}

int is_Collection_member_present(const struct Collection* collection_ptr, const struct Record* record_ptr)
{
    if(OC_find_item(collection_ptr->members, record_ptr) == NULL)
        return 0;
    else
        return 1;
}

int remove_Collection_member(struct Collection* collection_ptr, const struct Record* record_ptr)
{
    void *find_item_ptr = OC_find_item(collection_ptr->members, record_ptr);
    if(find_item_ptr == NULL)
        return 1;
    else {
        OC_delete_item(collection_ptr->members, find_item_ptr);
        return 0;
    }
}

void print_Collection(const struct Collection* collection_ptr)
{
    printf("Collection %s contains:", collection_ptr->name);
    if (OC_empty(collection_ptr->members))
        printf(" None\n");
    else {
        printf("\n");
        OC_apply(collection_ptr->members, print_record_item);
    }
}

void save_Collection(const struct Collection* collection_ptr, FILE* outfile)
{
    fprintf(outfile, "%s %d\n", collection_ptr->name, OC_get_size(collection_ptr->members));
    OC_apply_arg(collection_ptr->members, save_record_name, outfile);
}

struct Collection* load_Collection(FILE* input_file, const struct Ordered_container* records)
{
    char collection_name[COLLECTION_NAME_SIZE], fmt_str[20];
    int num_items, i;
    struct Collection *new_collection;
    
    sprintf(fmt_str, "%%%ds %%d", COLLECTION_NAME_SIZE-1);
    if(fscanf(input_file, fmt_str, collection_name, &num_items) != 2)
        return NULL;
    
    
    
    new_collection = create_Collection(collection_name);
    discard_file_input_remainder(input_file);
    
    
    for (i = 0; i < num_items; i++) {
        char title[RECORD_TITLE_SIZE];
        
        
        void *find_item_ptr;
        if(fgets(title, RECORD_TITLE_SIZE, input_file) == NULL) {
            destroy_Collection(new_collection);
            return NULL;
        }
        title[strlen(title) - 1] = '\0';
        
        
        find_item_ptr = OC_find_item_arg(records, title, compare_string_with_record);
        if (find_item_ptr == NULL) {
            destroy_Collection(new_collection);
            return NULL;
        }
        
        add_Collection_member(new_collection, OC_get_data_ptr(find_item_ptr));
    }
    
    return new_collection;
}

void save_record_name(void* data_ptr, void* arg_ptr)
{
    fprintf((FILE *)arg_ptr, "%s\n", get_Record_title((struct Record *)data_ptr));
}

