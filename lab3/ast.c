#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#include "symbol.h"

#define DEBUG_SEMANTIC 0

#define DEBUG_PRINT_TREE 0

const char *get_type_str(struct type_s *type);
const char *get_op_str(int op);
const char *get_func_str(int op);
// void ast_print_node_post(node *cur, int level);
// void ast_print_node(node *cur, int level);

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof *ast); //mmk
  ast->kind = kind;

  va_start(args, kind);

  switch(kind) {

  case SCOPE_NODE:
	  ast->scope.declarations = va_arg(args, node *); //Could be NULL.
	  ast->scope.stmts = va_arg(args, node *); //Could be NULL.
	  break;

  case DECLARATIONS_NODE:
	  ast->declarations.declarations = va_arg(args, node *); //Could be NULL.
	  ast->declarations.declaration = va_arg(args, node *);
	  break;

  case STATEMENTS_NODE:
	  ast->stmts.stmts = va_arg(args, node *); //Could be NULL.
	  ast->stmts.stmt = va_arg(args, node *); //Could be NULL.
	  break;

  case DECLARATION_NODE: //Note, create symbol table will be done after whole tree is initialised
	  ast->declaration.is_const = va_arg(args, int);
	  ast->declaration.id = va_arg(args, char *);
	  ast->declaration.type_node = va_arg(args, node *);
	  ast->declaration.expr = va_arg(args, node *); //Could be NULL.
	  break;

  //Statement grammar
  case ASSIGNMENT_NODE:
	  ast->assignment.variable = va_arg(args, node *);
	  ast->assignment.expr = va_arg(args, node *);
	  break;

  case IF_STATEMENT_NODE:
	  ast->if_stmt.condition_expr = va_arg(args, node *);
	  ast->if_stmt.if_blk_stmt = va_arg(args, node *); //Could be NULL.
	  ast->if_stmt.else_blk_stmt = va_arg(args, node *); //Could be NULL.
	  break;

  case NESTED_SCOPE_NODE:
	  ast->nested_scope = va_arg(args, node *);
	  break;

  case TYPE_NODE:

	  ast->type.type_code = va_arg(args, int);
	  ast->type.vec = va_arg(args, int) + 1;
	  break;

  //Expression grammar
  case CONSTRUCTOR_NODE:
	  ast->ctor.type_node = va_arg(args, node *);
	  ast->ctor.args = va_arg(args, node *); //Could be NULL.
	  break;

  case FUNCTION_NODE:
	  ast->func.name = va_arg(args, int);
	  ast->func.args = va_arg(args, node *); //Could be NULL.
	  break;

  case UNARY_EXPRESION_NODE:
	  ast->unary_expr.op = va_arg(args, int);
	  ast->unary_expr.right = va_arg(args, node *);
	  break;

  case BINARY_EXPRESSION_NODE:
	  ast->binary_expr.op = va_arg(args, int);
	  ast->binary_expr.left = va_arg(args, node *);
	  ast->binary_expr.right = va_arg(args, node *);
	  break;

  case BOOL_NODE:
	  ast->bool_val = va_arg(args, int);
	  break;

  case INT_NODE:
	  ast->int_val = va_arg(args, int);
	  break;

  case FLOAT_NODE:
  	  ast->int_val = va_arg(args, int);
  	  break;

  case NESTED_EXPRESSION_NODE:
	  ast->nested_expr.expr = va_arg(args, node *);
	  break;

  case EXP_VAR_NODE:
	  ast->exp_var_node.var_node= va_arg(args, node *);
	  break;

  case VAR_NODE:
	  ast->var_node.id = va_arg(args, char *);
	  ast->var_node.is_array = va_arg(args, int);
	  ast->var_node.index = va_arg(args, int);
	  break;

  case ARGUMENTS_NODE:
	  ast->args.args = va_arg(args, node *); //Could be NULL
	  ast->args.expr = va_arg(args, node *); //Could be NULL
	  break;

  default: break; //Error?
  }

  va_end(args);

  return ast;
}

void ast_free_post(node *ast, int level) {
	if(ast->kind == DECLARATION_NODE){
		free(ast->declaration.id);
	}
	free(ast);
}

void ast_free(node *ast) {
  ast_traverse(ast, 0, NULL, &ast_free_post);
}

void indent(int level, int is_open, int new_line) {
  if (new_line) {
    fprintf(dumpFile, "\n");
    int lv_i = 0;
    for (; lv_i < level; ++lv_i) {
      fprintf(dumpFile, "    ");
    }
  }
  fprintf(dumpFile, is_open ? "(" : ")");
}

