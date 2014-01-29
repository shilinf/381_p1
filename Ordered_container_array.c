#include <stdlib.h>
#include "Ordered_container.h"
#include "p1_globals.h"
#include "Utility.h"

/* for all malloc, need to check whether enough space. */


int binary_search_helper(const struct Ordered_container* c_ptr, const void* search_ptr, int *insertion_position, int (*comp_function) (const void* arg1, const void* arg2));

void OC_initial(struct Ordered_container* c_ptr);
void OC_clear_items(struct Ordered_container *c_ptr);

/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container {
	OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
	void** array;			/* pointer to array of pointers to void */
	int allocation;			/* current size of array */
	int size;				/* number of items  currently in the array */
};

struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
    /* the deallocation will be done when the function OC_destroy_container called.*/
    struct Ordered_container *c_ptr = malloc_guard(sizeof(struct Ordered_container));
    c_ptr->comp_fun = f_ptr;
    OC_initial(c_ptr);
    g_Container_count++;
    return c_ptr;
}

void OC_destroy_container(struct Ordered_container* c_ptr)
{
    OC_clear_items(c_ptr);
    free(c_ptr);
    g_Container_count--;
}

void OC_clear(struct Ordered_container* c_ptr)
{
    OC_clear_items(c_ptr);
    OC_initial(c_ptr);
}

void OC_initial(struct Ordered_container* c_ptr)
{
    /* the deallocation will be done when the function OC_destroy_container or OC_clear called.*/
    c_ptr->array = malloc_guard(3 * sizeof(void *));
    c_ptr->size = 0;
    c_ptr->allocation = 3;
    g_Container_items_allocated += c_ptr->allocation;
}

void OC_clear_items(struct Ordered_container *c_ptr)
{
    free(c_ptr->array);
    g_Container_items_in_use -= c_ptr->size;
    g_Container_items_allocated -= c_ptr->allocation;
}

int OC_get_size(const struct Ordered_container* c_ptr)
{
    return c_ptr->size;
}

int OC_empty(const struct Ordered_container* c_ptr)
{
    return (c_ptr->size == 0) ? 1 : 0;
}

void* OC_get_data_ptr(const void* item_ptr)
{
    return *(void **)item_ptr;
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
    void **array_iterator = (void **) item_ptr;
    void **array_last = c_ptr->array + c_ptr->size - 1;
    c_ptr->size--;
    while (array_iterator != array_last) {
        *array_iterator = *(array_iterator + 1);
        ++array_iterator;
    }
    g_Container_items_in_use--;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
    int i, insertion_position;
    int item_position =binary_search_helper(c_ptr, data_ptr, &insertion_position, c_ptr->comp_fun);
    if (item_position != -1)
        insertion_position = item_position;
    c_ptr->size++;
    if (c_ptr->size > c_ptr->allocation) {
        void **new_array;
        g_Container_items_allocated += c_ptr->allocation + 2;
        c_ptr->allocation = 2 * (c_ptr->allocation + 1);
        
        /* the deallocation will be done when the function OC_destroy_container or OC_clear called.*/
        new_array = malloc_guard(c_ptr->allocation * sizeof(void *));
        for (i = 0; i < insertion_position; i++)
            new_array[i] = c_ptr->array[i];
        new_array[insertion_position] = data_ptr;
        for (i = insertion_position+1; i < c_ptr->size; i++)
            new_array[i] = c_ptr->array[i-1];
        free(c_ptr->array);
        c_ptr->array = new_array;
    }
    else {
        for (i = c_ptr->size - 1; i > insertion_position; i--)
            c_ptr->array[i] = c_ptr->array[i-1];
        c_ptr->array[insertion_position] = data_ptr;
    }
    g_Container_items_in_use++;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
    int insertion_position;
    int item_position =binary_search_helper(c_ptr, data_ptr, &insertion_position, c_ptr->comp_fun);
    if (item_position == -1)
        return  NULL;
    else
        return c_ptr->array + item_position;
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr, const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
    int insertion_position;
    int item_position =binary_search_helper(c_ptr, arg_ptr, &insertion_position, fafp);
    if (item_position == -1)
        return  NULL;
    else
        return c_ptr->array + item_position;
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
    int i;
    for (i = 0; i < c_ptr->size; i++)
        afp(c_ptr->array[i]);
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
    int i;
    for (i=0; i < c_ptr->size; i++) {
        int afp_return_value = afp(c_ptr->array[i]);
        if (afp_return_value != 0)
            return afp_return_value;
    }
    return 0;
}

void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp, void* arg_ptr)
{
    int i;
    for (i = 0; i < c_ptr->size; i++)
        afp(c_ptr->array[i], arg_ptr);
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr, OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
    int i;
    for (i=0; i < c_ptr->size; i++) {
        int afp_return_value = afp(c_ptr->array[i], arg_ptr);
        if (afp_return_value != 0)
            return afp_return_value;
    }
    return 0;
}

/* do a generic function for comparison in binary search */
int binary_search_helper(const struct Ordered_container* c_ptr, const void* search_ptr, int *insertion_position, int (*comp_function) (const void* arg1, const void* arg2))
{
    int low,high,mid;
    low = 0;
    high = c_ptr->size-1;
    while(low <= high) {
        int comp_result;
        mid = (low+high) / 2;
        comp_result = comp_function(search_ptr, c_ptr->array[mid]);
        if(comp_result < 0)
            high = mid - 1;
        else if(comp_result > 0)
            low = mid + 1;
        else    /*found match*/
            return mid;
    }
    *insertion_position = high + 1; /*no match, set the insertion position */
    return -1;  /*no match */
}
