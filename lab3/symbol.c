#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "symbol.h"

symbol_table_entry* stack = NULL;

void scope_enter() {
	symbol_table_entry *temp = 
		(symbol_table_entry *) malloc(sizeof(symbol_table_entry));
	temp->next = stack;
	temp->sib = NULL;
	temp->id = NULL;
	stack = temp;
}

void scope_exit() {
	symbol_table_entry *cur = stack;
	stack = stack->next;
	while(cur) {
		symbol_table_entry *next = cur->sib;
		free(cur);
		cur = next;
	}
}

void symbol_cpy(symbol_table_entry *src, symbol_table_entry *dst) {
	dst->id = src->id;
	dst->is_const = src->is_const;
	dst->type_code = src->type_code;
	dst->vec = src->vec;
	dst->is_init = src->is_init;
}

void symbol_add(symbol_table_entry entry) {
	symbol_table_entry *temp = 
		(symbol_table_entry *) malloc(sizeof(symbol_table_entry));
	if(!stack) {
		stack = 
			(symbol_table_entry *) malloc(sizeof(symbol_table_entry));
		stack->sib = NULL;
		stack->next = NULL;
		stack->id = NULL;
	}
	temp->next = stack->next;
	temp->sib = stack;
	symbol_cpy(&entry, temp);
	stack = temp;
}

int scope_is_in_main(){
	return stack ? stack->next == NULL : 0;
}

int symbol_exists_in_this_scope(char *id) {
	symbol_table_entry *cur = stack;
	while(cur) {
		if (cur->id) {
			if(strcmp(cur->id, id) == 0) {
				return 1;
			}
		}
		cur = cur->sib;
	}
	return 0;
}

symbol_table_entry* symbol_find(char *id){
	symbol_table_entry *cur = stack;
	while(cur) {
		symbol_table_entry *cur2 = cur;
		while(cur2) {
			if (cur2->id) {
				if(strcmp(cur2->id, id) == 0) {
					return cur2;
				}
			}
			cur2 = cur2->sib;
		}
		cur = cur->next;
	}
	return NULL;
}