void ast_print_node_post(node *cur, int level) {
  switch(cur->kind) {
     case SCOPE_NODE:
        indent(level, 0, 1);
        break;
     case UNARY_EXPRESION_NODE:
        indent(level, 0, 0);
        break;
     case BINARY_EXPRESSION_NODE:
        indent(level, 0, 0);
        break;
     case INT_NODE:
        /* Do nothing */
        break;
     case FLOAT_NODE:
        /* Do nothing */
        break;
     case IDENT_NODE:
        /* Do nothing */
        break;
     case TYPE_NODE:
        /* Do nothing */
        break;
     case BOOL_NODE:
        /* Do nothing */
        break;
     case VAR_NODE:
        /* Do nothing */
        break;
     case FUNCTION_NODE:
        indent(level, 0, 0);
        break;
     case CONSTRUCTOR_NODE:
        indent(level, 0, 0);
        break;
     case ARGUMENTS_NODE:
        /* Do nothing */
        break;
     case STATEMENTS_NODE:
        indent(level, 0, 0);
       break;
     case IF_STATEMENT_NODE:
        indent(level, 0, 0);
        break;
     case ASSIGNMENT_NODE:
        indent(level, 0, 0);
        break;
     case NESTED_SCOPE_NODE:
        /* Do nothing */
        break;

     case DECLARATION_NODE:
        indent(level, 0, 0);
     case DECLARATIONS_NODE:
        indent(level, 0, 0);
        break;
     default:
        /* Do nothing */
        break;
  }
}

void ast_print_node(node *cur, int level) {
  switch(cur->kind) {
     case SCOPE_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "SCOPE");
        break;
     case UNARY_EXPRESION_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "UNARY %s %s", get_type_str(&cur->type), get_op_str(cur->unary_expr.op));
        break;
     case BINARY_EXPRESSION_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "BINARY %s %s", get_type_str(&cur->type), get_op_str(cur->binary_expr.op));
        break;
     case INT_NODE:
        fprintf(dumpFile, " ");
        fprintf(dumpFile, "%d", cur->int_val);
        break;
     case FLOAT_NODE:
        fprintf(dumpFile, " ");
        fprintf(dumpFile, "%f", cur->float_val);
        break;
     case IDENT_NODE:
        /* Do nothing */
        break;
     case TYPE_NODE:
        fprintf(dumpFile, " ");
        fprintf(dumpFile, "%s", get_type_str(&cur->type));
        break;
     case BOOL_NODE:
        fprintf(dumpFile, " ");
        if (cur->bool_val) {
          fprintf(dumpFile, "true");
        } else {
          fprintf(dumpFile, "false");
        }
        break;
     case VAR_NODE:
        fprintf(dumpFile, " ");
        if (cur->var_node.is_array) {
          fprintf(dumpFile,
            "INDEX %s %s %d",
            get_type_str(&cur->type),
            cur->var_node.id,
            cur->var_node.index);
        } else {
          fprintf(dumpFile, "%s", cur->var_node.id);
        }
        break;
     case EXP_VAR_NODE:
        /* Do nothing */
        break;
     case FUNCTION_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "CALL %s", get_func_str(cur->func.name));
        break;
     case CONSTRUCTOR_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "CALL");
        break;
     case ARGUMENTS_NODE:
        /* Do nothing */
        break;
     case STATEMENTS_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "STATEMENTS");
        break;
     case IF_STATEMENT_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "IF");
        break;
     case ASSIGNMENT_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "ASSIGNMENT %s", get_type_str(&cur->assignment.type));
        break;
     case NESTED_SCOPE_NODE:
        /* Do nothing */
        break;

     case DECLARATION_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "DECLARATION %s", cur->declaration.id); //get_type_str(&cur->declaration.type_node->type));
        break;
     case DECLARATIONS_NODE:
        indent(level, 1, 1);
        fprintf(dumpFile, "DECLARATIONS");
        break;
     default:
        /* Do nothing */
        break;
  }
}

void ast_print(node * ast) {
  ast_traverse(ast, 0, &ast_print_node, &ast_print_node_post);
  fprintf(dumpFile, "\n");
}

const char *get_type_str(struct type_s *type) {
  switch(type->type_code) {
    case FLOAT_T:
      return "FLOAT";
    case INT_T:
      return "INT";
    case BOOL_T:
      return "BOOL";
    case BVEC_T:
      switch(type->vec){
        case 2:
          return "BVEC2";
        case 3:
          return "BVEC3";
        case 4:
          return "BVEC4";
      }
    case IVEC_T:
      switch(type->vec){
        case 2:
          return "IVEC2";
        case 3:
          return "IVEC3";
        case 4:
          return "IVEC4";
      }
    case VEC_T:
      switch(type->vec){
        case 2:
          return "VEC2";
        case 3:
          return "VEC3";
        case 4:
          return "VEC4";
      }
    default:
      return "ANY";
  }
}

const char *get_op_str(int op) {
  switch(op) {
    case '-':
      return "-";
    case '!':
      return "!";
    case AND:
      return "&&";
    case OR:
      return "||";
    case EQ:
      return "==";
    case NEQ:
      return "!=";
    case '<':
      return "<";
    case LEQ:
      return "<=";
    case '>':
      return ">";
    case GEQ:
      return ">=";
    case '+':
      return "+";
    case '*':
      return "*";
    case '/':
      return "/";
    case '^':
      return "^";
    default:
      return "";
  }
}

const char *get_func_str(int name) {
  switch(name) {
    case 0:
      return "DP3";
    case 1:
      return "RSQ";
    case 2:
      return "LIT";
    default:
      return "";
  }
}

