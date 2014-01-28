#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "Ordered_container.h"
#include "Record.h"
#include "Collection.h"
#include "Utility.h"
#include "p1_globals.h"

#define FILE_NAME_SIZE 32
#define FILE_NAME_FMT "%31s"

struct Ordered_container *library_id;
struct Ordered_container *library_title;
struct Ordered_container *catalog;

int compare_record_id(const void* data_ptr1, const void* data_ptr2);
int compare_collection_name(const void* data_ptr1, const void* data_ptr2);
int compare_string_with_record(const void* arg_ptr, const void* data_ptr);
int compare_id_with_record(const void* arg_ptr, const void* data_ptr);
int trim_title(char *title);
void discard_input_remainder(void);

void *check_collection_name(void);


int compare_record_id(const void* data_ptr1, const void* data_ptr2)
{
    return get_Record_ID((struct Record *)data_ptr1) - get_Record_ID((struct Record *)data_ptr2);
}


int compare_collection_name(const void* data_ptr1, const void* data_ptr2)
{
    return strcmp(get_Collection_name((struct Collection *)data_ptr1), get_Collection_name((struct Collection *)data_ptr2));
}



int compare_id_with_record(const void* arg_ptr, const void* data_ptr)
{
    return *(int *)arg_ptr - get_Record_ID((struct Record *)data_ptr);
}

int compare_name_with_collection(const void* arg_ptr, const void* data_ptr)
{
    return strcmp((char *) arg_ptr, get_Collection_name((struct Collection *)data_ptr));
}


int check_record_in_collection (void* data_ptr, void* arg_ptr)
{
    return is_Collection_member_present((struct Collection *)data_ptr, (struct Record *)arg_ptr);
}

int check_collection_empty (void* data_ptr)
{
    /* check whether ! work as I like */
    return !Collection_empty((struct Collection *)data_ptr);
}


void print_collection_item (void *data_ptr)
{
    print_Collection((const struct Collection *) data_ptr);
}

void free_record(void *data_ptr)
{
    destroy_Record((struct Record *)data_ptr);
}


void OC_destory_collection(void *data_ptr)
{
    destroy_Collection((struct Collection *)data_ptr);
}

void save_collection_item(void* data_ptr, void* arg_ptr)
{
    save_Collection((struct Collection *)data_ptr, (FILE *)arg_ptr);
}

void fr(void)
{
    char title[RECORD_TITLE_SIZE];
    if(fgets(title, RECORD_TITLE_SIZE, stdin) == NULL) {
        printf("Could not read a title!\n");
        discard_input_remainder();
        return;
    }
    if (trim_title(title) == 0) {
        printf("Could not read a title!\n");
    }
    else {
        void *find_item_ptr = OC_find_item_arg(library_title, title, compare_string_with_record);
        if (find_item_ptr == NULL) {
            printf("No record with that title!\n");
        }
        else {
            print_Record(OC_get_data_ptr(find_item_ptr));
        }
    }
}

void pr(void)
{
    int id_input;
    if (scanf("%d", &id_input) == 1) {
        void *find_item_ptr = OC_find_item_arg(library_id, &id_input, compare_id_with_record);
        if (find_item_ptr == NULL) {
            printf("No record with that ID!\n");
            discard_input_remainder();
        }
        else {
            print_Record(OC_get_data_ptr(find_item_ptr));
        }
    }
    else {
        printf("Could not read an integer value!\n");
        discard_input_remainder();
    }
}

void pc(void)
{
    char collection_name[COLLECTION_NAME_SIZE];
    void *find_item_ptr;
    scanf(COLLECTION_NAME_FMT, collection_name);
    find_item_ptr = OC_find_item_arg(catalog, collection_name, compare_name_with_collection);
    if (find_item_ptr == NULL) {
        printf("No collection with that name!\n");
        discard_input_remainder();
    }
    else {
        print_Collection(OC_get_data_ptr(find_item_ptr));
    }
}


void pL(void)
{
    int library_size = OC_get_size(library_title);
    if (library_size == 0)
        printf("Library is empty\n");
    else {
        printf("Library contains %d records:\n", library_size);
        OC_apply(library_title, print_record_item);
    }
}

