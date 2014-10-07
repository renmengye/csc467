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
     LBR = 260,
     RBR = 261,
     LSCOPE = 262,
     RSCOPE = 263,
     PLUS = 264,
     MINUS = 265,
     MUL = 266,
     DIV = 267,
     XOR = 268,
     NOT = 269,
     AND = 270,
     OR = 271,
     EQ = 272,
     NEQ = 273,
     LT = 274,
     LE = 275,
     GT = 276,
     GE = 277,
     INT_T = 278,
     BOOL_T = 279,
     FLOAT_T = 280,
     VEC2_T = 281,
     VEC3_T = 282,
     VEC4_T = 283,
     BVEC2_T = 284,
     BVEC3_T = 285,
     BVEC4_T = 286,
     IVEC2_T = 287,
     IVEC3_T = 288,
     IVEC4_T = 289,
     D = 290
   };
#endif
/* Tokens.  */
#define myToken1 258
#define myToken2 259
#define LBR 260
#define RBR 261
#define LSCOPE 262
#define RSCOPE 263
#define PLUS 264
#define MINUS 265
#define MUL 266
#define DIV 267
#define XOR 268
#define NOT 269
#define AND 270
#define OR 271
#define EQ 272
#define NEQ 273
#define LT 274
#define LE 275
#define GT 276
#define GE 277
#define INT_T 278
#define BOOL_T 279
#define FLOAT_T 280
#define VEC2_T 281
#define VEC3_T 282
#define VEC4_T 283
#define BVEC2_T 284
#define BVEC3_T 285
#define BVEC4_T 286
#define IVEC2_T 287
#define IVEC3_T 288
#define IVEC4_T 289
#define D 290




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 57 "parser.y"

  int num;



/* Line 2068 of yacc.c  */
#line 126 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


