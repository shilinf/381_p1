#ifndef P1_GLOBALS_H
#define P1_GLOBALS_H

#define RECORD_TITLE_SIZE 64
#define COLLECTION_NAME_SIZE 16
#define COLLECTION_NAME_FMT "%15s"
#define RECORD_MEDIUM_SIZE 8
#define RECORD_MEDIUM_FMT "%7s"


extern int g_Container_count;		/* number of Ordered_containers currently allocated */
extern int g_Container_items_in_use;	/* number of Ordered_container items currently in use */
extern int g_Container_items_allocated;	/* number of Ordered_container items currently allocated */
extern int g_string_memory;			/* number of bytes used in C-strings */


#endif
