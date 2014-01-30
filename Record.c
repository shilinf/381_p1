#include "Record.h"
#include "p1_globals.h"
#include "Utility.h"
#include <stdlib.h>
#include <string.h>

/* a Record contains an int ID, rating, and pointers to C-strings for the 
 title and medium */
struct Record {
	char* title;
	int ID;
	char* medium;
	int rating;
};

static int id_number_counter = 0;

struct Record* create_Record(const char* medium, const char* title)
{
    /* the deallocation will be done when the function destroy_Record called.*/
    struct Record *new_record = malloc_guard(sizeof(struct Record));
    new_record->ID = ++id_number_counter;
    new_record->rating = 0;
    new_record->title = malloc_guard(strlen(title) + 1);
    g_string_memory += strlen(title) + 1;
    strcpy(new_record->title, title);
    new_record->medium = malloc_guard(strlen(medium) + 1);
    g_string_memory += strlen(medium) + 1;
    strcpy(new_record->medium, medium);
    return new_record;
}

void destroy_Record(struct Record* record_ptr)
{
    g_string_memory -= strlen(record_ptr->title) + 1;
    g_string_memory -= strlen(record_ptr->medium) + 1;
    free(record_ptr->title);
    free(record_ptr->medium);
    free(record_ptr);
}

int get_Record_ID(const struct Record* record_ptr)
{
    return record_ptr->ID;
}

const char* get_Record_title(const struct Record* record_ptr)
{
    return record_ptr->title;
}

void set_Record_rating(struct Record* record_ptr, int new_rating)
{
    record_ptr->rating = new_rating;
}

void print_Record(const struct Record* record_ptr)
{
    if (!record_ptr->rating)
        printf("%d: %s u %s\n", record_ptr->ID, record_ptr->medium,
               record_ptr->title);
    else
        printf("%d: %s %d %s\n", record_ptr->ID, record_ptr->medium,
               record_ptr->rating, record_ptr->title);
}

void save_Record(const struct Record* record_ptr, FILE* outfile)
{
    fprintf(outfile, "%d %s %d %s\n", record_ptr->ID, record_ptr->medium,
            record_ptr->rating, record_ptr->title);
}

struct Record* load_Record(FILE* infile)
{
    int record_id, rating;
    char medium[RECORD_MEDIUM_SIZE];
    char title[RECORD_TITLE_SIZE];
    struct Record *new_record;
    char fmt_str[30];
    sprintf(fmt_str, "%%d %%%ds %%d", RECORD_MEDIUM_SIZE-1);
    if (fscanf(infile, fmt_str, &record_id, medium, &rating) != 3)
        return NULL;
    fgetc(infile); /* read the leading space */
    if (fgets(title, RECORD_TITLE_SIZE, infile) == NULL) {
        return NULL;
    }
    title[strlen(title)-1] = '\0'; /* remove the newline character */
    new_record = create_Record(medium, title);
    set_Record_rating(new_record, rating);
    new_record->ID = record_id;
    id_number_counter--;
    /* set id_number counter to the largest record_id read in */
    if (record_id > id_number_counter)
        id_number_counter = record_id;
    return new_record;
}

void reset_Record_ID_counter(void)
{
    id_number_counter = 0;
}
