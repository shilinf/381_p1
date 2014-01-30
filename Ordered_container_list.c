#include "Ordered_container.h"
#include "p1_globals.h"
#include "Utility.h"
#include <stdlib.h>

/* struct LL_Node structure declaration. This declaration is local to this file. 
This is a two-way or doubly-linked list. Each node has a pointer to the previous 
node and another pointer to the next node in the list. This means insertions or
removals of a node can be made in constant time, once the location has been
determined. */
struct LL_Node { 
    struct LL_Node* prev;      /* pointer to the previous node */
	struct LL_Node* next;		/* pointer to the next node */
	void* data_ptr; 			/* uncommitted pointer to the data item */
};

/* Declaration for Ordered_container. This declaration is local to this file.  
A pointer is maintained to the last node in the list as well as the first, 
meaning that additions to the end of the list can be made in constant time. 
The number of nodes in the list is kept up-to-date in the size member
variable, so that the size of the list can be accessed in constant time. */
struct Ordered_container {
	OC_comp_fp_t comp_func;
	struct LL_Node* first;
	struct LL_Node* last;
	int size;
};

void OC_initial(struct Ordered_container *c_ptr);
void OC_clear_items(struct Ordered_container* c_ptr);
void OC_free_LL_Node(struct LL_Node *node);

struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
    /* the deallocation will be done when the function 
     OC_destroy_container called.*/
    struct Ordered_container *c_ptr =
    malloc_guard(sizeof(struct Ordered_container));
    OC_initial(c_ptr);
    c_ptr->comp_func = f_ptr;
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

/* set some initial value for container */
void OC_initial(struct Ordered_container *c_ptr)
{
    c_ptr->size = 0;
    c_ptr->first = NULL;
    c_ptr->last = NULL;
}

/* clear all nodes in the container */
void OC_clear_items(struct Ordered_container* c_ptr)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while (node_iterator) {
        struct LL_Node *node_next = node_iterator->next;
        OC_free_LL_Node(node_iterator);
        node_iterator = node_next;
    }
}

/* free the memory for nodes and change the global variables */
void OC_free_LL_Node(struct LL_Node *node)
{
    free(node);
    g_Container_items_in_use--;
    g_Container_items_allocated--;
}

int OC_get_size(const struct Ordered_container* c_ptr)
{
    return c_ptr->size;
}

int OC_empty(const struct Ordered_container* c_ptr)
{
    return !c_ptr->size;
}

void* OC_get_data_ptr(const void* item_ptr)
{
    return ((struct LL_Node *)item_ptr)->data_ptr;
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
    struct LL_Node *node = item_ptr;
    if(!node->prev && !node->next) {
        c_ptr->first = NULL;
        c_ptr->last = NULL;
    }
    else if(!node->prev) {
        c_ptr->first = node->next;
        node->next->prev = NULL;
    }
    else if(!node->next) {
        c_ptr->last = node->prev;
        node->prev->next = NULL;
    }
    else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    OC_free_LL_Node(node);
    c_ptr->size--;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
    /* the deallocation will be done when the function OC_free_LL_Node called */
    struct LL_Node *new_node = malloc_guard(sizeof(struct LL_Node));
    new_node->data_ptr = data_ptr;
    /* insert node is the first node */
    if(OC_empty(c_ptr)) {
        new_node->prev = NULL;
        new_node->next = NULL;
        c_ptr->first = new_node;
        c_ptr->last = new_node;
    }
    /* insert node in the beginning */
    else if(c_ptr->comp_func(c_ptr->first->data_ptr, data_ptr) >= 0) {
        c_ptr->first->prev = new_node;
        new_node->next = c_ptr->first;
        new_node->prev = NULL;
        c_ptr->first = new_node;
    }
    /* insert node in the end */
    else if(c_ptr->comp_func(c_ptr->last->data_ptr, data_ptr) <= 0){
        c_ptr->last->next = new_node;
        new_node->prev = c_ptr->last;
        new_node->next = NULL;
        c_ptr->last = new_node;
    }
    else {
        struct LL_Node *node_iterator = c_ptr->first;
        while(c_ptr->comp_func(node_iterator->data_ptr, data_ptr) < 0) {
            node_iterator = node_iterator->next;
        }
        node_iterator->prev->next = new_node;
        new_node->prev = node_iterator->prev;
        new_node->next = node_iterator;
        node_iterator->prev = new_node;
    }
    c_ptr->size++;
    g_Container_items_in_use++;
    g_Container_items_allocated++;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while(node_iterator) {
        if (c_ptr->comp_func(data_ptr, node_iterator->data_ptr) < 0)
            return NULL;
        if(!c_ptr->comp_func(node_iterator->data_ptr, data_ptr))
            return node_iterator;
        node_iterator = node_iterator->next;
    }
    return NULL;
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr,
                       const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while(node_iterator) {
        if (fafp(arg_ptr, node_iterator->data_ptr) < 0)
            return NULL;
        if(!fafp(arg_ptr, node_iterator->data_ptr))
            return node_iterator;
        node_iterator = node_iterator->next;
    }
    return NULL;
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while (node_iterator) {
        afp(node_iterator->data_ptr);
        node_iterator = node_iterator->next;
    }
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while (node_iterator) {
        int afp_return_value = afp(node_iterator->data_ptr);
        if (afp_return_value)
            return afp_return_value;
        node_iterator = node_iterator->next;
    }
    return 0;
}


void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp,
                  void* arg_ptr)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while (node_iterator) {
        afp(node_iterator->data_ptr, arg_ptr);
        node_iterator = node_iterator->next;
    }
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr,
                    OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
    struct LL_Node *node_iterator = c_ptr->first;
    while (node_iterator) {
        int afp_return_value = afp(node_iterator->data_ptr, arg_ptr);
        if (afp_return_value)
            return afp_return_value;
        node_iterator = node_iterator->next;
    }
    return 0;
}
