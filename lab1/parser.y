%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 *
 *   Viktor Riabtsev	997544420
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


// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {
	int intVal;
	float floatVal;
	bool boolVal;
	char *idName;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
%token
myToken1
myToken2

UNKNOWN   

ID

LBRAC
RBRAC
LSBRAC
RSBRAC
LSCOPE
RSCOPE

COMMA
SEMICOLON



PLUS
MINUS
MUL
DIV
POWER

ASSIGN

NOT
AND
OR
EQ
NEQ
LT
LE
GT
GE

BOOL_L
INT_L
FLOAT_L

CONST

IF
ELSE
WHILE

VOID_T
INT_T
BOOL_T
FLOAT_T
VEC2_T
VEC3_T
VEC4_T
BVEC2_T
BVEC3_T
BVEC4_T
IVEC2_T
IVEC3_T
IVEC4_T

DP3_F
LIT_F
RSQ_F

GL_FRAGCOLOR_V
GL_FRAGDEPTH_V
GL_FRAGCOORD_V
GL_TEXCOORD_V
GL_COLOR_V
GL_SECONDARY_V
GL_FOGFRAGCOORD_V
GL_LIGHT_HALF_V
GL_LIGHT_AMBIENT_V
GL_MATERIAL_SHININESS_V
ENV1_V
ENV2_V
ENV3_V



%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  :   tokens       
  ;
tokens
  :  tokens token  
  |      
  ;
// TODO: replace myToken with the token the you defined.
token
  :     myToken1 
  |     myToken2                     
  ;


%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

