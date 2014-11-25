#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ast.h"
#include "common.h"
#include "parser.tab.h"
#include "semantic.h"

#include "symbol.h"



#define DEBUG_PRINT_TREE 0


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
	  ast->type.is_const = 1;
	  ast->type.type_code = BOOL_T;
	  ast->type.vec = 1;
	  ast->bool_val = va_arg(args, int);
	  break;

  case INT_NODE:
	  ast->type.is_const = 1;
	  ast->type.type_code = INT_T;
	  ast->type.vec = 1;
	  ast->int_val = va_arg(args, int);
	  break;

  case FLOAT_NODE:
	  ast->type.is_const = 1;
	  ast->type.type_code = FLOAT_T;
	  ast->type.vec = 1;
  	  ast->float_val = va_arg(args, float);
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
	else if(ast->kind == VAR_NODE)
		free(ast->var_node.id);

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
      if (cur->args.expr)
        ast_traverse(cur->args.expr, level, pre_func, post_func);
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


