

#include <stdio.h>
#include "reg_conserve.h"
#include "arb.h"
#include "ast.h"

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

	cur->next = head->next;
	head->next = cur;

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

//call after checking that it isnt used in the same instruction.
//call on your current isntruction
int never_used_again(char *name, instr *cur){

	while(cur)


	return 1;
}


void conserve_reg(instr *cur){
	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Assembly register conservation function was called on an empty assembly instruction list.\n");
		return;
	}

	while(cur){

	}


}
