

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reg_conserve.h"

#define DEBUG_CONSERVE 0

#define DEBUG_TRACKER_1(__string, __var1) do {	\
			if(DEBUG_CONSERVE){ \
				int i;	\
				for(i=0;i<10;i++){	\
					char str[10];	\
					sprintf(str, "%d", i);	\
					printf("%d", is_predefined_input(str));	\
				}	\
					\
				printf(__string, __var1);	\
			}	}while(0)

#define DEBUG_TRACKER_0(__string) do {	\
			if(DEBUG_CONSERVE){ \
				int i;	\
				for(i=0;i<10;i++){	\
					char str[10];	\
					sprintf(str, "%d", i);	\
					printf("%d", is_predefined_input(str));	\
				}	\
					\
				printf(__string);	\
			}	}while(0)


int check_if_operation_references(char *op_name, char *name);

//allowed predefined output
int is_predefined_output(char *output){
	if(strcmp(output, "result.color") == 0 ||
       strcmp(output, "result.depth") == 0 ||
       strcmp(output, "result.color.x") == 0 ||
       strcmp(output, "result.depth.x") == 0 ||
       strcmp(output, "result.color.y") == 0 ||
       strcmp(output, "result.depth.y") == 0 ||
       strcmp(output, "result.color.z") == 0 ||
       strcmp(output, "result.depth.z") == 0 ||
       strcmp(output, "result.color.w") == 0 ||
       strcmp(output, "result.depth.w") == 0 )
		return 1;
	return 0;
}


//allowed predefined input
int is_predefined_input(char *input){

	if(		input[0] == '{' ||

			strcmp(input, "fragment.position") == 0 			||
			strcmp(input, "fragment.texcoord") == 0 			||
			strcmp(input, "fragment.color") == 0 				||
			strcmp(input, "fragment.color.secondary") == 0 			||
			strcmp(input, "fragment.fogcoord") == 0 		||

			strcmp(input, "state.light[0].half") == 0 			||
			strcmp(input, "state.lightmodel.ambient") == 0 		||
			strcmp(input, "state.material.shininess") == 0	||

			strcmp(input, "program.env[1]") == 0 					||
			strcmp(input, "program.env[2]") == 0 					||
			strcmp(input, "program.env[3]") == 0 ||

			strcmp(input, "fragment.position.x") == 0 			||
			strcmp(input, "fragment.texcoord.x") == 0 			||
			strcmp(input, "fragment.color.x") == 0 				||
			strcmp(input, "fragment.color.secondary.x") == 0 			||
			strcmp(input, "fragment.fogcoord.x") == 0 		||

			strcmp(input, "state.light[0].half.x") == 0 			||
			strcmp(input, "state.lightmodel.ambient.x") == 0 		||
			strcmp(input, "state.material.shininess.x") == 0	||

			strcmp(input, "program.env[1].x") == 0 					||
			strcmp(input, "program.env[2].x") == 0 					||
			strcmp(input, "program.env[3].x") == 0	||

			strcmp(input, "fragment.position.y") == 0 			||
			strcmp(input, "fragment.texcoord.y") == 0 			||
			strcmp(input, "fragment.color.y") == 0 				||
			strcmp(input, "fragment.color.secondary.y") == 0 			||
			strcmp(input, "fragment.fogcoord.y") == 0 		||

			strcmp(input, "state.light[0].half.y") == 0 			||
			strcmp(input, "state.lightmodel.ambient.y") == 0 		||
			strcmp(input, "state.material.shininess.y") == 0	||

			strcmp(input, "program.env[1].y") == 0 					||
			strcmp(input, "program.env[2].y") == 0 					||
			strcmp(input, "program.env[3].y") == 0 ||

			strcmp(input, "fragment.position.z") == 0 			||
			strcmp(input, "fragment.texcoord.z") == 0 			||
			strcmp(input, "fragment.color.z") == 0 				||
			strcmp(input, "fragment.color.secondary.z") == 0 			||
			strcmp(input, "fragment.fogcoord.z") == 0 		||

			strcmp(input, "state.light[0].half.z") == 0 			||
			strcmp(input, "state.lightmodel.ambient.z") == 0 		||
			strcmp(input, "state.material.shininess.z") == 0	||

			strcmp(input, "program.env[1].z") == 0 					||
			strcmp(input, "program.env[2].z") == 0 					||
			strcmp(input, "program.env[3].z") == 0 ||

			strcmp(input, "fragment.position.w") == 0 			||
			strcmp(input, "fragment.texcoord.w") == 0 			||
			strcmp(input, "fragment.color.w") == 0 				||
			strcmp(input, "fragment.color.secondary.w") == 0 			||
			strcmp(input, "fragment.fogcoord.w") == 0 		||

			strcmp(input, "state.light[0].half.w") == 0 			||
			strcmp(input, "state.lightmodel.ambient.w") == 0 		||
			strcmp(input, "state.material.shininess.w") == 0	||

			strcmp(input, "program.env[1].w") == 0 					||
			strcmp(input, "program.env[2].w") == 0 					||
			strcmp(input, "program.env[3].w") == 0)
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
		if(check_if_operation_references(cur->name, name)){
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

	DEBUG_TRACKER_0("\n Adding name fin\n");



}

void free_name(char *name){
	reg_list *cur = reghead;
	while(cur->next){
		cur = cur->next;
		if(check_if_operation_references(cur->name, name)){
			DEBUG_TRACKER_1("\n Freeing name: %s\n", name);
			cur->is_free = 1;
			if(cur->replaced_name){
				free(cur->replaced_name); //The mapped variable no longer appears anywhere and the string isnt needed.
			}
			cur->replaced_name = NULL;
			return;
		}
	}

	errorOccurred = 1;
	fprintf(errorFile,"Tried to free a name not in the active list. %s\n", name);
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

		if(cur->replaced_name && check_if_operation_references(cur->replaced_name, name)){
			return cur;
		}
	}

	return NULL;
}

