#include "Ordered_container.h"
#include "Record.h"
#include "Collection.h"
#include "Utility.h"
#include "p1_globals.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define FILE_NAME_SIZE 32

/* functions to handle command */
void find_Record_match_title(void);
void print_Record_match_id(void);
void print_Collection_match_name(void);
void print_Records(void);
void print_Catalog(void);
void print_memory_allocation(void);
void add_Record(void);
void add_Collection(void);
void add_Record_to_Collection(void);
void modify_Record_rating(void);
void delete_Record_from_Library(void);
void delete_Collection_from_Catalog(void);
void delete_Record_from_Collection(void);
int clear_Library(void);
void clear_Catalog(void);
void clear_all_data(void);
void save_all_data(void);
void restore_all_data(void);
void quit(void);

/* compare functions for OC_comp_fp_t */
int compare_Record_id(const void* data_ptr1, const void* data_ptr2);
int compare_Collection_name(const void* data_ptr1, const void* data_ptr2);
int compare_id_with_Record(const void* arg_ptr, const void* data_ptr);
int compare_name_with_Collection(const void* arg_ptr, const void* data_ptr);

/* implementation for OC_apply_if_arg_fp_t */
int check_record_in_Collection (void* data_ptr, void* arg_ptr);

/* implementation for OC_apply_if_fp_t */
int check_Collection_empty (void* data_ptr);

/* implementation for OC_apply_fp_t*/
void print_Collection_item (void *data_ptr);
void free_Record(void *data_ptr);
void OC_destory_Collection(void *data_ptr);

/* implementation for OC_apply_arg_fp_t*/
void save_Collection_item(void* data_ptr, void* arg_ptr);
void save_Record_item(void* data_ptr, void* arg_ptr);

/* helper functions */
void handle_invalid_command_error(void);
int read_check_title(char *title);
FILE *open_file(const char *mode);
void read_collection_name(char *collection_name);
void *find_collection_ptr(void);
void handle_restore_all_data_invalid(FILE *fp);
void discard_input_remainder(void);
int read_and_check_integer(int *id_input);
int check_Record_ptr_find_by_id(void *find_ptr);
int trim_title(char *title);

/* in-file global variables declaration for Catalog and Library */
static struct Ordered_container *library_ordered_by_id,
    *library_ordered_by_title, *catalog;

