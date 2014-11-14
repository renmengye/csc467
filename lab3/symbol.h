#ifndef _SYMBOL_H
#define _SYMBOL_H

typedef struct _symbol_table_entry {
	char *id;
	int is_const;
	int type_code;
	int vec;
	int is_init;
}symbol_table_entry;

void stack_push();
node * stack_pop();
node * stack_top();
int stack_is_empty();
int stack_size();

void scope_enter();
void scope_exit();

int symbol_exists_in_this_scope(char *id);
void symbol_add(symbol_table_entry entry);

symbol_table_entry* symbol_find(char *id);





#endif

