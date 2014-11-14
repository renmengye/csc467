#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

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
	  ast->type.vec = va_arg(args, int);
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

void ast_free(node *ast) {

}

void ast_print_node_post(node *cur, int level) {
  fprintf(dumpFile, "\n");
  int lv_i = 0;
  for(; lv_i < level; ++lv_i) {
    fprintf(dumpFile, "    ");
  }
  fprintf(dumpFile, ")");
}

void ast_print_node(node *cur, int level) {
  fprintf(dumpFile, "\n");
  int lv_i = 0;
  for(; lv_i < level; ++lv_i) {
    fprintf(dumpFile, "    ");
  }
  fprintf(dumpFile, "(");
  switch(cur->kind) {
     case SCOPE_NODE:
        fprintf(dumpFile, "SCOPE");
        break;
     case UNARY_EXPRESION_NODE:
        fprintf(dumpFile, "UNARY %s %s", get_type_str(&cur->type), get_op_str(cur->unary_expr.op));
        break;
     case BINARY_EXPRESSION_NODE:
        fprintf(dumpFile, "BINARY %s %s", get_type_str(&cur->type), get_op_str(cur->unary_expr.op));
        break;
     case INT_NODE:
        fprintf(dumpFile, "%d", cur->int_val);
        break;
     case FLOAT_NODE:
        fprintf(dumpFile, "%f", cur->float_val);
        break;
     case IDENT_NODE:
        /* Do nothing */
        break;
     case TYPE_NODE:
        fprintf(dumpFile, get_type_str(&cur->type));
        break;
     case BOOL_NODE:
        if (cur->bool_val) {
          fprintf(dumpFile, "true");
        } else {
          fprintf(dumpFile, "false");
        }
        break;
     case VAR_NODE:
        if (cur->var_node.is_array) {
          fprintf(dumpFile, 
            "INDEX %s %s %d", 
            get_type_str(&cur.type),
            cur->var_node.id, 
            cur->var_node.index);
        } else {
          fprintf(dumpFile, cur->var_node.id);
        }
        break;
     case FUNCTION_NODE:
        fprintf(dumpFile, "CALL %s", get_func_str(cur->func.name));
        break;
     case CONSTRUCTOR_NODE:
        fprintf(dumpFile, "CONSTRUCTOR %s", get_type_str(&cur->type));
        break;
     case ARGUMENTS_NODE:
        /* Do nothing */
        break;

     case STATEMENTS_NODE:
        fprintf(dumpFile, "STATEMENTS");
       break;
     case IF_STATEMENT_NODE:
        fprintf(dumpFile, "IF");
        break;
     //case WHILE_STATEMENT_NODE:
       /* Do nothing */
        //break;
     case ASSIGNMENT_NODE:
        fprintf(dumpFile, "ASSIGN %s", get_type_str(&cur->assignment.type));
        break;
     case NESTED_SCOPE_NODE:
        fprintf(dumpFile, "SCOPE");
        break;

     case DECLARATION_NODE:
        fprintf(dumpFile, "DECLARATION %s %s", cur->declaration.id, get_type_str(&cur->type));
        break;
     case DECLARATIONS_NODE:
        fprintf(dumpFile, "DECLARATIONS");
        break;
     default:
        /* Do nothing */
        break;
  }
}