void pC(void)
{
    int catalog_size = OC_get_size(catalog);
    if (catalog_size == 0) {
        printf("Catalog is empty\n");
    }
    else {
        printf("Catalog contains %d collections:\n", catalog_size);
        OC_apply(catalog, print_collection_item);
    }
}

void pa(void)
{
    printf("Memory allocations:\n");
    printf("Records: %d\n", OC_get_size(library_title));
    printf("Collections: %d\n", OC_get_size(catalog));
    printf("Containers: %d\n", g_Container_count);
    printf("Container items in use: %d\n", g_Container_items_in_use);
    printf("Container items allocated: %d\n", g_Container_items_allocated);
    printf("C-strings: %d bytes total\n", g_string_memory);
}

void ar(void)
{
    /* read title is same as fr (try to use function afterwards) */
    char medium[RECORD_MEDIUM_SIZE], title[RECORD_TITLE_SIZE];
    scanf(RECORD_MEDIUM_FMT, medium);
    fgets(title, RECORD_TITLE_SIZE, stdin);
    if (trim_title(title) == 0) {
        printf("Could not read a title!\n");
    }
    else {
        void *find_item_ptr = OC_find_item_arg(library_title, &title, compare_string_with_record);
        if (find_item_ptr == NULL) {
            struct Record *new_record = create_Record(medium, title);
            OC_insert(library_title, new_record);
            OC_insert(library_id, new_record);
            printf("Record %d added\n", get_Record_ID(new_record));
        }
        else {
            printf("Library already has a record with this title!\n");
        }
    }
}

void ac(void)
{
    /* almost same as pc duplicate */
    char collection_name[COLLECTION_NAME_SIZE];
    void *find_item_ptr;
    scanf(COLLECTION_NAME_FMT, collection_name);
    find_item_ptr = OC_find_item_arg(catalog, collection_name, compare_name_with_collection);
    if (find_item_ptr == NULL) {
        struct Collection *new_collection = create_Collection(collection_name);
        OC_insert(catalog, new_collection);
        printf("Collection %s added\n", collection_name);
    }
    else {
        printf("Catalog already has a collection with this name!\n");
        discard_input_remainder();
    }
}

void am(void)
{
    /*duplicate */
    char collection_name[COLLECTION_NAME_SIZE];
    void *find_item_ptr;
    scanf(COLLECTION_NAME_FMT, collection_name);
    find_item_ptr = OC_find_item_arg(catalog, collection_name, compare_string_with_record);
    if (find_item_ptr == NULL) {
        printf("No collection with that name!\n");
        discard_input_remainder();
    }
    else {
        int id_input;
        struct Collection *find_collection = OC_get_data_ptr(find_item_ptr);
        if (scanf("%d", &id_input) == 1) {
            void *find_item_ptr = OC_find_item_arg(library_id, &id_input, compare_id_with_record);
            if (find_item_ptr == NULL) {
                printf("No record with that ID!\n");
                discard_input_remainder();
            }
            else {
                struct Record *find_item =OC_get_data_ptr(find_item_ptr);
                if (add_Collection_member(find_collection, find_item) != 0)
                    printf("Record is already a member in the collection!\n");
                else
                    printf("Member %d %s added\n", id_input, get_Record_title(find_item));
            }
        }
        else {
            printf("Could not read an integer value!\n");
            discard_input_remainder();
        }
    }
}

void mr(void)
{
    int id_input;
    if (scanf("%d", &id_input) == 1) {
        void *find_item_ptr = OC_find_item_arg(library_id, &id_input, compare_id_with_record);
        if (find_item_ptr == NULL) {
            printf("No record with that ID!\n");
            discard_input_remainder();
        }
        else {
            int rating_input;
            if (scanf("%d", &rating_input) == 1) {
                if (rating_input >=1 && rating_input <= 5) {
                    set_Record_rating(OC_get_data_ptr(find_item_ptr), rating_input);
                    printf("Rating for record %d changed to %d\n", id_input, rating_input);
                }
                else {
                    printf("Rating is out of range!\n");
                    discard_input_remainder();
                }
            }
            else {
                printf("Could not read an integer value!\n");
                discard_input_remainder();
            }
        }
    }
    else {
        printf("Could not read an integer value!\n");
        discard_input_remainder();
    }
}

