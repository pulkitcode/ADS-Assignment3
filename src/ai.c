#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "stack.h"


void copy_state(state_t* dst, state_t* src){
	
	memcpy( dst->field, src->field, SIZE*SIZE*sizeof(int8_t) );

	dst->cursor = src->cursor;
	dst->selected = src->selected;
}


node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;	
	new_n->depth = 0;
	copy_state(&(new_n->state), init_state);
	return new_n;
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

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
 node_t* applyAction(node_t* n, position_s* selected_peg, move_t action ,node_t * new_node){


	new_node->parent = n;
    
	copy_state(&new_node->state,&n->state);

	new_node->depth = n->depth+1;;

	new_node->move = action;
    
    execute_move_t( &(new_node->state), &(new_node->state.cursor), action );
	
    return new_node;

}

/**
 * Find a solution path as per algorithm description in the handout
 */

void find_solution( state_t* init_state  ){

    HashTable table;


    // Choose initial capacity of PRIME NUMBER 
    // Specify the size of the keys and values you want to store once 
    ht_setup( &table, sizeof(int8_t) * SIZE * SIZE, sizeof(int8_t) * SIZE * SIZE, 16769023);

    // Initialize Stack
    initialize_stack();

    //Add the initial node
    node_t* n = create_init_node( init_state );
    stack_push(n);
    int rempegs = num_pegs(&n->state);

    while(!is_stack_empty()){

		n = stack_top();

		stack_pop();
		
        expanded_nodes++;
        
        if (num_pegs(&n->state)<rempegs){
            save_solution(n);
            rempegs = num_pegs(&n->state);
			
        }
	
        for(int i =0;i<SIZE;i++){
            for(int j = 0;j<SIZE;j++){

                
				(&n->state)->cursor.x = i;
                (&n->state)->cursor.y = j;

				for(move_t k =left;k<=down;k++){
					
				
					if(can_apply(&n->state,&(&n->state)->cursor,k)){

						
						node_t * new_node = (node_t *) malloc(sizeof(node_t));
						new_node = applyAction(n,&(&n->state)->cursor,k,new_node);
						

						generated_nodes++;

						if(won(&new_node->state)){
								save_solution(new_node);
								rempegs = num_pegs(&new_node->state);

								ht_destroy(&table);
								free(new_node);
								return;
						}

						if(!ht_contains(&table,(void*)(&new_node->state))){
							
							stack_push(new_node);
							ht_insert(&table,(void*)(&new_node->state),(void*)(&new_node->state));
						}
						else{
							
							free(new_node);
						}

					}

				}
			}

		}
		
		
        if(expanded_nodes>=budget){
			ht_destroy(&table);
			free_stack();
			free(n);
            return;
        }

	
	}
	
	
	ht_destroy(&table);
	free_stack();
	free(n);
	

}










