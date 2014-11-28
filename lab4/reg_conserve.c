

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reg_conserve.h"

#define DEBUG_CONSERVE 1

//allowed predefined output
int is_predefined_output(char *output){
	if(strcmp(output, "result.color") == 0 ||
       strcmp(output, "result.depth") == 0  )
		return 1;
	return 0;
}


//allowed predefined input
int is_predefined_input(char *input){

	if(		strcmp(input, "fragment.position") == 0 			||
			strcmp(input, "fragment.texcoord") == 0 			||
			strcmp(input, "fragment.color") == 0 				||
			strcmp(input, "fragment.color.secondary") == 0 			||
			strcmp(input, "fragment.fogcoord") == 0 		||

			strcmp(input, "state.light[0].half") == 0 			||
			strcmp(input, "state.lightmodel.ambient") == 0 		||
			strcmp(input, "state.material.shininess") == 0	||

			strcmp(input, "program.env[1]") == 0 					||
			strcmp(input, "program.env[2]") == 0 					||
			strcmp(input, "program.env[3]") == 0)
		return 1;

	return 0;
}

int is_one_input(op_kind op){
	if(op == COS || //op == EX2 ||
	   op == FLR || op == FRC || op == KIL || op == LG2 ||
	   op == LIT || op == MOV || op == RCP || op == RSQ || op == SCS || op == SIN ||
	   op == SWZ){
		return 1;
	}
	return 0;
}

int is_two_input(op_kind op){
	if(op == ADD || op == DP3 || op == DP4 || op == DPH || //op == DST ||
	   op == MAX ||
	   op == MIN || op == MUL || op == POW || op == SGE || op == SLT || op == SUB ||
	   op == XPD){
		return 1;
	}
	return 0;
}

int is_three_input(op_kind op){
	if(op == CMP || op == LRP || op == MAD || op == TEX || op == TXB || op == TXP){
		return 1;
	}
	return 0;
}


reg_list headEntry = {NULL, NULL, 0, NULL};
reg_list *reghead = &headEntry;
//reg_list *last = head;

void add_name(char *name){
	if(name == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Assembly instruction: encountered a NULL register name.\n");
		return;
	}

	reg_list *cur = reghead;
	while(cur->next){
		cur = cur->next;
		if(strcmp(cur->name, name) == 0){
			errorOccurred = 1;
			fprintf(errorFile,"Tried to add the same register name twice to the active list.\n");
			return;
		}
	}

	cur = (reg_list *) malloc(sizeof(reg_list));
	cur->name = name;
	cur->is_free = 0;
	cur->replaced_name = NULL;

	cur->next = reghead->next;
	reghead->next = cur;

	if(DEBUG_CONSERVE){
		int i;
		for(i=0;i<10;i++){
			char str[10];
			sprintf(str, "%d", i);
			printf("%d", is_predefined_input(str));
		}
		printf("\nAdding name fin\n");
	}

}

void free_name(char *name){
	reg_list *cur = reghead;
	while(cur->next){
		cur = cur->next;
		if(strcmp(cur->name, name) == 0){
			cur->is_free = 1;
			if(cur->replaced_name){
				free(cur->replaced_name); //The mapped variable no longer appears anywhere and the string isnt needed.
			}
			cur->replaced_name = NULL;
			return;
		}
	}

	errorOccurred = 1;
	fprintf(errorFile,"Tried to free a name not in the active list.\n");
}

void free_name_struct(reg_list *cur){

	cur->is_free = 1;
	if(cur->replaced_name){
		free(cur->replaced_name); //The mapped variable no longer appears anywhere and the string isnt needed.
	}
	cur->replaced_name = NULL;
}

reg_list * get_free_name(void){

	reg_list *cur = reghead;
	while(cur->next){
		cur = cur->next;
		if(cur->is_free == 1){
			cur->is_free = 0;
			return cur;
		}
	}

	return NULL;
}

reg_list * get_name_that_maps_to(char *name){

	reg_list *cur = reghead;
	while(cur->next){
		cur = cur->next;
		if(strcmp(cur->replaced_name, name) == 0){
			return cur;
		}
	}

	return NULL;
}

//call after checking that it isnt used in the same instruction.
//call on your current isntruction
int never_used_again(char *name, instr *cur){

	if(DEBUG_CONSERVE){
		int i;
		for(i=0;i<10;i++){
			char str[10];
			sprintf(str, "%d", i);
			printf("%d", is_predefined_input(str));
		}
		printf("\nEntering never used again check\n");
	}

	while(cur->next){

		cur = cur->next;



		if(cur->kind == OPERATION){
			if(strcmp(cur->in1, name) == 0)
				return 0;

			if((is_two_input(cur->op) || is_three_input(cur->op)) && strcmp(cur->in2, name) == 0)
				return 0;

			if(is_three_input(cur->op) && strcmp(cur->in3, name) == 0)
				return 0;


		}

		if(strcmp(cur->out, name) == 0)
						return 0;

	}

	return 1;
}