void dr(void)
{
    char title[RECORD_TITLE_SIZE];
    if(fgets(title, RECORD_TITLE_SIZE, stdin) == NULL) {
        printf("Could not read a title!\n");
        return;
    }
    if (trim_title(title) == 0) {
        printf("Could not read a title!\n");
    }
    else {
        void * find_item_ptr = OC_find_item_arg(library_title, title, compare_string_with_record);
        if (find_item_ptr == NULL) {
            printf("No record with that title!\n");
        }
        else {
            struct Record *find_data = OC_get_data_ptr(find_item_ptr);
            if (OC_apply_if_arg(catalog, check_record_in_collection, find_data) == 0) {
                printf("Record %d %s deleted\n", get_Record_ID(find_data), title);
                OC_delete_item(library_id, OC_find_item(library_id, find_data));
                OC_delete_item(library_title, OC_find_item(library_title, find_data));
                destroy_Record(find_data);
            }
            else
                printf("Cannot delete a record that is a member of a collection!\n");
        }
    }
}

/* used the function check_collection_name */
void dc(void)
{
    void *find_collection_item_ptr = check_collection_name();
    void *collection_data_ptr;
    if (find_collection_item_ptr == NULL) {
        printf("No collection with that name!\n");
        discard_input_remainder();
    }
    else {
        collection_data_ptr = OC_get_data_ptr(find_collection_item_ptr);
        printf("Collection %s deleted\n", get_Collection_name(collection_data_ptr));
        OC_delete_item(catalog, find_collection_item_ptr);
        destroy_Collection(collection_data_ptr);
    }
}

void dm(void)
{
    /*read collection name duplicate code*/
    char collection_name[COLLECTION_NAME_SIZE];
    void *find_item_ptr;
    scanf(COLLECTION_NAME_FMT, collection_name);
    find_item_ptr = OC_find_item_arg(catalog, collection_name, compare_name_with_collection);
    if (find_item_ptr == NULL) {
        printf("No collection with that name!\n");
        discard_input_remainder();
    }
    else {
        int id_input;
        if (scanf("%d", &id_input) == 1) {
            void *find_record_item_ptr = OC_find_item_arg(library_id, &id_input, compare_id_with_record);
            if (find_record_item_ptr == NULL) {
                printf("No record with that ID!\n");
                discard_input_remainder();
            }
            else {
                struct Record *find_record = OC_get_data_ptr(find_record_item_ptr);
                if (is_Collection_member_present(OC_get_data_ptr(find_item_ptr), find_record) == 0) {
                    printf("Record is not a member in the collection!\n");
                    discard_input_remainder();
                }
                else {
                    printf("Member %d %s deleted\n", id_input, get_Record_title(find_record));
                    remove_Collection_member(OC_get_data_ptr(find_item_ptr), find_record);
                }
            }
        }
        else {
            printf("Could not read an integer value!\n");
            discard_input_remainder();
        }
    }
}

int cL(void)
{
    if (OC_apply_if(catalog, check_collection_empty) == 0) {
        OC_apply(library_title, free_record);
        OC_clear(library_title);
        OC_clear(library_id);
        reset_Record_ID_counter();
        return 1;
    }
    else {
        printf("Cannot clear all records unless all collections are empty!\n");
        discard_input_remainder();
        return 0;
    }
}

void cC(void)
{
    OC_apply(catalog, OC_destory_collection);
    OC_clear(catalog);
}

void cA(void)
{
    cC();
    cL();
}

void sA(void)
{
    FILE *fp;
    char file_name[FILE_NAME_SIZE];
    scanf(FILE_NAME_FMT, file_name);
    if ((fp = fopen(file_name, "w")) == NULL) {
        printf("Could not open file!\n");
        discard_input_remainder();
    }
    else {
        fprintf(fp, "%d\n", OC_get_size(library_title));
        OC_apply_arg(library_title, save_record_item_full, fp);
        fprintf(fp, "%d\n", OC_get_size(catalog));
        OC_apply_arg(catalog, save_collection_item, fp);
        fclose(fp);
        printf("Data saved\n");
    }
}



