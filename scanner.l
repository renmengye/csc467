%{
/**********************************************************************
 * TA email: maali@socialdynamics.ca
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"


#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS	[ \t\n]
DELIM	[ \t\n\(\)\[\]\{\},;\+\-\*\/\^=!\<\>]|&&|"||"|==|!=|"<"=|">"=



%%
(true|false)/{DELIM}	{yTRACE(BOOL_L); return BOOL_L;}

const/{DELIM}	{yTRACE(CONST); return CONST;}

if/{DELIM}	{yTRACE(IF); return IF;}
else/{DELIM}	{yTRACE(ELSE); return ELSE;}
while/{DELIM}	{yTRACE(WHILE); return WHILE;}

void/{DELIM}	{yTRACE(VOID_T); return VOID_T;}
int/{DELIM}	{yTRACE(INT_T); return INT_T;}
bool/{DELIM}	{yTRACE(BOOL_T); return BOOL_T;}
float/{DELIM}	{yTRACE(FLOAT_T); return FLOAT_T;}
vec2/{DELIM}	{yTRACE(VEC2_T); return VEC2_T;}
vec3/{DELIM}	{yTRACE(VEC3_T); return VEC3_T;}
vec4/{DELIM}	{yTRACE(VEC4_T); return VEC4_T;}
bvec2/{DELIM}	{yTRACE(BVEC2_T); return BVEC2_T;}
bvec3/{DELIM}	{yTRACE(BVEC3_T); return BVEC3_T;}
bvec4/{DELIM}	{yTRACE(BVEC4_T); return BVEC4_T;}
ivec2/{DELIM}	{yTRACE(IVEC2_T); return IVEC2_T;}
ivec3/{DELIM}	{yTRACE(IVEC3_T); return IVEC3_T;}
ivec4/{DELIM}	{yTRACE(IVEC4_T); return IVEC4_T;}

dp3/{DELIM}	{yTRACE(DP3_F); return DP3_F;}
lit/{DELIM}	{yTRACE(LIT_F); return LIT_F;}
rsq/{DELIM}	{yTRACE(RSQ_F); return RSQ_F;}

[a-zA-Z_][a-zA-Z0-9_]* {yTRACE(ID); return ID;}

"("	{yTRACE(LBRAC); return LBRAC;}
")"	{yTRACE(RBRAC); return RBRAC;}
"["	{yTRACE(LSBRAC); return LSBRAC;}
"]"	{yTRACE(RSBRAC); return RSBRAC;}
"{"	{yTRACE(LSCOPE); return LSCOPE;}
"}"	{yTRACE(RSCOPE); return RSCOPE;}

,	{yTRACE(COMMA); return COMMA;}
;	{yTRACE(SEMICOLON); return SEMICOLON;}

"+"	{yTRACE(PLUS); return PLUS;}
"-"	{yTRACE(MINUS); return MINUS;}
"*"	{yTRACE(MUL); return MUL;}
"/"	{yTRACE(DIV); return DIV;}
"^"	{yTRACE(POWER); return POWER;}

=	{yTRACE(ASSIGN); return ASSIGN;}
!	{yTRACE(NOT); return NOT;}
&&	{yTRACE(AND); return AND;}
"||"	{yTRACE(OR); return OR;}
==	{yTRACE(EQ); return EQ;}
!=	{yTRACE(NEQ); return NEQ;}
"<"	{yTRACE(LT); return LT;}
"<"=	{yTRACE(LE); return LE;}
">"	{yTRACE(GT); return GT;}
">"=	{yTRACE(GE); return GE;}

(-{WS}*)?(([1-9][0-9]*)|0)/{DELIM}	{yTRACE(INT_L); return INT_L;}
(-{WS}*)?([0-9]+"."[0-9]*)/{DELIM}	{yTRACE(FLOAT_L); return FLOAT_L;}






"//"[^\n]*
"/"\"*"."*"?\"*/"
	
"/*"        {
              int c1      = 0;
              int c2      = yyinput(); 
              int curline = yyline;
              for(;;) {
                if (c2 == EOF) {
                  fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                  errorOccurred = TRUE;
                  yyterminate();
                }
                if (c1 == '*' && c2 == '/')
                  break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                  yyline++;
              }
            }

{WS}+


.	{ yERROR("Unknown token"); }



%%