void ast_traverse(node * cur,
                  int level,
                  TR_FUNC pre_func,
                  TR_FUNC post_func) {

	if(DEBUG_SEMANTIC){
			printf("ABOUT TO TRANSVERSE kind: %s\n", node_name(cur->kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		}

  if (pre_func) pre_func(cur, level);
  level++;
  switch(cur->kind) {
    case SCOPE_NODE:
      if(cur->scope.declarations)
        ast_traverse(cur->scope.declarations, level, pre_func, post_func);
      if(cur->scope.stmts)
        ast_traverse(cur->scope.stmts, level, pre_func, post_func);
      break;
    case UNARY_EXPRESION_NODE:
      ast_traverse(cur->unary_expr.right, level, pre_func, post_func);
      break;
    case BINARY_EXPRESSION_NODE:
      ast_traverse(cur->binary_expr.left, level, pre_func, post_func);
      ast_traverse(cur->binary_expr.right, level, pre_func, post_func);
      break;
    case INT_NODE:
      /* Do nothing */
      break;
    case FLOAT_NODE:
      /* Do nothing */
      break;
    case IDENT_NODE:
      /* Do nothing */
      break;
    case TYPE_NODE:
      /* Do nothing */
      break;
    case BOOL_NODE:
      /* Do nothing */
      break;
    case VAR_NODE:
      /* Do nothing */
    	break;
    case EXP_VAR_NODE:
      ast_traverse(cur->exp_var_node.var_node, level, pre_func, post_func);
      break;
    case FUNCTION_NODE:
      if (cur->func.args)
        ast_traverse(cur->func.args, level, pre_func, post_func);
      break;
    case CONSTRUCTOR_NODE:
      ast_traverse(cur->ctor.type_node, level, pre_func, post_func);
      ast_traverse(cur->ctor.args, level, pre_func, post_func);
      break;
    case ARGUMENTS_NODE:
      if (cur->args.args)
        ast_traverse(cur->args.args, level, pre_func, post_func);
      break;

    case STATEMENTS_NODE:
      if (cur->stmts.stmts)
        ast_traverse(cur->stmts.stmts, level, pre_func, post_func);
      ast_traverse(cur->stmts.stmt, level, pre_func, post_func);
      break;
    case IF_STATEMENT_NODE:
      ast_traverse(cur->if_stmt.condition_expr, level, pre_func, post_func);
      ast_traverse(cur->if_stmt.if_blk_stmt, level, pre_func, post_func);
      if (cur->if_stmt.else_blk_stmt)
        ast_traverse(cur->if_stmt.else_blk_stmt, level, pre_func, post_func);
      break;
    case ASSIGNMENT_NODE:
      ast_traverse(cur->assignment.variable, level, pre_func, post_func);
      ast_traverse(cur->assignment.expr, level, pre_func, post_func);
      break;
    case NESTED_SCOPE_NODE:
      ast_traverse(cur->nested_scope, level, pre_func, post_func);
      break;

    case DECLARATION_NODE:
      ast_traverse(cur->declaration.type_node, level, pre_func, post_func);
      if (cur->declaration.expr)
        ast_traverse(cur->declaration.expr, level, pre_func, post_func);
      break;
    case DECLARATIONS_NODE:
      if (cur->declarations.declarations)
        ast_traverse(cur->declarations.declarations, level, pre_func, post_func);
      ast_traverse(cur->declarations.declaration, level, pre_func, post_func);
      break;
    default:
      /* Do nothing */
      break;
  }
  level--;
  if(DEBUG_SEMANTIC){
  			printf("TRANSVERSED kind: %s\n", node_name(cur->kind));
  			int i = 0;
  			for(i = 0; i < 20; i++){
  				printf("-");
  			}
  			printf("\n");

  		}
  if (post_func) post_func(cur, level);
}

int type_of_vector_element(int vec_type){
	if(vec_type == BOOL_T ||
	   vec_type == FLOAT_T ||
	   vec_type == INT_T)
		return vec_type;

	if(vec_type == BVEC_T)
		return BOOL_T;
	else if(vec_type == VEC_T)
		return FLOAT_T;
	else
		return INT_T;
}



void ast_check_semantics(){
	if(DEBUG_SEMANTIC){
		printf("Semantic check starting\n");
		int i = 0;
		for(i = 0; i < 20; i++){
			printf("-");
		}
		printf("\n");
	}

	if(ast == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Main scope not found.\n");
		return;
	}
	else{
		ast_traverse(ast, 0, &ast_scope_generator, &ast_sementic_check);

	}

}

void ast_sementic_check(node* cur, int x){ //Done bottom-up.
	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Semantic function visited a NULL node, should not have happened\n");
		return;
	}

	node_kind kind = cur->kind;

	if(DEBUG_SEMANTIC){
		printf("kind: %s\n", node_name(kind));
		int i = 0;
		for(i = 0; i < 20; i++){
			printf("-");
		}
		printf("\n");

	}
	switch(kind) {

	  case SCOPE_NODE: //Everything in this scope has been dealt with, need to exit the scope.
		  scope_exit();
		  break;

	  case DECLARATIONS_NODE:
		  break;

	  case STATEMENTS_NODE:
		  break;

		  /* In the case of a declaration being initialised, we need to know the type of
		   * that expression.  */
	  case DECLARATION_NODE:

		  if(symbol_exists_in_this_scope(cur->declaration.id)){
			  fprintf(errorFile,"Variable with ID: %s, already exists in this scope.\n", cur->declaration.id);
			  break;
		  }

		  //Checking that we'arnt trying to declare a variable from the predifined list
		  if(strcmp(cur->declaration.id, "gl_FragColor") == 0 			||
		     strcmp(cur->declaration.id, "gl_FragDepth") == 0 			||
		     strcmp(cur->declaration.id, "gl_FragCoord") == 0 			||

		     strcmp(cur->declaration.id, "gl_TextCoord") == 0 			||
		     strcmp(cur->declaration.id, "gl_Color") == 0 				||
		     strcmp(cur->declaration.id, "gl_Secondary") == 0 			||
		     strcmp(cur->declaration.id, "gl_FogFragCoord") == 0 		||

		     strcmp(cur->declaration.id, "gl_Light_Half") == 0 			||
		     strcmp(cur->declaration.id, "gl_Light_Ambient") == 0 		||
		     strcmp(cur->declaration.id, "gl_Material_Shininess") == 0	||

		     strcmp(cur->declaration.id, "env1") == 0 					||
		     strcmp(cur->declaration.id, "env2") == 0 					||
		     strcmp(cur->declaration.id, "env3") == 0){
			  fprintf(errorFile,"Tried to declare a predefined variable\n");
			  break;
		  }

		  symbol_table_entry new_entry;

		  if(DEBUG_SEMANTIC){
			printf("Declaration %d, %d\n", cur->declaration.type_node->type.type_code, cur->declaration.type_node->type.vec);
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }
		  /* When a declared variable is also assigned a value. */
		  if(cur->declaration.expr){
			  //type mismatch/ only if not any on the right.
			  //Any type should have no error
			  if(cur->declaration.expr->type.type_code != -1 &&
				 !(cur->declaration.type_node->type.type_code == cur->declaration.expr->type.type_code &&
				   cur->declaration.type_node->type.vec == cur->declaration.expr->type.vec)){
				  fprintf(errorFile,"Declaration of %s, expecting type: %s, getting type: %s\n",
						  cur->declaration.id,
						  get_type_str(&(cur->declaration.type_node->type)),
						  get_type_str(&(cur->declaration.expr->type)));

			  }

			  //If const variable, it can only be assigned a literal or a uniform variable
			  if(cur->declaration.is_const){
				  if(!cur->declaration.expr->type.is_const){
					  fprintf(errorFile,"const variables can only be initialised with either a literal or a uniform predefined variable.\n");

				  }
			  }

			  new_entry.is_init = 1;
		  }
		  /* Pure declaration, uninitialised */
		  else{
			  new_entry.is_init = 0;
		  }

		  new_entry.id = cur->declaration.id;
		  new_entry.is_const = cur->declaration.is_const;
		  new_entry.type_code = cur->declaration.type_node->type.type_code;
		  new_entry.vec = cur->declaration.type_node->type.vec;

		  symbol_add(new_entry);

		  break;

		  //Statement grammar
	  case ASSIGNMENT_NODE:{

		  symbol_table_entry *varEntry = symbol_find(cur->assignment.variable->var_node.id);

		  //Actual variables
		  if(varEntry){

			  //If individual array access assignment
			  if(cur->assignment.variable->var_node.is_array){
				  //checking index that is ok is done in variable node

				  //Check for changing a const already initialised
				  if(varEntry->is_const &&
					 (varEntry->is_init & (1 << (cur->assignment.variable->var_node.index - 1))) ){
					  fprintf(errorFile,"Attempting to change a const variable.\n");
					  break;
				  }
			  }
			  else{
				  //Check for changing a const already initialised
				  if(varEntry->is_const && (varEntry->is_init == 0b1111 >> (4 - varEntry->vec)) ){
					  fprintf(errorFile,"Attempting to change a const variable.\n");
					  break;
				  }

			  }

			  //Type check
			  if(cur->assignment.expr->type.type_code != -1 &&
				 !(cur->assignment.variable->type.type_code == cur->assignment.expr->type.type_code &&
				   cur->assignment.variable->type.vec == cur->assignment.expr->type.vec)){
				  fprintf(errorFile,"Assignment of %s, expecting type: %s, getting type: %s\n",
						  cur->assignment.variable->var_node.id,
						  get_type_str(&(cur->assignment.variable->type)),
						  get_type_str(&(cur->assignment.expr->type)));
				  break;
			  }

			  //Check if const, can only be assigned literals or a uniform value
			  if(varEntry->is_const){
				  if(!cur->assignment.expr->type.is_const){
					  fprintf(errorFile,"const variables can only be initialised with either a literal or a uniform predefined variable.\n");
					  break;
				  }
			  }


			  //Checks complete, valid assignment

			  //Setting as initialised.
			  //If array access assignment, need to set correct bit
			  if(cur->assignment.variable->var_node.is_array){
				  varEntry->is_init = varEntry->is_init | (1 << (cur->assignment.variable->var_node.index - 1));

			  }
			  else{//else, assign appropriate value to init
				  //Check for changing a const already initialised
				  varEntry->is_init = 0b1111 >> (4 - varEntry->vec);

			  }

		  }
		  //Var ID not found in symbol table.
		  else{
			  //Predefined cases
			  if(strcmp(cur->assignment.variable->var_node.id, "gl_FragColor") == 0 ||
				 strcmp(cur->assignment.variable->var_node.id, "gl_FragDepth") == 0 ||
				 strcmp(cur->assignment.variable->var_node.id, "gl_FragCoord") == 0   ){
				  //These can only be modified in the main scope
				  if(!scope_is_in_main()){
					  fprintf(errorFile,"Predefined variables of type result can only be changed in the main scope\n");
					  break;
				  }

				  //Type check
				  if(cur->assignment.expr->type.type_code != -1 &&
					 !(cur->assignment.variable->type.type_code == cur->assignment.expr->type.type_code &&
					   cur->assignment.variable->type.vec == cur->assignment.expr->type.vec)){
					  fprintf(errorFile,"Assignment of %s, expecting type: %s, getting type: %s\n",
							  cur->assignment.variable->var_node.id,
							  get_type_str(&(cur->assignment.variable->type)),
							  get_type_str(&(cur->assignment.expr->type)));
					  break;
				  }

				  //Checks Complete, valid assignment
				  //Don't need to do anything
			  }
			  else{
				  fprintf(errorFile,"Undeclared variable %s\n, or trying to modify predefined vars incorrectly.\n", cur->assignment.variable->var_node.id);
				  break;
			  }
		  }
		  break;
	  }

	  case IF_STATEMENT_NODE:
		  //Need to make sure the expression is of boolean type or any
		  if(cur->if_stmt.condition_expr->type.type_code != -1 &&
			 !(cur->if_stmt.condition_expr->type.type_code == BOOL_T &&
			   cur->if_stmt.condition_expr->type.vec == 1)){
			  fprintf(errorFile,"if condition, expecting type: BOOL_T, getting type: %s\n",
					  get_type_str(&(cur->if_stmt.condition_expr->type)));
			  break;
		  }

		  break;

	  case NESTED_SCOPE_NODE:
		  break;
		  //End of Statement grammar

	  case TYPE_NODE: //Leaf. Do nothing.
		  break;

		  //Expression grammar
	  case CONSTRUCTOR_NODE:{

		  //Inherit type
		  cur->type.is_const = cur->ctor.type_node->type.is_const;
		  cur->type.type_code = cur->ctor.type_node->type.type_code;
		  cur->type.vec = cur->ctor.type_node->type.vec;

		  int cur_vec = cur->type.vec;
		  node *ptr = cur->ctor.args;

          //Void input not allowed for constructor
          if(ptr == NULL){
        	  fprintf(errorFile,"Constructor is not allowed a void input\n");
              break;
          }

          do{
        	  ptr = ptr->args.args; //node with first valid expression

        	  if(ptr->type.is_const == 0){
        		  cur->type.is_const = 0;
        	  }

        	  //type check
        	  if(ptr->type.type_code != -1 &&
				 !(type_of_vector_element(cur->type.type_code) == ptr->type.type_code &&
				   1 == ptr->type.vec)){
        		  struct type_s t;
        		  t.type_code = cur->type.type_code;
        		  t.vec = 1;
        		  fprintf(errorFile,"Constructor cell at index %d: expecting type: %s, getting type: %s\n",
        				  cur->type.vec - cur_vec,
        				  get_type_str(&(t)),
        				  get_type_str(&(ptr->type)));
        	  }

        	  cur_vec--;

          } while(cur_vec > 0 && ptr->args.args);

          //Not enough arguments in the constructor.
          if(cur_vec > 0){
    		  fprintf(errorFile,"Constructor expecting %d arguments, but received only %d.\n",
    				  cur->type.vec, cur->type.vec - cur_vec);

          }

          //Too many arguments
          if(ptr->args.args){
    		  fprintf(errorFile,"Constructor expecting %d arguments, but received more.\n",
    				  cur->type.vec);
          }

          //Inheritance happened, so we are all good.

		  break;
      }
	  case FUNCTION_NODE:{

		  //DP3 = 0, FLOAT_T if VEC4 or VEC3 arguments, INT_T if IV (VEC4, VEC4) or (VEC3, VEC3)
		  //LIT = 1, VEC4, (VEC4)
		  //RSQ = 2  FLOAT, (float) or (int)


		  cur->type.is_const = 0;

		  int func = cur->func.name;

		  if(func == 1)
			  cur->type.vec = 4;
		  else
			  cur->type.vec = 1;

    	  if(cur->func.name == 0)
    		  cur->type.type_code = -1;
    	  else if(cur->func.name == 1)
    		  cur->type.type_code = VEC_T;
    	  else
    		  cur->type.type_code = FLOAT_T;

		  node *ptr = cur->func.args;

          //Void input not allowed for functions
          if(ptr == NULL){
        	  fprintf(errorFile,"The three predefined functions do not allow a void input\n");
              break;
          }

		  ptr = ptr->args.args; //node with first valid expression

		  if(func == 0){

			  int type = ptr->type.type_code;
			  int vec = ptr->type.vec;

			  if(type != -1 &&
				 (!(type == VEC_T	||
				   type == IVEC_T)	||
				  !(vec == 3 ||
				    vec == 4))){
				  fprintf(errorFile,"Predefined function:dp3 expecting first argument as type:IVEC_T or VEC_T with dimension of 3 or 4, getting type: %s\n",
						  get_type_str(&(ptr->type)));
			  }

			  ptr = ptr->args.args;

			  if(type == VEC_T && (vec == 3 || vec == 4)){
				  cur->type.type_code = FLOAT_T;

				  if(ptr == NULL){
		    		  fprintf(errorFile,"Function dp3 expecting 2 arguments, but received only 1.\n");
		    		  break;
				  }

				  //Type consistency check
				  if(ptr->type.type_code != -1 &&
					 !(ptr->type.type_code == type &&
					   ptr->type.vec == vec)){
					  struct type_s t;
					  t.type_code = type;
					  t.vec = vec;
	        		  fprintf(errorFile,"Function dp3 expecting 2nd argument as type: %s, getting type: %s\n",
	        				  get_type_str(&(t)),
	        				  get_type_str(&(ptr->type)));
				  }


			  }
			  else if(type == IVEC_T && (vec == 3 || vec == 4)){
				  cur->type.type_code = INT_T;

				  if(ptr == NULL){
		    		  fprintf(errorFile,"Function dp3 expecting 2 arguments, but received only 1.\n");
		    		  break;
				  }

				  //Type consistency check
				  if(ptr->type.type_code != -1 &&
					 !(ptr->type.type_code == type &&
					   ptr->type.vec == vec)){
					  struct type_s t;
					  t.type_code = type;
					  t.vec = vec;
	        		  fprintf(errorFile,"Function dp3 expecting 2nd argument as type: %s, getting type: %s\n",
	        				  get_type_str(&(t)),
	        				  get_type_str(&(ptr->type)));
				  }
			  }
			  else{

				  if(ptr == NULL){
		    		  fprintf(errorFile,"Function dp3 expecting 2 arguments, but received only 1.\n");
		    		  break;
				  }

				  if(ptr->type.type_code != -1 &&
					 (!(ptr->type.type_code == VEC_T	||
					    ptr->type.type_code == IVEC_T)	||
					  !(ptr->type.vec == 3 ||
					    ptr->type.vec == 4))){
					  fprintf(errorFile,"Predefined function:dp3 expecting 2nd argument as type:IVEC_T or VEC_T with dimension of 3 or 4, getting type: %s\n",
							  get_type_str(&(ptr->type)));
				  }

			  }
	          //Too many arguments
	          if(ptr->args.args){
	    		  fprintf(errorFile,"Function dp3 expects 2 arguments, but received more.\n");
	          }

		  }
		  else if(func == 1){
			  if(ptr->type.type_code != -1 &&
				 !(ptr->type.type_code == VEC_T &&
				   ptr->type.vec == 4)){
				  fprintf(errorFile,"Predefined function:lit expecting argument as type:VEC_T with dimension of 4, getting type: %s\n",
						  get_type_str(&(ptr->type)));
			  }

	          //Too many arguments
	          if(ptr->args.args){
	    		  fprintf(errorFile,"Function lit expects 1 argument, but received more.\n");
	          }
		  }
		  else{
			  int type = ptr->type.type_code;
			  int vec = ptr->type.vec;

			  if(type != -1 &&
				 (!(type == FLOAT_T || type == INT_T) ||
				    vec != 1)){
				  fprintf(errorFile,"Predefined function:rsq expecting it's argument as type:FLOAT_T or INT_T, getting type: %s\n",
						  get_type_str(&(ptr->type)));
			  }

			  ptr = ptr->args.args;


	          //Too many arguments
	          if(ptr->args.args){
	    		  fprintf(errorFile,"Function lit expects 1 argument, but received more.\n");
	          }
		  }

          //Inheritance happened, so we are all good.
		  break;
	  }
	  case UNARY_EXPRESION_NODE:
		  cur->type.is_const = 0;
		  cur->type.vec = cur->unary_expr.right->type.vec;

		  if(cur->unary_expr.op == '-'){
			  if(cur->unary_expr.right->type.type_code == BOOL_T || cur->unary_expr.right->type.type_code == BVEC_T){
	    		  fprintf(errorFile,"Unary operator -, expects an arithmetic type, getting: %s.\n",
	    				  get_type_str(&(cur->unary_expr.right->type)));
	    		  cur->type.type_code = -1;
	    		  break;

			  }
			  cur->type.type_code = cur->unary_expr.right->type.type_code;

		  }
		  //'!'
		  else{
			  if(cur->unary_expr.right->type.type_code != BOOL_T || cur->unary_expr.right->type.type_code != BVEC_T ||
				 cur->unary_expr.right->type.type_code != -1 ){
	    		  fprintf(errorFile,"Unary operator !, expects a logical type, getting: %s.\n",
	    				  get_type_str(&(cur->unary_expr.right->type)));
	    		  cur->type.type_code = -1;
	    		  break;

			  }
			  cur->type.type_code = cur->unary_expr.right->type.type_code;
		  }



		  break;

	  case BINARY_EXPRESSION_NODE:{


		  cur->type.is_const = 0;

		  int left_vec = cur->binary_expr.left->type.vec;
		  int right_vec = cur->binary_expr.right->type.vec;

		  cur->type.vec = (left_vec > right_vec) ?
		  	  	  	  	  left_vec : right_vec;

		  int left_type = cur->binary_expr.left->type.type_code;
		  int right_type = cur->binary_expr.right->type.type_code;
		  int op = cur->binary_expr.op;

		  //Binary
		  if(op == AND || op == OR){
			  //Ensuring logical inputs
			  if((left_type == -1 || type_of_vector_element(left_type) == BOOL_T) &&
				 (right_type == -1 || type_of_vector_element(right_type) == BOOL_T)){

				  if(left_vec == right_vec){
					  //type equality already assured if sizes equal
					  cur->type.type_code = (left_type == -1) ? right_type : left_type;

				  }
				  else{
		    		  fprintf(errorFile,"Binary operator %s, expects inputs of equal dimension, getting: %s and %s\n",
		    				  get_op_str(op),
		    				  get_type_str(&(cur->binary_expr.left->type)),
		    				  get_type_str(&(cur->binary_expr.right->type)));

		    		  cur->type.type_code = -1;
				  }

			  }
			  else{
	    		  fprintf(errorFile,"Binary operator %s, expects logical type inputs, getting: %s and %s\n",
	    				  get_op_str(op),
	    				  get_type_str(&(cur->binary_expr.left->type)),
	    				  get_type_str(&(cur->binary_expr.right->type)));

	    		  cur->type.type_code = -1;
			  }
		  }
		  //Arithmetic
		  else{
			  //Ensuring arithmetic inputs
			  if(type_of_vector_element(left_type) != BOOL_T && type_of_vector_element(right_type) != BOOL_T){
				  if(op == '+' || op == '-' || op == EQ || op == NEQ){
					  if(left_vec == right_vec && (left_type == -1 || right_type == -1 || left_type == right_type)){
						  cur->type.type_code = (left_type == -1) ? right_type : left_type;

					  }
					  else{
			    		  fprintf(errorFile,"Binary operator %s, expects inputs of equal dimension and same type, getting: %s and %s\n",
			    				  get_op_str(op),
			    				  get_type_str(&(cur->binary_expr.left->type)),
			    				  get_type_str(&(cur->binary_expr.right->type)));

			    		  cur->type.type_code = -1;
					  }
				  }
				  else if(op == '*'){
					  if(left_type == -1 || right_type == -1 || type_of_vector_element(left_type) == type_of_vector_element(right_type)){
						  cur->type.type_code = (left_vec > right_vec) ?
								  	  	   	     left_type : right_type;
					  }
					  else{
						  fprintf(errorFile,"Binary operator %s, expects arguments of the same basic type, getting: %s and %s\n",
								  get_op_str(op),
								  get_type_str(&(cur->binary_expr.left->type)),
								  get_type_str(&(cur->binary_expr.right->type)));
						  cur->type.type_code = -1;
					  }
				  }
				  else{// /, ^, <, <=, >, >=
					  if(left_vec == 1 && right_vec == 1 &&
						 (left_type == -1 || right_type == -1 || left_type == right_type)){

						  cur->type.type_code = (left_vec > right_vec) ?
								  	  	   	     left_type : right_type;
					  }
					  else{
						  fprintf(errorFile,"Binary operator %s, expects arguments of the same type and dimension 1, getting: %s and %s\n",
								  get_op_str(op),
								  get_type_str(&(cur->binary_expr.left->type)),
								  get_type_str(&(cur->binary_expr.right->type)));
						  cur->type.type_code = -1;
					  }
				  }
			  }
			  else{
				  fprintf(errorFile,"Binary operator %s, expects arithmetic type inputs, getting: %s and %s\n",
						  get_op_str(op),
						  get_type_str(&(cur->binary_expr.left->type)),
						  get_type_str(&(cur->binary_expr.right->type)));

				  cur->type.type_code = -1;
			  }
		  }
	  }
		  break;

	  case BOOL_NODE:
		  cur->type.is_const = 1;
		  cur->type.type_code = BOOL_T;
		  cur->type.vec = 1;

		  break;

	  case INT_NODE:
		  cur->type.is_const = 1;
		  cur->type.type_code = INT_T;
		  cur->type.vec = 1;

		  break;

	  case FLOAT_NODE:
		  cur->type.is_const = 1;
		  cur->type.type_code = FLOAT_T;
		  cur->type.vec = 1;
	  	  break;

	  case NESTED_EXPRESSION_NODE:
		  cur->type.is_const = cur->nested_expr.expr->type.is_const;
		  cur->type.type_code = cur->nested_expr.expr->type.type_code;
		  cur->type.vec = cur->nested_expr.expr->type.vec;
		  break;

	  case EXP_VAR_NODE:
		  cur->type.is_const = cur->exp_var_node.var_node->type.is_const;
		  cur->type.type_code = cur->exp_var_node.var_node->type.type_code;
		  cur->type.vec = cur->exp_var_node.var_node->type.vec;
		  break;
		  //End of expression grammar


	  case VAR_NODE:{


		  if(DEBUG_SEMANTIC){
			printf("Entering code for %s\n", node_name(kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }
		  char *var_id = cur->var_node.id;
		  //Checking if we are accessing a predefine variable.
		  if(strcmp(var_id, "gl_FragColor") == 0 			||
		     strcmp(var_id, "gl_FragDepth") == 0 			||
		     strcmp(var_id, "gl_FragCoord") == 0){
			  fprintf(errorFile,"Tried to access a predefined write-only (result) variable\n");
			  cur->type.is_const = 0;
			  cur->type.type_code = -1;
			  cur->type.vec = 1;

			  break;
		  }

		  if(strcmp(var_id, "gl_TextCoord") == 0 			||
		     strcmp(var_id, "gl_Color") == 0 				||
		     strcmp(var_id, "gl_Secondary") == 0 			||
		     strcmp(var_id, "gl_FogFragCoord") == 0){
			  cur->type.is_const = 0;
			  cur->type.type_code = VEC_T;
			  cur->type.vec = 4;
			  break;
		  }

		  if(strcmp(var_id, "gl_Light_Half") == 0 			||
		     strcmp(var_id, "gl_Light_Ambient") == 0 		||
		     strcmp(var_id, "gl_Material_Shininess") == 0	||

		     strcmp(var_id, "env1") == 0 					||
		     strcmp(var_id, "env2") == 0 					||
		     strcmp(var_id, "env3") == 0){
			  cur->type.is_const = 1;
			  cur->type.type_code = VEC_T;
			  cur->type.vec = 4;
			  break;
		  }


		  if(DEBUG_SEMANTIC){
			printf("Pre symbol search %s\n", node_name(kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }

		  //At this point, we know this access isn't a predefined variable.

		  //Check if this symbol exists:
		  symbol_table_entry *var_entry = symbol_find(var_id);


		  if(DEBUG_SEMANTIC){
			printf("Post symbol search %s\n", node_name(kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }

		  if(var_entry == NULL){
			  fprintf(errorFile,"Variable with id: %s has not been declared.\n",
					  var_id);

			  cur->type.is_const = 0;
			  cur->type.type_code = -1;
			  cur->type.vec = 1;
			  break;
		  }

		  if(var_entry->is_init == 0){
			  fprintf(errorFile,"Variable with id: %s has not been initialized.\n",
					  var_id);
		  }

		  if(cur->var_node.is_array){
			  if(cur->var_node.index < 0 || cur->var_node.index >= var_entry->vec){
				  fprintf(errorFile,"Array access of variable with id: %s is out of bounds.\n",
						  var_id);

			  }
			  cur->type.vec = 1;
		  }
		  else{
			  cur->type.vec = var_entry->vec;
		  }

		  cur->type.is_const = var_entry->is_const;
		  cur->type.type_code = var_entry->type_code;
		  break;
	  }
	  case ARGUMENTS_NODE:
		  if(cur->args.expr){
			  cur->type.is_const = cur->args.expr->type.is_const;
			  cur->type.type_code = cur->args.expr->type.type_code;
			  cur->type.vec = cur->args.expr->type.vec;
		  }
		  else{
			  cur->type.is_const = 0;
			  cur->type.type_code = -1;
			  cur->type.vec = 1;
		  }

		  break;

	  default: break; //Error?
	  }


}


void ast_scope_generator(node *cur, int x){ //Done pre-post.

	if(cur->kind == SCOPE_NODE){
		scope_enter();
	}


}

const char* node_name(node_kind kind){


	switch(kind){

	case SCOPE_NODE:
		  return "SCOPE_NODE";

	  case DECLARATIONS_NODE:
		  return "DECLARATIONS_NODE";

	  case STATEMENTS_NODE:
		  return "STATEMENTS_NODE";

	  case DECLARATION_NODE: //Note, create symbol table will be done after whole tree is initialised
		  return "DECLARATION_NODE";

	  //Statement grammar
	  case ASSIGNMENT_NODE:
		  return "ASSIGNMENT_NODE";

	  case IF_STATEMENT_NODE:
		  return "IF_STATEMENT_NODE";

	  case NESTED_SCOPE_NODE:
		  return "NESTED_SCOPE_NODE";

	  case TYPE_NODE:
		  return "TYPE_NODE";

	  //Expression grammar
	  case CONSTRUCTOR_NODE:
		  return "CONSTRUCTOR_NODE";

	  case FUNCTION_NODE:
		  return "FUNCTION_NODE";

	  case UNARY_EXPRESION_NODE:
		  return "UNARY_EXPRESION_NODE";

	  case BINARY_EXPRESSION_NODE:
		  return "BINARY_EXPRESSION_NODE";

	  case BOOL_NODE:
		  return "BOOL_NODE";

	  case INT_NODE:
		  return "INT_NODE";

	  case FLOAT_NODE:
		  return "FLOAT_NODE";

	  case NESTED_EXPRESSION_NODE:
		  return "NESTED_EXPRESSION_NODE";

	  case EXP_VAR_NODE:
		  return "EXP_VAR_NODE";

	  case VAR_NODE:
		  return "VAR_NODE";

	  case ARGUMENTS_NODE:
		  return "ARGUMENTS_NODE";

	  default: return "WHAT_NODE?";; //Error?

	}
}


