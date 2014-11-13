#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0


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
	  ast->declaration.type = va_arg(args, node *);
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
	  ast->type.type = va_arg(args, int);
	  ast->type.vec = va_arg(args, int);
	  break;

  //Expression grammar
  case CONSTRUCTOR_NODE:
	  ast->ctor.type = va_arg(args, node *);
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
	  ast->nested_expr = va_arg(args, node *);
	  break;

  case EXP_VAR_NODE:
	  ast->var_node = va_arg(args, node *);
	  break;

  case VAR_NODE:
	  ast->var.id = va_arg(args, char *);
	  ast->var.isArray = va_arg(args, int);
	  ast->var.dim = va_arg(args, int);
	  break;

  case ARGUMENTS_NODE:
	  ast->args.args = va_arg(args, node *); //Could be NULL
	  ast->args.expr = va_arg(args, node *); //Could be NULL

  default: break; //Error?
  }

  va_end(args);

  return ast;
}

void ast_free(node *ast) {

}

void ast_print(node * ast) {

}



//void ast_print_node(node *cur, int level) {
//  fprintf(dumpFile, "\n");
//  int lv_i = 0;
//  for(; lv_i < level; ++lv_i) {
//    fprintf(dumpFile, "    ");
//  }
//  fprintf(dumpFile, "(");
//  switch(cur->kind) {
//      case SCOPE_NODE:
//        fprintf(dumpFile, "SCOPE");
//        break;
//      case UNARY_EXPRESION_NODE:
//        ast_traverse(cur->unary_expr.right, pre_func);
//        break;
//      case BINARY_EXPRESSION_NODE:
//        ast_traverse(cur->binary_expr.left, pre_func);
//        ast_traverse(cur->binary_expr.right, pre_func);
//        break;
//      case INT_NODE:
//        fprintf(dumpFile, "%d", cur->int_val);
//        break;
//      case FLOAT_NODE:
//        /* Do nothing */
//        break;
//      case IDENT_NODE:
//        /* Do nothing */
//        break;
//      case TYPE_NODE:
//        /* Do nothing */
//        break;
//      case BOOL_NODE:
//        /* Do nothing */
//        break;
//      case VAR_NODE:
//        ast_traverse(cur->var.id, pre_func);
//        break;
//      case FUNCTION_NODE:
//        if (cur->func.args)
//          ast_traverse(cur->func.args, pre_func);
//        break;
//      case CONSTRUCTOR_NODE:
//        ast_traverse(cur->ctor.type, pre_func);
//        ast_traverse(cur->ctor.args, pre_func);
//        break;
//      case ARGUMENTS_NODE:
//        if (cur->args.args)
//          ast_traverse(cur->args.args, pre_func);
//        break;
//
//      case STATEMENTS_NODE:
//        if (cur->stmts.stmts)
//          ast_traverse(cur->stmts.stmts, pre_func);
//        ast_traverse(cur->stmts.stmt, pre_func);
//        break;
//      case IF_STATEMENT_NODE:
//        ast_traverse(cur->if_stmt.condition_expr, pre_func);
//        ast_traverse(cur->if_stmt.if_blk_stmt, pre_func);
//        if (cur->if_stmt.else_blk_stmt)
//          ast_traverse(cur->if_stmt.condition_expr, pre_func);
//        break;
//      case WHILE_STATEMENT_NODE:
//        /* Do nothing */
//        break;
//      case ASSIGNMENT_NODE:
//        ast_traverse(cur->assignment.variable, pre_func);
//        ast_traverse(cur->assignment.expr, pre_func);
//        break;
//      case NESTED_SCOPE_NODE:
//        ast_traverse(cur->nested_scope, pre_func);
//        break;
//
//      case DECLARATION_NODE:
//        ast_traverse(cur->declaration.type, pre_func);
//        ast_traverse(cur->declaration.id, pre_func);
//        if (cur->declaration.expr)
//          ast_traverse(cur->declaration.expr, pre_func);
//        break;
//      case DECLARATIONS_NODE:
//        if (cur->declarations.declarations)
//          ast_traverse(cur->declarations.declarations, pre_func);
//        ast_traverse(cur->declarations.declaration, pre_func);
//        break;
//      default:
//        /* Do nothing */
//        break;
//  }
//}



void ast_traverse(node* cur, int level, void (*pre_func(node *, int)), void (*post_func(node *, int))) {
  pre_func(cur, level);
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
      //ast_traverse(cur->var.id, level, pre_func, post_func);
      break;
    case FUNCTION_NODE:
      if (cur->func.args)
        ast_traverse(cur->func.args, level, pre_func, post_func);
      break;
    case CONSTRUCTOR_NODE:
      ast_traverse(cur->ctor.type, level, pre_func, post_func);
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
//    case WHILE_STATEMENT_NODE:
      /* Do nothing */
      break;
    case ASSIGNMENT_NODE:
      ast_traverse(cur->assignment.variable, level, pre_func, post_func);
      ast_traverse(cur->assignment.expr, level, pre_func, post_func);
      break;
    case NESTED_SCOPE_NODE:
      ast_traverse(cur->nested_scope, level, pre_func, post_func);
      break;

    case DECLARATION_NODE:
      ast_traverse(cur->declaration.type, level, pre_func, post_func);
//      ast_traverse(cur->declaration.id, level, pre_func, post_func);
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
}
