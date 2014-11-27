/*
 * arm.h
 *
 *  Created on: 2014-11-25
 *      Author: riabtse1
 */

#include "ast.h"

#ifndef ARM_H_
#define ARM_H_

/*Plan of action
 *
 -Create assembly code structure, API for adding a new one (doubly linked list/tree).
 During traversals, we're probably going to be adding AT some position in the list instead of just add.
 Functions relating to this will obviously be piling up as later stages are worked on/defined.

	Need to add to ast.h, the ast tree expression nodes needs to contain a "varible name" that is made as you go.
	Literals will have their values->string form as the variable name.

-Write function that will be used to traverse the AST and create the assembly tree.

-Write traversal function of this assembly code list/tree.

-Write function that will deal of the if/else blocks and combine them and join back to overall list
(i.e to where the branch actually occured).

-Write function that will move non constant varible declarations to where the varible is first used.

-Write function that go through assembly and conserver register names.
(will need a linked list of all varible names used so far,
with a field that indicates if they are free or not at that time)
 *
 */
typedef struct _assembly_node arm_node;

typedef enum {
    OPERATION = 0,
    DECLARATION = 1
} instr_kind;

typedef enum {
	ABS = 1,
	ADD = 2,
	COS = 3,
	CMP = 4,
	DP3 = 5,
	DP4 = 6,
	DPH = 7,// no dst or ex2?
	FLR = 8,
	FRC = 9,
	KIL = 10,
	LG2 = 11,
	LIT = 12,
	LRP = 13,
	MAD = 14,
	MAX = 15,
	MIN = 16,
	MOV = 17,
	MUL = 18,
	POW = 19,
	RCP = 20,
	RSQ = 21,
	SCS = 22,
	SGE = 23,
	SIN = 24,
	SLT = 25,
	SUB = 26,
	SWZ = 27,
	TEX = 28,
	TXB = 29,
	TXP = 30,
	XPD = 31
} op_kind;

int is_one_input(op_kind op){
	if(op == COS || //op == EX2 ||
	   op == FLR || op == FRC || op == KIL || op == LG2 ||
	   op == LIT || op == MOV || op == RCP || op == RSQ || op == SCS || op == SIN ||
	   op == SWZ){
		return 1;
	}
	return 0;
}

int is_two_input(op_kind op){
	if(op == ADD || op == DP3 || op == DP4 || op == DPH || //op == DST ||
	   op == MAX ||
	   op == MIN || op == MUL || op == POW || op == SGE || op == SLT || op == SUB ||
	   op == XPD){
		return 1;
	}
	return 0;
}

int is_three_input(op_kind op){
	if(op == CMP || op == LRP || op == MAD || op == TEX || op == TXB || op == TXP){
		return 1;
	}
	return 0;
}


struct _instr{
    instr_kind kind; //Distinguish declarations from actual operations and branch nodes
    op_kind op;
    char *in1, *in2, *in3;

    // Deleting is_lit. Just recycle registers start with temp.
    //In the last step, when namespace is conserved, the function that keeps track of used/free var names
    //needs to ignore the input if it is a literal.
    //int is_lit1, is_lit2, is_lit2;
    char *out;
    struct *_instr;
} instr;

struct _cond {
	char *name;
	struct _cond *next;
};

struct _cond* cur_cond;
instr *result;
int temp_reg_counter = 0;
void free_result();
char *get_instr_str(instr *inst);
instr *generate(node *ast);
void generate_post(node *cur, int level);
void enter_cond(char* cond_var);
void exit_cond();
char* get_cond();

#endif /* ARM_H_ */