void ast_print(node * ast) {
  ast_traverse(ast, 0, &ast_print_node, &ast_print_node_post);
  fprintf(dumpFile, '\n');
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
      return "";
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

  if (pre_func) pre_func(cur, level);  
  level++;
  switch(cur->kind) {
    case SCOPE_NODE:
      ast_traverse(cur->scope.declarations, level, pre_func, post_func);
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
        ast_traverse(cur->if_stmt.condition_expr, level, pre_func, post_func);
      break;
    //case WHILE_STATEMENT_NODE:
      /* Do nothing */
      //break;
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
  if (post_func) post_func(cur, level);
}


void ast_check_semantics(){
	if(ast == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Main scope not found.\n");
		return;
	}
	else{
		//call bottom-up traverse function with ast_sementicCheck function.
	}

}

void ast_sementic_check(node* cur){ //Done bottom-up.
	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Semantic function visited a NULL node, should not have happened\n");
		return;
	}

	node_kind kind = cur->kind;

	switch(kind) {

	  case SCOPE_NODE: //No errors possible here. Nothing to pass up.
		  break;

	  case DECLARATIONS_NODE:
		  break;

	  case STATEMENTS_NODE:
		  break;

		  /* In the case of a declaration being initialised, we need to know the type of
		   * that expression.  */
	  case DECLARATION_NODE: //In the case of

		  if(symbol_exists_in_this_scope(cur->declaration.id)){
			  fprintf(errorFile,"Variable with ID: %s, already exists in this scope.\n", cur->declaration.id);
			  break;
		  }

		  int initialised = 0;


		  /* When a declared variable is also assigned a value. */
		  if(cur->declaration.expr){
			  int expression_type, expression_vec;




			  //if(cur->declaration.type->type_node.type_code != cur->declaration.expr->)
			  //cur->declaration.type->type_node.type_code
			  //cur->declaration.type->type_node.vec

		  }
		  /* Pure declaration, uninitialised */
		  else{

		  }

		  symbol_add(cur->declaration.id);

		  break;

		  //Statement grammar
	  case ASSIGNMENT_NODE:
		  //cur->assignment.variable = va_arg(args, node *);
		  //cur->assignment.expr = va_arg(args, node *);
		  break;

	  case IF_STATEMENT_NODE:
		 // cur->if_stmt.condition_expr = va_arg(args, node *);
		  //cur->if_stmt.if_blk_stmt = va_arg(args, node *); //Could be NULL.
		  //cur->if_stmt.else_blk_stmt = va_arg(args, node *); //Could be NULL.
		  break;

	  case NESTED_SCOPE_NODE:
		  //cur->nested_scope = va_arg(args, node *);
		  break;

	  case TYPE_NODE: //Leaf. Do nothing.
		  break;
		  //End of Statement grammar

		  //Expression grammar
	  case CONSTRUCTOR_NODE:
		  //cur->ctor.type = va_arg(args, node *);
		  //cur->ctor.args = va_arg(args, node *); //Could be NULL.
		  break;

	  case FUNCTION_NODE:
		  //cur->func.name = va_arg(args, int);
		  //cur->func.args = va_arg(args, node *); //Could be NULL.
		  break;

	  case UNARY_EXPRESION_NODE:
		  //cur->unary_expr.op = va_arg(args, int);
		  //cur->unary_expr.right = va_arg(args, node *);
		  break;

	  case BINARY_EXPRESSION_NODE:
		  //cur->binary_expr.op = va_arg(args, int);
		  //cur->binary_expr.left = va_arg(args, node *);
		  //cur->binary_expr.right = va_arg(args, node *);
		  break;

	  case BOOL_NODE:
		  //cur->bool_val = va_arg(args, int);
		  break;

	  case INT_NODE:
		  //cur->int_val = va_arg(args, int);
		  break;

	  case FLOAT_NODE:
	  	  //cur->int_val = va_arg(args, int);
	  	  break;

	  case NESTED_EXPRESSION_NODE:
		  //cur->nested_expr = va_arg(args, node *);
		  break;

	  case EXP_VAR_NODE:
		  //cur->var_node = va_arg(args, node *);
		  break;
		  //End of expression grammar


	  case VAR_NODE:
		  //cur->var.id = va_arg(args, char *);
		  //cur->var.isArray = va_arg(args, int);
		  //cur->var.dim = va_arg(args, int);
		  break;

	  case ARGUMENTS_NODE:
		  //cur->args.args = va_arg(args, node *); //Could be NULL
		  //cur->args.expr = va_arg(args, node *); //Could be NULL

	  default: break; //Error?
	  }


}
