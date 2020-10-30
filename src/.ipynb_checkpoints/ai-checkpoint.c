#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "stack.h"


bool same_board(state_t *state1, state_t *state2){
	for(int j =0;j<SIZE;j++){
		for(int k = 0;k<SIZE;k++){
			if(!(state1->field[j][k]==state2->field[j][k])){
				return false;
			}
		}
	}
	return true;
}

bool check_if_in( node_t* new_node){

	for(int i =0;i<=stack_top_idx;i++){
		for(int j =0;j<SIZE;j++){
			for(int k = 0;k<SIZE;k++){
				if(same_board(&stack[stack_top_idx]->state,&new_node->state)){
					return true;
				}
			}
		}
		
		
	}
	return false;
	
    //FILL IN THE GRAPH ALGORITHM
}



void copy_state(state_t* dst, state_t* src){
	
	//Copy field
	//printf("\nhello\n");
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

    //printf("hello  1 \n");
    //node_t * new_node = (node_t *) malloc(sizeof(node_t));

	new_node->parent = n;
    
	copy_state(&new_node->state,&n->state);

	//printf("\nafter copy state\n");
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
	//free(init_state);
    stack_push(n);
    int rempegs = num_pegs(&n->state);

    while(!is_stack_empty()){

		n = stack_top();
		//printf("\ndepth of node: %d\n",n->depth);

        stack_pop();
		
        expanded_nodes++;
		printf("\nExpand nodes: %d\n",expanded_nodes);
        
        if (num_pegs(&n->state)<rempegs){
			//printf("\ninside save soln\n");
			//printf("\nDepth: %d\n",n->depth);
            save_solution(n);
			//printf("\nafter maybe\n");
            rempegs = num_pegs(&n->state);
			

        }
		

        for(int i =0;i<SIZE;i++){
            for(int j = 0;j<SIZE;j++){

                //printf("\n %d %d pegs inside for loop: %d\n",i,j,num_pegs(&n->state));
				(&n->state)->cursor.x = i;
                (&n->state)->cursor.y = j;

				
				
                if(can_apply(&n->state,&(&n->state)->cursor,left)){
					
					node_t * new_node = (node_t *) malloc(sizeof(node_t));
                    new_node = applyAction(n,&(&n->state)->cursor,left,new_node);
                    
					generated_nodes++;
                    
					printf("\nGenerated nodes: %d i = %d j = %d Top index: %d\n",generated_nodes,i,j,stack_top_idx);
					if(won(&new_node->state)){
							save_solution(new_node);
							rempegs = num_pegs(&new_node->state);
							
							ht_clear(&table);
							free(new_node);
							while(n!=NULL){
								node_t* new_node2;
								new_node2 = n;
								free(new_node2);
								n = n->parent;
							}
							//free(n);
							//printf("\nTop index of stack: %d\n", stack_top_idx);
							printf("\nTop index of stack: %d\n", stack[stack_top_idx]->depth);
							free_stack();
							//printf("\nRemaining Pegs: %d\n\n",rempegs);
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
                if(can_apply(&n->state,&(&n->state)->cursor,right)){
				
					//printf("\nright\n");
					node_t * new_node = (node_t *) malloc(sizeof(node_t));
					new_node = applyAction(n,&(&n->state)->cursor,right,new_node);
                    generated_nodes++;
                    printf("\nGenerated nodes: %d i = %d j = %d Top index: %d\n",generated_nodes,i,j,stack_top_idx);

					if(won(&new_node->state)){
							//printf("\nWon\n");
							save_solution(new_node);
							rempegs = num_pegs(&new_node->state);
							ht_clear(&table);
							free(new_node);
							while(n!=NULL){
								node_t* new_node2;
								new_node2 = n;
								free(new_node2);
								n = n->parent;
							}
							//free(n);
							printf("\nTop index of stack: %d\n", stack[stack_top_idx]->depth);
							free_stack();
							//printf("\nRemaining Pegs: %d\n\n",rempegs);
							return;
					}

					if(!ht_contains(&table,(void*)(&new_node->state))){
						stack_push(new_node);
						ht_insert(&table,(void*)(&new_node->state),(void*)(&new_node->state));
					}
					else{
						free(new_node);
					}
					//printf("\nRemaining Pegs: %d\n\n",num_pegs(&new_node->state));
					//n = new_node;
                    
                }
                if(can_apply(&n->state,&(&n->state)->cursor,up)){
                   
					//printf("\nup\n");
					node_t * new_node = (node_t *) malloc(sizeof(node_t));
					new_node  = applyAction(n,&(&n->state)->cursor,up,new_node);
                    generated_nodes++;
					printf("\nGenerated nodes: %d i = %d j = %d Top index: %d\n",generated_nodes,i,j,stack_top_idx);
                    
					if(won(&new_node->state)){
							save_solution(new_node);
							rempegs = num_pegs(&new_node->state);
							ht_clear(&table);
							free(new_node);
							while(n!=NULL){
								node_t* new_node2;
								new_node2 = n;
								free(new_node2);
								n = n->parent;
							}
							//free(n);
							//printf("\nTop index of stack: %d\n", stack_top_idx);
							printf("\nTop index of stack: %d\n", stack[stack_top_idx]->depth);
							free_stack();
							//printf("\nRemaining Pegs: %d\n\n",rempegs);
							return;
					}

					if(!ht_contains(&table,(void*)(&new_node->state))){
						stack_push(new_node);
						ht_insert(&table,(void*)(&new_node->state),(void*)(&new_node->state));
					}
					else{
						free(new_node);
					}
					//printf("\nRemaining Pegs: %d\n\n",num_pegs(&new_node->state));
					//n = new_node;
                    
                }
                if(can_apply(&n->state,&(&n->state)->cursor,down)){
                   
					//printf("\ndown\n");
					node_t * new_node = (node_t *) malloc(sizeof(node_t));
					new_node = applyAction(n,&(&n->state)->cursor,down,new_node);
                    generated_nodes++;
					printf("\nGenerated nodes: %d i = %d j = %d Top index: %d\n",generated_nodes,i,j,stack_top_idx);
                   
					if(won(&new_node->state)){
							save_solution(new_node);
							rempegs = num_pegs(&new_node->state);
							ht_clear(&table);
							free(new_node);
							while(n!=NULL){
								node_t* new_node2;
								new_node2 = n;
								free(new_node2);
								n = n->parent;
							}
							//free(n);
							//printf("\nTop index of stack: %d\n", stack_top_idx);
							printf("\nTop index of stack: %d\n", stack[stack_top_idx]->depth);
							free_stack();
							//printf("\nRemaining Pegs: %d\n\n",rempegs);
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


		//printf("\nloops over\n");
        if(expanded_nodes>=budget){
            return;
        }

		//free(n);
		n= NULL;
		
	
	}
	
	ht_clear(&table);
	while(n!=NULL){
		node_t* new_node2;
		new_node2 = n;
		free(new_node2);
		n = n->parent;
	}
	//free(n);
	free_stack();
	

}











