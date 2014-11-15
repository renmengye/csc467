
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;

typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),
  
  UNARY_EXPRESION_NODE  = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  INT_NODE              = (1 << 2) | (1 << 5), 
  FLOAT_NODE            = (1 << 2) | (1 << 6),
  IDENT_NODE            = (1 << 2) | (1 << 7),
  VAR_NODE              = (1 << 2) | (1 << 8),
  FUNCTION_NODE         = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 10),
  ARGUMENTS_NODE        = (1 << 2) | (1 << 11),
  TYPE_NODE             = (1 << 2) | (1 << 12),
  BOOL_NODE             = (1 << 2) | (1 << 13),

  STATEMENTS_NODE       = (1 << 1) | (1 << 10),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 11),
//  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 12),
  ASSIGNMENT_NODE       = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 14),
  NESTED_EXPRESSION_NODE= (1 << 1) | (1 << 15),
  EXP_VAR_NODE			= (1 << 1) | (1 << 16),

  DECLARATION_NODE      = (1 << 15),
  DECLARATIONS_NODE     = (1 << 14),

} node_kind;


struct type_s {
  int type_code;
  int vec;
  int is_const;
};

struct node_ {

  // an example of tagging each node with a type
  node_kind kind;

  struct type_s type;

  union {
    struct {
      node *declarations;
      node *stmts;
    } scope;

    struct {
      node *declarations;
      node *declaration;
    } declarations;

    struct {
      int is_const;
      char *id;
      node *type_node;
      node *expr;
    } declaration;

    struct {
      node *stmts;
      node *stmt;
    } stmts;
  
    struct {
      int op;
      node *right;
    } unary_expr;

    struct {
      int op;
      node *left;
      node *right;
    } binary_expr;

    struct {
      char *id;
      int is_array;
      int index;
    } var_node;

    struct {
      node *var_node;
    } exp_var_node;

    node *nested_scope;

    struct{
      node *expr;
    } nested_expr;

    int int_val;
    float float_val;
    int bool_val;

    struct {
      node *condition_expr;
      node *if_blk_stmt;
      node *else_blk_stmt;
    } if_stmt;

    struct {
      struct type_s type;
      node *variable;
      node *expr;
    } assignment;

    struct {
      node *type_node;
      node *args;
    } ctor;

    struct {
      node *args;
      node *expr;
    } args;

    struct {
      int name;
      node *args;
    } func;
    // etc.
  };
};


typedef void (*TR_FUNC)(node *, int);
node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);
void ast_traverse(node* root, int level, 
                  TR_FUNC pre_func, 
                  TR_FUNC post_func);

#endif /* AST_H_ */
