#include "Collection.h"
#include "Ordered_container.h"
#include "Utility.h"
#include "Record.h"
#include "p1_globals.h"
#include <stdlib.h>
#include <string.h>


/* a Collection contains a pointer to a C-string name and a container
that holds pointers to Records - the members. */
struct Collection {
	char* name;
	struct Ordered_container* members; 
};

void save_Record_name(void* data_ptr, void* arg_ptr);
void discard_file_input_remainder(FILE *input_file);

struct Collection* create_Collection(const char* name)
{
    /* the deallocation will be done when the function destroy_Collection 
     called.*/
    struct Collection *new_collection = malloc_guard(sizeof(struct Collection));
    new_collection->name = string_deep_copy(name);
    new_collection->members = OC_create_container(compare_Record_title);
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

int add_Collection_member(struct Collection* collection_ptr,
                          const struct Record* record_ptr)
{
    if(OC_find_item(collection_ptr->members, record_ptr))
        return 1;
    else {
        OC_insert(collection_ptr->members, (void *)record_ptr);
        return 0;
    }
}

int is_Collection_member_present(const struct Collection* collection_ptr,
                                 const struct Record* record_ptr)
{
    return OC_find_item(collection_ptr->members, record_ptr) ? 1 : 0;
}

int remove_Collection_member(struct Collection* collection_ptr,
                             const struct Record* record_ptr)
{
    void *find_item_ptr = OC_find_item(collection_ptr->members, record_ptr);
    if(find_item_ptr) {
        OC_delete_item(collection_ptr->members, find_item_ptr);
        return 0;
    }
    else
        return 1;
}

void print_Collection(const struct Collection* collection_ptr)
{
    printf("Collection %s contains:", collection_ptr->name);
    if (OC_empty(collection_ptr->members))
        printf(" None\n");
    else {
        printf("\n");
        OC_apply(collection_ptr->members, print_Record_item);
    }
}

void save_Collection(const struct Collection* collection_ptr, FILE* outfile)
{
    fprintf(outfile, "%s %d\n", collection_ptr->name,
            OC_get_size(collection_ptr->members));
    OC_apply_arg(collection_ptr->members, save_Record_name, outfile);
}

struct Collection* load_Collection(FILE* input_file,
                                   const struct Ordered_container* records)
{
    char collection_name[COLLECTION_NAME_SIZE], fmt_str[30];
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
        if(!fgets(title, RECORD_TITLE_SIZE, input_file)) {
            destroy_Collection(new_collection);
            return NULL;
        }
        title[strlen(title) - 1] = '\0'; /* remove the newline character */
        find_item_ptr = OC_find_item_arg(records, title,
                                         compare_string_with_Record);
        if (!find_item_ptr) {
            destroy_Collection(new_collection);
            return NULL;
        }
        add_Collection_member(new_collection, OC_get_data_ptr(find_item_ptr));
    }
    return new_collection;
}

/* print record name to output file */
void save_Record_name(void* data_ptr, void* arg_ptr)
{
    fprintf((FILE *)arg_ptr, "%s\n",
            get_Record_title((struct Record *)data_ptr));
}

/* discard input remaining on the current line */
void discard_file_input_remainder(FILE *input_file)
{
    while (fgetc(input_file) != '\n') {
        ;
    }
}
