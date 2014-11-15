#ifndef _SYMBOL_H
#define _SYMBOL_H



typedef struct _symbol_table_entry {
	char *id;
	int is_const;
	int type_code;
	int vec;
	int is_init; //VERY IMPORTANT, the implementation for this is not straight forward
}symbol_table_entry;

//Depending on the size of the vector, say 4, the is_init can be in binary:
// 0000 for fully unitialized, 1000, means the 4th, ie id[3] has been initialized
// 0001 makes the id[0] initialized.

//Doesnt change anything else, since you end up returning a pointer to data entry anyway,
//so I can edit the contetns to my hearts content

void stack_push();
void * stack_pop();
void * stack_top();
int stack_is_empty();
int stack_size();

void scope_enter();
void scope_exit();

//MORE STUFF. I NEED ONE MORE FUNCTION
//One type of predefined variables: result, can only be assigned in the main block and
//note in any if or else block.
int scope_are_in_main();

int symbol_exists_in_this_scope(char *id);
void symbol_add(symbol_table_entry entry);

symbol_table_entry* symbol_find(char *id);





#endif