void conserve_reg(instr *cur){

	if(DEBUG_CONSERVE){
		int i;
		for(i=0;i<10;i++){
			char str[10];
			sprintf(str, "%d", i);
			printf("%d", is_predefined_input(str));
		}
		printf("\nEntered conserve\n");
	}

	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Assembly register conservation function was called on an empty assembly instruction list.\n");
		return;
	}

	instr *prev = cur;
	while(cur){

		if(cur->kind == DECLARATION){

			if(DEBUG_CONSERVE){
				int i;
				for(i=0;i<10;i++){
					char str[10];
					sprintf(str, "%d", i);
					printf("%d", is_predefined_input(str));
				}
				printf("\nDeclaration\n");
			}

			reg_list *free_reg = get_free_name();

			if(free_reg){
				if(DEBUG_CONSERVE){
					int i;
					for(i=0;i<10;i++){
						char str[10];
						sprintf(str, "%d", i);
						printf("%d", is_predefined_input(str));
					}
					printf("\nFound free\n");
				}

				free_reg->replaced_name = cur->out;
				free_reg->is_free = 0;

				//Destroying the instruction. NEEED TO FREE NAMES????? No.
				prev->next = cur->next;
				free(cur);
			}
			else{
				if(DEBUG_CONSERVE){
					int i;
					for(i=0;i<10;i++){
						char str[10];
						sprintf(str, "%d", i);
						printf("%d", is_predefined_input(str));
					}
					printf("\nNew active\n");
				}

				add_name(cur->out);
				if(never_used_again(cur->out, cur)){
					free_name(cur->out);
				}
			}
		}
		else{//OPERATION
			reg_list *established_name;

			if(!is_predefined_input(cur->in1)){

				established_name = get_name_that_maps_to(cur->in1);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING?
					char *premapped = cur->in1;
					cur->in1 = established_name->name;

					//Checking if we can free this var.
					if((is_one_input(cur->op) ||
						(is_two_input(cur->op) && strcmp(premapped, cur->in2) != 0) ||
						(is_three_input(cur->op) && strcmp(premapped, cur->in2) != 0 && strcmp(premapped, cur->in3) != 0)
					   ) &&
					   strcmp(premapped, cur->out) != 0 &&
					   never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free

					//Checking if we can free this var.
					if((is_one_input(cur->op) ||
						(is_two_input(cur->op) && strcmp(cur->in1, cur->in2) != 0) ||
						(is_three_input(cur->op) && strcmp(cur->in1, cur->in2) != 0 && strcmp(cur->in1, cur->in3) != 0)
					   ) &&
					   strcmp(cur->in1, cur->out) != 0 &&
					   never_used_again(cur->in1, cur)){
						free_name(cur->in1);
					}


				}
			}

			if((is_two_input(cur->op) || is_three_input(cur->op)) &&
			   (cur->op != MOV || (cur->op == MOV && cur->in2[0] != '{')) &&
			    !is_predefined_input(cur->in2)){

				established_name = get_name_that_maps_to(cur->in2);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING? Only on last iteration
					char *premapped = cur->in2;
					cur->in2 = established_name->name;

					//Checking if we can free this var.
					if((is_two_input(cur->op) ||
						(is_three_input(cur->op) && strcmp(premapped, cur->in3) != 0)
					   ) &&
					   strcmp(premapped, cur->out) != 0 &&
					   never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free

					//Checking if we can free this var.
					if((is_two_input(cur->op) ||
						(is_three_input(cur->op) && strcmp(cur->in2, cur->in3) != 0)
					   ) &&
					   strcmp(cur->in2, cur->out) != 0 &&
					   never_used_again(cur->in2, cur)){
						free_name(cur->in2);
					}
				}

			}

			if(is_three_input(cur->op) && !is_predefined_input(cur->in3)){

				established_name = get_name_that_maps_to(cur->in3);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING? Only on last iteration
					char *premapped = cur->in3;
					cur->in3 = established_name->name;

					//Checking if we can free this var.
					if(strcmp(premapped, cur->out) != 0 &&
					   never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free

					//Checking if we can free this var.
					if(strcmp(cur->in3, cur->out) != 0 &&
					   never_used_again(cur->in3, cur)){
						free_name(cur->in3);
					}
				}
			}

			if(!is_predefined_output(cur->out)){
				established_name = get_name_that_maps_to(cur->out);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING? Only on last iteration
					char *premapped = cur->out;
					cur->out = established_name->name;

					//Checking if we can free this var.
					if(never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free

					//Checking if we can free this var.
					if(never_used_again(cur->out, cur)){
						free_name(cur->out);
					}
				}
			}

		}

		if(cur != prev)
			prev = prev->next;

		cur = cur->next;

	}


}