int main(void)
{
    char action, object;
    
    /* create containers for Catalog and Library */
    library_ordered_by_id = OC_create_container((OC_comp_fp_t)
                                                compare_Record_id);
    library_ordered_by_title =  OC_create_container((OC_comp_fp_t)
                                                    compare_Record_title);
    catalog = OC_create_container((OC_comp_fp_t) compare_Collection_name);
    
    while (1) {
        printf("\nEnter command: ");
        scanf(" %c", &action);
        scanf(" %c", &object);
        switch (action) {
            case 'f':
                switch (object) {
                    case 'r':
                        find_Record_match_title();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'p':
                switch (object) {
                    case 'r':
                        print_Record_match_id();
                        break;
                    case 'c':
                        print_Collection_match_name();
                        break;
                    case 'L':
                        print_Records();
                        break;
                    case 'C':
                        print_Catalog();
                        break;
                    case 'a':
                        print_memory_allocation();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'a':
                switch (object) {
                    case 'r':
                        add_Record();
                        break;
                    case 'c':
                        add_Collection();
                        break;
                    case 'm':
                        add_Record_to_Collection();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'm':
                switch (object) {
                    case 'r':
                        modify_Record_rating();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'd':
                switch (object) {
                    case 'r':
                        delete_Record_from_Library();
                        break;
                    case 'c':
                        delete_Collection_from_Catalog();
                        break;
                    case 'm':
                        delete_Record_from_Collection();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'c':
                switch (object) {
                    case 'L':
                        if (clear_Library()) {
                            printf("All records deleted\n");
                        }
                    break;
                    case 'C':
                        clear_Catalog();
                        printf("All collections deleted\n");
                        break;
                    case 'A':
                        clear_all_data();
                        printf("All data deleted\n");
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 's':
                switch (object) {
                    case 'A':
                        save_all_data();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'r':
                switch (object) {
                    case 'A':
                        restore_all_data();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
                break;
            case 'q':
                switch (object) {
                    case 'q':
                        quit();
                        break;
                    default:
                        handle_invalid_command_error();
                        break;
                }
            default:
                handle_invalid_command_error();
                break;
        }
    }
    return 0;
}

/* compare two records' id */
int compare_Record_id(const void* data_ptr1, const void* data_ptr2)
{
    return get_Record_ID((struct Record *)data_ptr1) -
    get_Record_ID((struct Record *)data_ptr2);
}

/* compare two collections' name */
int compare_Collection_name(const void* data_ptr1, const void* data_ptr2)
{
    return strcmp(get_Collection_name((struct Collection *)data_ptr1),
                  get_Collection_name((struct Collection *)data_ptr2));
}

/* Print error message for invalid comamnd and read to new line */
void handle_invalid_command_error(void)
{
    printf("%s", "Unrecognized command!\n");
    discard_input_remainder();
}

/* find and print the specified record with the matching title */
void find_Record_match_title(void)
{
    char title[RECORD_TITLE_SIZE];
    if (read_check_title(title)) {
        void *find_item_ptr = OC_find_item_arg(library_ordered_by_title, title,
                                               compare_string_with_Record);
        if (find_item_ptr)
            print_Record(OC_get_data_ptr(find_item_ptr));
        else
            printf("No record with that title!\n");
    }
}

/* print the specified record with the matching ID number */
void print_Record_match_id(void)
{
    int id_input;
    if (read_and_check_integer(&id_input)) {
        void *find_item_ptr = OC_find_item_arg(library_ordered_by_id, &id_input,
                                               compare_id_with_Record);
        if (check_Record_ptr_find_by_id(find_item_ptr))
            print_Record(OC_get_data_ptr(find_item_ptr));
    }
}

/* print collection - print each record in the collection with the
 specified name */
void print_Collection_match_name(void)
{
    void *find_item_ptr = find_collection_ptr();
    if (find_item_ptr)
        print_Collection(OC_get_data_ptr(find_item_ptr));
}

/* print all the records in the Library */
void print_Records(void)
{
    int library_size = OC_get_size(library_ordered_by_title);
    if (library_size) {
        printf("Library contains %d records:\n", library_size);
        OC_apply(library_ordered_by_title, print_Record_item);
    }
    else
        printf("Library is empty\n");
}

/* print the Catalog - print all the collections in the Catalog */
void print_Catalog(void)
{
    int catalog_size = OC_get_size(catalog);
    if (catalog_size) {
        printf("Catalog contains %d collections:\n", catalog_size);
        OC_apply(catalog, print_Collection_item);
    }
    else
        printf("Catalog is empty\n");
}

/* print the collection passed in */
void print_Collection_item (void *data_ptr)
{
    print_Collection((const struct Collection *) data_ptr);
}

/* print memory allocations  */
void print_memory_allocation(void)
{
    printf("Memory allocations:\n");
    printf("Records: %d\n", OC_get_size(library_ordered_by_title));
    printf("Collections: %d\n", OC_get_size(catalog));
    printf("Containers: %d\n", g_Container_count);
    printf("Container items in use: %d\n", g_Container_items_in_use);
    printf("Container items allocated: %d\n", g_Container_items_allocated);
    printf("C-strings: %d bytes total\n", g_string_memory);
}

/* add a record to the Library */
void add_Record(void)
{
    char medium[RECORD_MEDIUM_SIZE], title[RECORD_TITLE_SIZE];
    char fmt_str[20];
    sprintf(fmt_str, "%%%ds", RECORD_MEDIUM_SIZE-1);
    scanf(fmt_str, medium);
    if (read_check_title(title) == 1) {
        void *find_item_ptr = OC_find_item_arg(library_ordered_by_title, title,
                                               compare_string_with_Record);
        if (find_item_ptr)
            printf("Library already has a record with this title!\n");
        else {
            struct Record *new_record = create_Record(medium, title);
            OC_insert(library_ordered_by_title, new_record);
            OC_insert(library_ordered_by_id, new_record);
            printf("Record %d added\n", get_Record_ID(new_record));
        }
    }
}

/* add a collection with the specified name */
void add_Collection(void)
{
    char collection_name[COLLECTION_NAME_SIZE];
    void *find_item_ptr;
    read_collection_name(collection_name);
    find_item_ptr = OC_find_item_arg(catalog, collection_name,
                                     compare_name_with_Collection);
    if (find_item_ptr) {
        printf("Catalog already has a collection with this name!\n");
        discard_input_remainder();
    }
    else {
        struct Collection *new_collection = create_Collection(collection_name);
        OC_insert(catalog, new_collection);
        printf("Collection %s added\n", collection_name);
    }
}

/* compare name with one collection's name */
int compare_name_with_Collection(const void* arg_ptr, const void* data_ptr)
{
    return strcmp((char *) arg_ptr,
                  get_Collection_name((struct Collection *)data_ptr));
}

/* add a record to a specified collection */
void add_Record_to_Collection(void)
{
    void *find_item_ptr = find_collection_ptr(), *find_Record_item_ptr;
    int id_input;
    struct Collection *find_collection;
    struct Record *find_Record;
    if (!find_item_ptr) {
        return;
    }
    find_collection = OC_get_data_ptr(find_item_ptr);
    if (!read_and_check_integer(&id_input)) {
        return;
    }
    find_Record_item_ptr = OC_find_item_arg(library_ordered_by_id, &id_input,
                                            compare_id_with_Record);
    if (!check_Record_ptr_find_by_id(find_Record_item_ptr)) {
        return;
    }
    find_Record =OC_get_data_ptr(find_Record_item_ptr);
    if (add_Collection_member(find_collection, find_Record)) {
        printf("Record is already a member in the collection!\n");
        discard_input_remainder();
    }
    else
        printf("Member %d %s added\n", id_input, get_Record_title(find_Record));
}

/* modify the rating of the specified record with the matching ID number */
void modify_Record_rating(void)
{
    int id_input, rating_input;
    void *find_item_ptr;
    if (!read_and_check_integer(&id_input)) {
        return;
    }
    find_item_ptr = OC_find_item_arg(library_ordered_by_id, &id_input,
                                     compare_id_with_Record);
    
    if (!check_Record_ptr_find_by_id(find_item_ptr)) {
        return;
    }
    if (!read_and_check_integer(&rating_input)) {
        return;
    }
    if (rating_input >=1 && rating_input <= 5) {
        set_Record_rating(OC_get_data_ptr(find_item_ptr), rating_input);
        printf("Rating for record %d changed to %d\n", id_input, rating_input);
    }
    else {
        printf("Rating is out of range!\n");
        discard_input_remainder();
    }
}

/* delete the specified record from the Library */
void delete_Record_from_Library(void)
{
    char title[RECORD_TITLE_SIZE];
    if (read_check_title(title)) {
        struct Record *find_data;
        void * find_item_ptr = OC_find_item_arg(library_ordered_by_title, title,
                                                compare_string_with_Record);
        if (!find_item_ptr) {
            printf("No record with that title!\n");
            return;
        }
        find_data = OC_get_data_ptr(find_item_ptr);
        if (OC_apply_if_arg(catalog, check_record_in_Collection, find_data))
            printf("Cannot delete a record that is a member of a collection!\n");
        else {
            printf("Record %d %s deleted\n", get_Record_ID(find_data), title);
            OC_delete_item(library_ordered_by_id,
                           OC_find_item(library_ordered_by_id, find_data));
            OC_delete_item(library_ordered_by_title,
                           OC_find_item(library_ordered_by_title, find_data));
            destroy_Record(find_data);
        }
    }
}

/* Check whether a record is in collection.
 Return non-zero if the record is a member, zero if not. */
int check_record_in_Collection (void* data_ptr, void* arg_ptr)
{
    return is_Collection_member_present((struct Collection *)data_ptr,
                                        (struct Record *)arg_ptr);
}

/* delete the specified collection from the Catalog */
void delete_Collection_from_Catalog(void)
{
    void *find_item_ptr = find_collection_ptr();
    if (find_item_ptr) {
        void *collection_data_ptr = OC_get_data_ptr(find_item_ptr);
        printf("Collection %s deleted\n",
               get_Collection_name(collection_data_ptr));
        OC_delete_item(catalog, find_item_ptr);
        destroy_Collection(collection_data_ptr);
    }
}

/* delete the specified record as member of the a specified collection */
void delete_Record_from_Collection(void)
{
    void *find_item_ptr = find_collection_ptr(), *find_record_item_ptr;
    int id_input;
    struct Record *find_record;
    if (!find_item_ptr) {
        return;
    }
    if (!read_and_check_integer(&id_input)) {
        return;
    }
    find_record_item_ptr = OC_find_item_arg(library_ordered_by_id, &id_input,
                                            compare_id_with_Record);
    if (!check_Record_ptr_find_by_id(find_record_item_ptr)) {
        return;
    }
    find_record = OC_get_data_ptr(find_record_item_ptr);
    if (is_Collection_member_present(OC_get_data_ptr(find_item_ptr),
                                     find_record)) {
        printf("Member %d %s deleted\n", id_input,
               get_Record_title(find_record));
        remove_Collection_member(OC_get_data_ptr(find_item_ptr), find_record);
    }
    else {
        printf("Record is not a member in the collection!\n");
        discard_input_remainder();
    }
}

/* Clear the Library. If collections are not empty, return 0. Otherwise,
 return 1. */
int clear_Library(void)
{
    if (OC_apply_if(catalog, check_Collection_empty)) {
        printf("Cannot clear all records unless all collections are empty!\n");
        discard_input_remainder();
        return 0;
    }
    else {
        OC_apply(library_ordered_by_title, free_Record);
        OC_clear(library_ordered_by_title);
        OC_clear(library_ordered_by_id);
        reset_Record_ID_counter();
        return 1;
    }
}

/* Check whether a collection is empty.
 Return zero if the collection is empty, non-zero if not. */
int check_Collection_empty (void* data_ptr)
{
    return !Collection_empty((struct Collection *)data_ptr);
}

/* destroy the record passed in */
void free_Record(void *data_ptr)
{
    destroy_Record((struct Record *)data_ptr);
}

/* clear the Catalog: destroy all of the collections in the Catalog,
 and clear the Catalog */
void clear_Catalog(void)
{
    OC_apply(catalog, OC_destory_Collection);
    OC_clear(catalog);
}

/* destroy the collection passed in */
void OC_destory_Collection(void *data_ptr)
{
    destroy_Collection((struct Collection *)data_ptr);
}

/*  clear all data: first clear the Catalog as in cC,
 then clear the Library as in cL */
void clear_all_data(void)
{
    clear_Catalog();
    clear_Library();
}

/* save all data: write the Library and Catalog data to the named file */
void save_all_data(void)
{
    FILE *fp = open_file("w");
    if (fp) {
        fprintf(fp, "%d\n", OC_get_size(library_ordered_by_title));
        OC_apply_arg(library_ordered_by_title, save_Record_item, fp);
        fprintf(fp, "%d\n", OC_get_size(catalog));
        OC_apply_arg(catalog, save_Collection_item, fp);
        fclose(fp);
        printf("Data saved\n");
    }
}

/* arg_ptr is a FILE pointer. Save Record to that file. */
void save_Record_item(void* data_ptr, void* arg_ptr)
{
    save_Record((struct Record *)data_ptr, (FILE *)arg_ptr);
}

/* arg_ptr is a FILE pointer. Save Collection to that file.  */
void save_Collection_item(void* data_ptr, void* arg_ptr)
{
    save_Collection((struct Collection *)data_ptr, (FILE *)arg_ptr);
}

/* restore all data - restore the Library and Catalog data from the file */
void restore_all_data(void)
{
    int num_record, num_collection, i;
    FILE *fp = open_file("r");
    if (!fp) {
        return;
    }
    clear_all_data();
    if (fscanf(fp, "%d", &num_record) != 1) {
        handle_restore_all_data_invalid(fp);
        return;
    }
    for (i = 0; i < num_record; i++) {
        struct Record *new_record = load_Record(fp);
        if (!new_record) {
            handle_restore_all_data_invalid(fp);
            return;
        }
        else {
            OC_insert(library_ordered_by_id, new_record);
            OC_insert(library_ordered_by_title, new_record);
        }
    }
    if (fscanf(fp, "%d", &num_collection) != 1) {
        handle_restore_all_data_invalid(fp);
        return;
    }
    for (i = 0; i < num_collection; i++) {
        struct Collection *new_collection =
        load_Collection(fp, library_ordered_by_title);
        if (!new_collection) {
            handle_restore_all_data_invalid(fp);
            return;
        }
        OC_insert(catalog, new_collection);
    }
    fclose(fp);
    printf("Data loaded\n");
}

/* Print error message for command restore all data. Then close file and
 clear all data have read in. */
void handle_restore_all_data_invalid(FILE *fp)
{
    printf("Invalid data found in file!\n");
    fclose(fp);
    clear_all_data();
}

/* Read file name and open the file with mode passed in.
 Print error message if open the file failed.
 Return the FILE pointer */
FILE *open_file(const char *mode)
{
    FILE *fp;
    char file_name[FILE_NAME_SIZE];
    char fmt_str[20];
    sprintf(fmt_str, "%%%ds", FILE_NAME_SIZE-1);
    scanf(fmt_str, file_name);
    if (!(fp = fopen(file_name, mode))) {
        printf("Could not open file!\n");
        discard_input_remainder();
    }
    return fp;
}

/* clear all data (as in cA), and also destroy the Library and Catalog
 containers themselves, so that all memory is deallocated, and then terminate */
void quit(void)
{
    clear_all_data();
    printf("All data deleted\n");
    OC_destroy_container(library_ordered_by_id);
    OC_destroy_container(library_ordered_by_title);
    OC_destroy_container(catalog);
    printf("Done\n");
    exit(EXIT_SUCCESS);
}

/* Read in an integer and check whether scanf succeeds.
 Return 1 if scanf succeeds, 0 if not*/
int read_and_check_integer(int *id_input)
{
    if (scanf("%d", id_input) != 1) {
        printf("Could not read an integer value!\n");
        discard_input_remainder();
        return 0;
    }
    else
        return 1;
}

/* check Record ptr find by ID and print error message if find_ptr is null
 return 0 if find_ptr is null, 1 if not */
int check_Record_ptr_find_by_id(void *find_ptr)
{
    if (!find_ptr) {
        printf("No record with that ID!\n");
        discard_input_remainder();
        return 0;
    }
    else
        return 1;
}

/* compare id with one record's id */
int compare_id_with_Record(const void* arg_ptr, const void* data_ptr)
{
    return *(int *)arg_ptr - get_Record_ID((struct Record *)data_ptr);
}

/* Find the collection pointer from catalog with the specified collection name. 
 Print error message and read to new line if the collection doesn't exist*/
void *find_collection_ptr(void)
{
    char collection_name[COLLECTION_NAME_SIZE];
    void *find_item_ptr;
    read_collection_name(collection_name);
    find_item_ptr = OC_find_item_arg(catalog, collection_name,
                            compare_name_with_Collection);
    if (!find_item_ptr) {
        printf("No collection with that name!\n");
        discard_input_remainder();
    }
    return find_item_ptr;
}

/* read collection name to the passed in char pointer */
void read_collection_name(char *collection_name)
{
    char fmt_str[20];
    sprintf(fmt_str, "%%%ds", COLLECTION_NAME_SIZE-1);
    scanf(fmt_str, collection_name);
}

/* Read title and check whether its valid.
 return 1 if the title is valie, 0 if not.*/
int read_check_title(char *title)
{
    fgets(title, RECORD_TITLE_SIZE, stdin);
    if (trim_title(title))
        return 1;
    else {
        printf("Could not read a title!\n");
        return 0;
    }
}

/* Remove leading or trailing whitespace of the title passed in, and all 
 embedded consecutive whitespace characters shrunk to a single 
 character (' '). Return 0 if the tile is empty after trim, 1 if not*/
int trim_title(char *title)
{
    char *title_iterator = title;
    int valid = 0;
    while (*title) {
        while (isspace(*title))
            title++;
        while (!isspace(*title) && *title != '\0') {
            *title_iterator++ = *title++;
            valid = 1;
        }
        /* If title doesn't touch '\0', add one space to end of current string*/
        if (*title != '\0')
            *title_iterator++ = ' ';
    }
    /* If the last character before '\0' is space and there exists non-sapce
     character in the string, put the '\0' one position before to
     occupy the trailing whitespace. */
    if (isspace(*(--title)) && valid)
        *(title_iterator-1) = '\0';
    else /* Otherwise, simply add '\0' to the end of string */
        *title_iterator = '\0';
    return valid;
}

/* Read to new line */
void discard_input_remainder(void)
{
    while (getchar() != '\n') {
        ;
    }
}
