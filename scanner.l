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

WS          [ \t]



%%

[a-zA-Z_][a-zA-Z0-9]* {yTRACE(ID); return ID;}

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

[ \t\n]+
.           { yERROR("Unknown token"); }



%%