void rA(void)
{
    FILE *fp;
    char file_name[FILE_NAME_SIZE];
    int num_record, i, num_collection, read_success = 1;
    scanf(FILE_NAME_FMT, file_name);
    if ((fp = fopen(file_name, "r")) == NULL) {
        printf("Could not open file!\n");
        discard_input_remainder();
        return;
    }
    cA();

    /* remember handle the ID number, remember remove all unvalid data before return */
    if (fscanf(fp, "%d", &num_record) != 1) {
        printf("Invalid data found in file!\n");
        discard_input_remainder();
        return;
    }
    for (i = 0; i < num_record; i++) {
        struct Record *new_record = load_Record(fp);
        fprintf(stderr, "%d", i);
        if (new_record == NULL) {
            read_success = 0;
            break;
        }
        else {
            OC_insert(library_id, new_record);
            OC_insert(library_title, new_record);
        }
    }
    if (read_success == 0) {
        printf("Invalid data found in file!\n");
        discard_input_remainder();
        fclose(fp);
        return;
    }
    if (fscanf(fp, "%d", &num_collection) != 1) {
        printf("Invalid data found in file!\n");
        discard_input_remainder();
        return;
    }
    for (i = 0; i < num_collection; i++) {
        struct Collection *new_collection = load_Collection(fp, library_title);
        if (new_collection == NULL) {
            printf("Invalid data found in file!\n");
            discard_input_remainder();
            return;
        }
        OC_insert(catalog, new_collection);
    }
    fclose(fp);
    printf("Data loaded\n");
}

void qq(void)
{
    cA();
    printf("All data deleted\n");
    OC_destroy_container(library_id);
    OC_destroy_container(library_title);
    OC_destroy_container(catalog);
    printf("Done\n");
    exit(EXIT_SUCCESS);
}

/*?? Is this useful? */
void *check_collection_name(void)
{
    char collection_name[COLLECTION_NAME_SIZE];
    scanf(COLLECTION_NAME_FMT, collection_name);
    return OC_find_item_arg(catalog, collection_name, compare_name_with_collection);
}


int main(void)
{
    char action, object;
    const char * const command_invalid = "Unrecognized command!\n";
    
    library_id = OC_create_container((OC_comp_fp_t) compare_record_id);
    library_title =  OC_create_container((OC_comp_fp_t) compare_record_title);
    catalog = OC_create_container((OC_comp_fp_t) compare_collection_name);
    
    while (1) {
        /*if ((action = getchar()) != 1 || (object = getchar()) != 1) {
            printf("%s", command_invalid);
        }
         P14 14 don't check eof
         */
        printf("\nEnter command: ");
        scanf(" %c", &action);
        scanf(" %c", &object);
        switch (action) {
            case 'f':
                switch (object) {
                    case 'r':
                        fr();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'p':
                switch (object) {
                    case 'r':
                        pr();
                        break;
                    case 'c':
                        pc();
                        break;
                    case 'L':
                        pL();
                        break;
                    case 'C':
                        pC();
                        break;
                    case 'a':
                        pa();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'a':
                switch (object) {
                    case 'r':
                        ar();
                        break;
                    case 'c':
                        ac();
                        break;
                    case 'm':
                        am();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'm':
                switch (object) {
                    case 'r':
                        mr();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'd':
                switch (object) {
                    case 'r':
                        dr();
                        break;
                    case 'c':
                        dc();
                        break;
                    case 'm':
                        dm();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'c':
                switch (object) {
                    case 'L':
                        if (cL() == 1) {
                            printf("All records deleted\n");
                        }
                    break;
                    case 'C':
                        cC();
                        printf("All collections deleted\n");
                        break;
                    case 'A':
                        cA();
                        printf("All data deleted\n");
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 's':
                switch (object) {
                    case 'A':
                        sA();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'r':
                switch (object) {
                    case 'A':
                        rA();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
                break;
            case 'q':
                switch (object) {
                    case 'q':
                        qq();
                        break;
                    default:
                        printf("%s", command_invalid);
                        discard_input_remainder();
                        break;
                }
            default:
                printf("%s", command_invalid);
                discard_input_remainder();
                break;
        }
    }
    return 0;
}

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
        if (*title != '\0') {
            *title_iterator++ = ' ';
        }
    }
    if (isspace(*(--title))) {
        *(title_iterator-1) = '\0';
    }
    else {
        *title_iterator = '\0';
    }
    return valid;
}

void discard_input_remainder(void)
{
    while (getchar() != '\n') {
        ;
    }
}
