#ifndef _SYMBOL_H
#define _SYMBOL_H

typedef struct _symbol_table_entry {
	char *id;
	int is_const;
	int type_code;
	int vec;
	int is_init; //VERY IMPORTANT, the implementation for this is not straight forward
	struct _symbol_table_entry *next;
	struct _symbol_table_entry *sib;
} symbol_table_entry;

//Depending on the size of the vector, say 4, the is_init can be in binary:
// 0000 for fully unitialized, 1000, means the 4th, ie id[3] has been initialized
// 0001 makes the id[0] initialized.

//Doesnt change anything else, since you end up returning a pointer to data entry anyway,
//so I can edit the contetns to my hearts content

void scope_enter();
void scope_exit();
int scope_is_in_main();
int symbol_exists_in_this_scope(char *id);
void symbol_add(symbol_table_entry entry);
symbol_table_entry* symbol_find(char *id);

#endif

