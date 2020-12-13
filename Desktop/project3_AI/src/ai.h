#ifndef __AI__
#define __AI__

#include <stdint.h>
#include <unistd.h>
#include "utils.h"

typedef struct ll_node ll_node_t;

struct ll_node{
    node_t* data;
    ll_node_t* next;
};

typedef struct{
    ll_node_t* head;
    ll_node_t* foot;
} list_t;

void initialize_ai();
void find_solution( state_t* init_state );
void free_memory(unsigned expanded_nodes);

list_t *insert_at_foot(list_t *list, node_t* value);
list_t *make_empty_list(void);
void free_list(list_t *list);


#endif

