/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     myToken1 = 258,
     myToken2 = 259,
     ID = 260,
     LBRAC = 261,
     RBRAC = 262,
     LSBRAC = 263,
     RSBRAC = 264,
     LSCOPE = 265,
     RSCOPE = 266,
     COMMA = 267,
     SEMICOLON = 268,
     LQUOTE = 269,
     RQUOTE = 270,
     LCOMMENT = 271,
     RCOMMENT = 272,
     PLUS = 273,
     MINUS = 274,
     MUL = 275,
     DIV = 276,
     POWER = 277,
     ASSIGN = 278,
     NOT = 279,
     AND = 280,
     OR = 281,
     EQ = 282,
     NEQ = 283,
     LT = 284,
     LE = 285,
     GT = 286,
     GE = 287,
     BOOL_L = 288,
     INT_L = 289,
     FLOAT_L = 290,
     CONST = 291,
     IF = 292,
     ELSE = 293,
     WHILE = 294,
     VOID_T = 295,
     INT_T = 296,
     BOOL_T = 297,
     FLOAT_T = 298,
     VEC2_T = 299,
     VEC3_T = 300,
     VEC4_T = 301,
     BVEC2_T = 302,
     BVEC3_T = 303,
     BVEC4_T = 304,
     IVEC2_T = 305,
     IVEC3_T = 306,
     IVEC4_T = 307,
     DP3_F = 308,
     LIT_F = 309,
     RSQ_F = 310
   };
#endif
/* Tokens.  */
#define myToken1 258
#define myToken2 259
#define ID 260
#define LBRAC 261
#define RBRAC 262
#define LSBRAC 263
#define RSBRAC 264
#define LSCOPE 265
#define RSCOPE 266
#define COMMA 267
#define SEMICOLON 268
#define LQUOTE 269
#define RQUOTE 270
#define LCOMMENT 271
#define RCOMMENT 272
#define PLUS 273
#define MINUS 274
#define MUL 275
#define DIV 276
#define POWER 277
#define ASSIGN 278
#define NOT 279
#define AND 280
#define OR 281
#define EQ 282
#define NEQ 283
#define LT 284
#define LE 285
#define GT 286
#define GE 287
#define BOOL_L 288
#define INT_L 289
#define FLOAT_L 290
#define CONST 291
#define IF 292
#define ELSE 293
#define WHILE 294
#define VOID_T 295
#define INT_T 296
#define BOOL_T 297
#define FLOAT_T 298
#define VEC2_T 299
#define VEC3_T 300
#define VEC4_T 301
#define BVEC2_T 302
#define BVEC3_T 303
#define BVEC4_T 304
#define IVEC2_T 305
#define IVEC3_T 306
#define IVEC4_T 307
#define DP3_F 308
#define LIT_F 309
#define RSQ_F 310




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 57 "parser.y"

  int num;



/* Line 2068 of yacc.c  */
#line 166 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


