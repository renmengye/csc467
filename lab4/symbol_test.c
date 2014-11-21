#include <stdlib.h>
#include <stdio.h>

#include "symbol.h"

int main() {
	scope_is_in_main()==0 ? printf("Pass\n") : printf("Error\n");
	scope_enter();
	symbol_table_entry entry;
	scope_is_in_main()==1 ? printf("Pass\n") : printf("Error\n");
	entry.id = "haha";
	entry.is_const = 1;
	entry.type_code = 321;
	entry.vec = 2;
	entry.is_init = 0xf;
	symbol_add(entry);
	entry.id = "haha2";
	symbol_add(entry);
	scope_enter();
	scope_is_in_main()==0 ? printf("Pass\n") : printf("Error\n");
	entry.id = "haha3";
	symbol_add(entry);
	scope_enter();
	symbol_find("haha3")!=NULL ? printf("Pass\n") : printf("Error\n");
	symbol_exists_in_this_scope("haha3")==0 ? printf("Pass\n") : printf("Error\n");
	scope_exit();
	symbol_find("haha4")==NULL ? printf("Pass\n") : printf("Error\n");
	symbol_exists_in_this_scope("haha3")==1 ? printf("Pass\n") : printf("Error\n");
	scope_exit();
	symbol_find("haha3")==NULL ? printf("Pass\n") : printf("Error\n");
	symbol_find("haha2")!=NULL ? printf("Pass\n") : printf("Error\n");
	scope_exit();
	return 0;
}