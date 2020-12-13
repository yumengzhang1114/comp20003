#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "stack.h"

void copy_state(state_t* dst, state_t* src){
    
    //Copy field
    memcpy( dst->field, src->field, SIZE*SIZE*sizeof(int8_t) );

    dst->cursor = src->cursor;
    dst->selected = src->selected;
}

/**
 * Saves the path up to the node as the best solution found so far
*/
void save_solution( node_t* solution_node ){
    node_t* n = solution_node;
    while( n->parent != NULL ){
        copy_state( &(solution[n->depth]), &(n->state) );
        solution_moves[n->depth-1] = n->move;

        n = n->parent;
    }
    solution_size = solution_node->depth;
}


node_t* create_init_node( state_t* init_state ){
    node_t * new_n = (node_t *) malloc(sizeof(node_t));
    new_n->parent = NULL;
    new_n->depth = 0;
    copy_state(&(new_n->state), init_state);
    return new_n;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
node_t* applyAction(node_t* n, position_s* selected_peg, move_t action ){

    node_t* new_node = NULL;
    
    //FILL IN MISSING CODE
    
    //create a new node and points to the parent
    new_node = create_init_node(&(n->state));
    new_node->parent = n;
    
    //update the state with the action chosen
    new_node->state.cursor.x = selected_peg->x;
    new_node->state.cursor.y = selected_peg->y;
    execute_move_t( &(new_node->state), &(new_node->state.cursor), action);
    
    //update the depth of the node
    new_node->depth = n->depth + 1;
    
    //update the action used to create the node
    new_node->move = action;

    return new_node;

}

/**
 * Find a solution path as per algorithm description in the handout
 */

void find_solution( state_t* init_state ){
    
    HashTable table;

    // Choose initial capacity of PRIME NUMBER
    // Specify the size of the keys and values you want to store once
    ht_setup( &table, sizeof(int8_t) * SIZE * SIZE, sizeof(int8_t) * SIZE * SIZE, 16769023);

    // Initialize Stack
    initialize_stack();

    // Add the initial node
    node_t* n = create_init_node( init_state );
    
    // push the intial node to stack and insert to hash table
    // for future repetitive check
    stack_push(n);
    ht_insert(&table,&(n->state),&(n->depth));
    
    //get the number pf remaining pegs in the initial state
    int remaining_pegs = num_pegs(&(n->state));
    
    //the linked list is used to free the memory
    list_t* list = make_empty_list();
    
    node_t* new_node;
    position_s position;
    while(!is_stack_empty()){
        //when we pop an item from the stack, we add it into the
        //linked list to avoid overlap in the stack so that the
        // allocated memory cannot be freed.
        n = stack_top();
        stack_pop();
        insert_at_foot(list,n);
        expanded_nodes = expanded_nodes + 1;
        
        //found a better solution
        if(num_pegs(&(n->state)) < remaining_pegs){
            save_solution(n);
            remaining_pegs = num_pegs(&(n->state));
        }
        // iterate through every possibile move to find every
        // possible solution
        for(int8_t i = 0;i < SIZE; i++){
            for(int8_t j = 0;j < SIZE; j++){
                for(int k = 0; k < NUMBER_OF_DIRECTION; k++){
                    position.x = i;
                    position.y = j;
                    // if legal action, create a new node
                    if(can_apply(&(n->state),&position,k)){
                        new_node = applyAction(n,&position,k);
                        generated_nodes = generated_nodes + 1;
                        
                        // if the game is solved, save the solution, free
                        // all memory and the function terminate.
                        if(won(&(new_node->state))){
                            save_solution(new_node);
                            remaining_pegs = num_pegs(&(new_node->state));
                            //free the node created, hash table, stack and linked list
                            free(new_node);
                            ht_destroy(&table);
                            free_stack();
                            free_list(list);
                            return;
                        }
                        // avoid duplicates.if not contain in hash table, push it into
                        // stack and insert into hash table. otherwise destroy the node
                        if(!ht_contains(&table,&(new_node->state))){
                            stack_push(new_node);
                            ht_insert(&table,&(new_node->state),&(new_node->depth));
                        }else{free(new_node);}
                   }
                }
             }
        }
        //budget exhusted,free all memory and the function terminate
        if(expanded_nodes >= budget){
            ht_destroy(&table);
            free_stack();
            free_list(list);
            return;
        }
    }
    ht_destroy(&table);
    free_stack();
    free_list(list);
}
    

//the linked list is used to free the memory


/* this function is borrowed from chapter 10.2 in the book Programming,
   Problem solving, and Abstaction with C by Alistair Moffat.
   It is used to make an empty list */
list_t
*make_empty_list(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list != NULL);
    list->head = list->foot = NULL;
    return list;
}

/* the idea of this function is borrowed from chapter 10.2 in the
book Programming,Problem solving, and Abstaction with C by Alistair
Moffat. It is used to insert a node at the foot of the linked list */
list_t
*insert_at_foot(list_t *list, node_t* value) {
    ll_node_t* new;
    new = (ll_node_t*)malloc(sizeof(ll_node_t));
    assert(list != NULL && new != NULL);
    new->data = value;
    new->next = NULL;
    if (list->foot == NULL) {
        /* this is the first insertion into the list */
        list->head = list->foot = new;
    } else {
        list->foot->next = new;
        list->foot = new;
    }
    return list;
}

/*  this function is used to free the entire linked list */
void
free_list(list_t *list) {
    ll_node_t *curr, *prev;
    assert(list != NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev->data);
        free(prev);
    }
    free(list);
}




