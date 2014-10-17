%{
/**********************************************************************
 * TA email: maali@socialdynamics.ca
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Viktor Riabtsev	997544420
 *
 *
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"
#include "stdlib.h"


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
(true|false)/{DELIM}	{	yylval.boolVal = (strncmp(yytext, "true", 5*sizeof(char)) == 0) ? 1 : 0; 
				yTRACE(BOOL_L); return BOOL_L;}

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

gl_FragColor/{DELIM}	{yTRACE(GL_FRAGCOLOR_V); return GL_FRAGCOLOR_V;}
gl_FragDepth/{DELIM}	{yTRACE(GL_FRAGDEPTH_V); return GL_FRAGDEPTH_V;}
gl_FragCoord/{DELIM}	{yTRACE(GL_FRAGCOORD_V); return GL_FRAGCOORD_V;}
gl_TexCoord/{DELIM}	{yTRACE(GL_TEXCOORD_V); return GL_TEXCOORD_V;}
gl_Color/{DELIM}	{yTRACE(GL_COLOR_V); return GL_COLOR_V;}
gl_Secondary/{DELIM}	{yTRACE(GL_SECONDARY_V); return GL_SECONDARY_V;}
gl_FogFragCoord/{DELIM}	{yTRACE(GL_FOGFRAGCOORD_V); return GL_FOGFRAGCOORD_V;}
gl_Light_Half/{DELIM}	{yTRACE(GL_LIGHT_HALF_V); return GL_LIGHT_HALF_V;}
gl_Light_Ambient/{DELIM}	{yTRACE(GL_LIGHT_AMBIENT_V); return GL_LIGHT_AMBIENT_V;}
gl_Material_Shininess/{DELIM}	{yTRACE(GL_MATERIAL_SHININESS_V); return GL_MATERIAL_SHININESS_V;}
env1/{DELIM}	{yTRACE(ENV1_V); return ENV1_V;}
env2/{DELIM}	{yTRACE(ENV2_V); return ENV2_V;}
env3/{DELIM}	{yTRACE(ENV3_V); return ENV3_V;}

[a-zA-Z_][a-zA-Z0-9_]* {if(yyleng > 32)
			{
				yERROR("Identifier exceeds maximum length");
			}
			else
			{
				yylval.idName = (char *) malloc((yyleng+1)*sizeof(char));
				if(yylval.idName != NULL) 
					strncpy(yylval.idName, yytext, yyleng+1);
				else
				{
					yERROR("Allocating memory for identifier string failed.");
				}
 			
				yTRACE(ID); return ID;
			}
		       }

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

(([1-9][0-9]*)|0)/{DELIM}	{
					int a = atoi(yytext);
					char b[11];
					sprintf(b, "%d", a);

					if(strcmp(yytext, b) != 0)
					{
						yERROR("Out of bounds int");
					}
					else
					{
						yylval.intVal = a; 
						yTRACE(INT_L); return INT_L;
					}				
				}
(([0-9]+"."[0-9]*)|([0-9]*"."[0-9]+))/{DELIM}	{yylval.intVal = atof(yytext); yTRACE(FLOAT_L); return FLOAT_L;}






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
