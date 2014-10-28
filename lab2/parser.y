%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * 
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

enum {
  DP3 = 0, 
  LIT = 1, 
  RSQ = 2
};

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}

// defines the yyval union
%union {
  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
}

%token          FLOAT_T
%token          INT_T
%token          BOOL_T
%token          CONST
%token          FALSE_C TRUE_C
%token          FUNC
%token          IF WHILE ELSE
%token          AND OR NEQ EQ LEQ GEQ


// links specific values of tokens to yyval
%token <as_vec>   VEC_T
%token <as_vec>   BVEC_T
%token <as_vec>   IVEC_T
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID

%left     '|'
%left     '&'
%nonassoc '=' NEQ '<' LEQ '>' GEQ
%left     '+' '-'
%left     '*' '/'
%right    '^'
%nonassoc '!' UMINUS

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler


	declaration missing empty rule 
 ***********************************************************************/
program
  :   scope                               { yTRACE("program -> scope"); }
  ;

scope
  :   '{' declarations statements '}'     { yTRACE("scope -> { declarations statements }"); }
  ;

declarations
  :   declarations declaration            { yTRACE("declarations -> declarations declaration"); }
  |   /* empty */                         { yTRACE("declarations -> empty"); }
  ;

declaration
  :   type ID ';'                         { yTRACE("declaration -> type ID ;"); }
  |   type ID '=' expression ';'          { yTRACE("declaration -> type ID = expression ;"); }
  |   CONST type ID '=' expression ';'    { yTRACE("declaration -> CONST type ID '=' expression ;"); }
  |   /* empty */                         { yTRACE("declaration -> empty"); }
  ;

statements
  :   statements statement                { yTRACE("statements -> statements statement"); }
  |   /* empty */                         { yTRACE("statements -> empty"); }
  ;

statement
  :   variable '=' expression ';'			{ yTRACE("statement -> variable = expression ;"); }
  |   IF '(' expression ')' statement else_statement	{ yTRACE("statement -> IF ( expression ) statement else_statement"); }
  |   WHILE '(' expression ')' statement		{ yTRACE("statement -> WHILE ( expression ) statement"); } 
  |   scope						{ yTRACE("statement -> scope"); }
  |   ';'                                               { yTRACE("statement -> ;"); }
  ;

else_statement
  :   ELSE statement			{ yTRACE("else_statement -> ELSE statement"); }
  |   /* empty */                       { yTRACE("else_statement -> empty"); }
  ;

statement_closed
  :   variable '=' expression ';'                                 { yTRACE("statement_closed -> variable = expression ;"); }
  |   BREAK ';'                                                   { yTRACE("statement_closed -> BREAK ;"); }
  |   RETURN expression_opt ';'                                   { yTRACE("statement_closed -> RETURN expression_opt ;"); }
  |   scope                                                       { yTRACE("statement_closed -> scope"); }
  |   WHILE '(' expression ')' statement_closed                   { yTRACE("statement_closed -> WHILE '(' expression ) statement_closed"); }
  |   IF '(' expression ')' statement_closed ELSE statement_closed{ yTRACE("statement_closed"); }
  |   ';'                                                         { yTRACE("statement_closed"); }
  ;

arguments_opt
  :   arguments
  |   /* empty */                         { yTRACE("arguments_opt"); }
  ;

type
  :   INT_T
  |   BOOL_T
  |   FLOAT_T
  |   VEC_T
  |   IVEC_T
  |   BVEC_T                             { yTRACE("type -> ");}
  ;

expression_opt
  :   expression
  |   /* empty */                         { yTRACE("expression_opt");}
  ;

expression
  :   INT_C
  |   FLOAT_C
  |   '-' expression
  |   expression '+' expression
  |   expression '-' expression
  |   expression '*' expression
  |   expression '/' expression
  |   expression '^' expression
  |   TRUE_C
  |   FALSE_C
  |   '!' expression
  |   expression '&' expression
  |   expression '|' expression
  |   expression '=' expression
  |   expression NEQ expression
  |   expression '<' expression
  |   expression LEQ expression
  |   expression '>' expression
  |   expression GEQ expression
  |   '(' expression ')'
  |   variable
  |   constructor_call                   { yTRACE("expression");}               
  ;

variable
  :   ID
  |   ID '[' expression ']'              { yTRACE("variable");}
  ;

constructor_call
  : type '(' arguments_opt ')'           { yTRACE("constructor_call");}
  ;

type
  :   INT_T
  |   BOOL_T
  |   FLOAT_T
  |   VEC_T
  |   IVEC_T
  |   BVEC_T                             { yTRACE("type -> BVEC%d_T");}
  ;
%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if(errorOccurred) {
    return;    /* Error has already been reported by scanner */
  } else {
    errorOccurred = 1;
  }

  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}