int check_if_operation_references(char *op_name, char *name){
	if(strcmp(op_name, name) == 0)
		return 1;

	if( strrchr(op_name, '.') && strncmp(op_name, name ,(int) (strrchr(op_name, '.') - op_name)) == 0 )
		return 1;

	if( strrchr(name, '.') && strncmp(op_name, name ,(int) (strrchr(name, '.') - name)) == 0 )
		return 1;


	return 0;
}

//call after checking that it isnt used in the same instruction.
//call on your current isntruction
int never_used_again(char *name, instr *cur){

	DEBUG_TRACKER_0("\n Entering never used again check\n");


	while(cur->next){

		cur = cur->next;



		if(cur->kind == OPERATION){
			DEBUG_TRACKER_1("\n Check if ref: %s\n", cur->in1);
			if(check_if_operation_references(cur->in1, name))
				return 0;

			if((is_two_input(cur->op) || is_three_input(cur->op)) && check_if_operation_references(cur->in2, name))
				return 0;

			if(is_three_input(cur->op) && check_if_operation_references(cur->in3, name))
				return 0;


		}
		DEBUG_TRACKER_1("\n %s\n", cur->out);

		if(check_if_operation_references(cur->out, name))
						return 0;

	}


	return 1;
}



void conserve_reg(instr *cur){

	DEBUG_TRACKER_0("\n Entered conserve\n");

	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Assembly register conservation function was called on an empty assembly instruction list.\n");
		return;
	}

	instr *prev = cur;

	while(cur){

		if(cur->kind == DECLARATION){

			DEBUG_TRACKER_0("\n Declaration\n");

			reg_list *free_reg = get_free_name();

			if(free_reg){
				DEBUG_TRACKER_1("\n Found free: %s\n", free_reg->name);

				free_reg->replaced_name = cur->out;
				free_reg->is_free = 0;

				//Destroying the instruction. NEEED TO FREE NAMES????? No.
				prev->next = cur->next;
				free(cur);
			}
			else{
				DEBUG_TRACKER_1("\n New active: %s\n", cur->out);

				add_name(cur->out);
				if(never_used_again(cur->out, cur)){
					free_name(cur->out);
				}
			}
		}
		else{//OPERATION



			reg_list *established_name;

			if((cur->op != MOV || (cur->op == MOV && cur->in1[0] != '{')) &&
			   !is_predefined_input(cur->in1)){

				DEBUG_TRACKER_0("\n Operation first var\n");

				established_name = get_name_that_maps_to(cur->in1);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING?
					char *premapped = cur->in1;
					DEBUG_TRACKER_0("\n Operation first var, established\n");

					if(strrchr(cur->in1, '.')){
						char index = strrchr(cur->in1, '.')[1]; //first char after .
						snprintf(cur->in1, 100, "%s.%c", established_name->name, index);
					}
					else{
						cur->in1 = established_name->name;
					}

					//Checking if we can free this var.
					if((is_one_input(cur->op) ||
						(is_two_input(cur->op) && !check_if_operation_references(premapped, cur->in2)) ||
						(is_three_input(cur->op) && !check_if_operation_references(premapped, cur->in2) && !check_if_operation_references(premapped, cur->in3))
					   ) &&
							!check_if_operation_references(premapped, cur->out) &&
					   never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free
					DEBUG_TRACKER_0("\n Operation first var, not est\n");
					//Checking if we can free this var.
					if((is_one_input(cur->op) ||
						(is_two_input(cur->op) && !check_if_operation_references(cur->in1, cur->in2)) ||
						(is_three_input(cur->op) && !check_if_operation_references(cur->in1, cur->in2) && !check_if_operation_references(cur->in1, cur->in3))
					   ) &&
					   !check_if_operation_references(cur->in1, cur->out) &&
					   never_used_again(cur->in1, cur)){
						free_name(cur->in1);
					}
				}

			}

			DEBUG_TRACKER_0("\n Operation past first if\n");

			if((is_two_input(cur->op) || is_three_input(cur->op)) &&
			    !is_predefined_input(cur->in2)){

				established_name = get_name_that_maps_to(cur->in2);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING? Only on last iteration
					char *premapped = cur->in2;

					if(strrchr(cur->in2, '.')){
						char index = strrchr(cur->in2, '.')[1]; //first char after .
						snprintf(cur->in2, 100, "%s.%c", established_name->name, index);
					}
					else{
						cur->in2 = established_name->name;
					}

					//Checking if we can free this var.
					if((is_two_input(cur->op) ||
						(is_three_input(cur->op) && !check_if_operation_references(premapped, cur->in3))
					   ) &&
					   !check_if_operation_references(premapped, cur->out) &&
					   never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free

					//Checking if we can free this var.
					if((is_two_input(cur->op) ||
						(is_three_input(cur->op) && !check_if_operation_references(cur->in2, cur->in3))
					   ) &&
					   !check_if_operation_references(cur->in2, cur->out) &&
					   never_used_again(cur->in2, cur)){
						free_name(cur->in2);
					}
				}

			}

			if(is_three_input(cur->op) && !is_predefined_input(cur->in3)){

				established_name = get_name_that_maps_to(cur->in3);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING? Only on last iteration
					char *premapped = cur->in3;

					if(strrchr(cur->in3, '.')){
						char index = strrchr(cur->in3, '.')[1]; //first char after .
						snprintf(cur->in3, 100, "%s.%c", established_name->name, index);
					}
					else{
						cur->in3 = established_name->name;
					}

					//Checking if we can free this var.
					if(!check_if_operation_references(premapped, cur->out) &&
					   never_used_again(premapped, cur)){
						free_name_struct(established_name);
					}
				}
				else{//in this case, in1 must already be in the active register list, and is not free

					//Checking if we can free this var.
					if(!check_if_operation_references(cur->in3, cur->out) &&
					   never_used_again(cur->in3, cur)){
						free_name(cur->in3);
					}
				}
			}

			if(!is_predefined_output(cur->out)){

				DEBUG_TRACKER_0("\n Operation, check output\n");

				established_name = get_name_that_maps_to(cur->out);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING? Only on last iteration
					char *premapped = cur->out;

					if(strrchr(cur->out, '.')){
						char index = strrchr(cur->out, '.')[1]; //first char after .
						snprintf(cur->out, 100, "%s.%c", established_name->name, index);
					}
					else{
						cur->out = established_name->name;
					}

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
		DEBUG_TRACKER_0("\n loop iteration\n");

		if(cur != prev)
			prev = prev->next;

		cur = cur->next;

	}


}
