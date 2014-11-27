

#include <stdio.h>
#include "reg_conserve.h"


reg_list headEntry = {NULL, 0, NULL};
reg_list *head = &headEntry;
//reg_list *last = head;

void add_name(char *name){
	if(name == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Assembly instruction: encountered a NULL register name.\n");
		return;
	}

	reg_list *cur = head;
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

	cur->next = head->next;
	head->next = cur;

}

void free_name(char *name){
	reg_list *cur = head;
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

	reg_list *cur = head;
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

	reg_list *cur = head;
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

	while(cur->next){
		cur = cur->next;

		if(strcmp(cur->in1, name) == 0)
			return 0;

		if(cur->kind == OPERATION){
			if(is_two_inputs(cur->op) && strcmp(cur->in2, name) == 0)
				return 0;

			if(is_three_inputs(cur->op) && strcmp(cur->in3, name) == 0)
				return 0;

			if(strcmp(cur->out, name) == 0)
				return 0;
		}

	}

	return 1;
}
//allowed predefined output
int is_predefined_output(char *output){
	if(strcmp(output, "gl_FragColor") == 0 ||
       strcmp(output, "gl_FragDepth") == 0  )
		return 1;
	return 0;
}


//allowed predefined input
int is_predefined_input(char *input){

	if(		strcmp(input, "gl_FragCoord") == 0 			||
			strcmp(input, "gl_TexCoord") == 0 			||
			strcmp(input, "gl_Color") == 0 				||
			strcmp(input, "gl_Secondary") == 0 			||
			strcmp(input, "gl_FogFragCoord") == 0 		||

			strcmp(input, "gl_Light_Half") == 0 			||
			strcmp(input, "gl_Light_Ambient") == 0 		||
			strcmp(input, "gl_Material_Shininess") == 0	||

			strcmp(input, "env1") == 0 					||
			strcmp(input, "env2") == 0 					||
			strcmp(input, "env3") == 0)
		return 1;

	return 0;
}


void conserve_reg(instr *cur){
	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Assembly register conservation function was called on an empty assembly instruction list.\n");
		return;
	}

	while(cur){
		if(cur->kind == DECLARATION){
			reg_list *free_name = get_free_name();

			if(free_name){
				free_name->replaced_name = cur->in1;
				free_name->is_free = 0;
			}
			else{
				add_name(cur->in1);
				if(never_used_again(cur->in1, cur)){
					free_name(cur->in1);
				}
			}
		}
		else{//OPERATION
			reg_list *established_name;

			if(!is_predefined_input(cur->in1)){

				established_name = get_name_that_maps_to(cur->in1);
				if(established_name){ //IS THE NAME UNIQUE, NEED TO BE FREE BEFORE CHANGING?

				}
			}

		}
	}


}
