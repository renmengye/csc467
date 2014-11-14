#ifndef _SYMBOL_H
#define _SYMBOL_H

struct stack_elem;

void stack_push(node *);
node * stack_pop();
node * stack_top();
int stack_is_empty();
int stack_size();

void symbol_add(char *id, int is_const ,int type_code, int vec, int initialised);
int symbol_exists(char *id);
int symbol_exists_in_this_scope(char *);
int symbol_is_initialised(char *id);



#endif

