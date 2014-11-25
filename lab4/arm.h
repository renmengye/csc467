/*
 * arm.h
 *
 *  Created on: 2014-11-25
 *      Author: riabtse1
 */

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
  OPERATION 	= 0,
  DECLARATION 	= 1,
  BRANCH		= 2,


} arm_node_kind;


struct _assembly_node{

	arm_node_kind kind; //Distinguish declarations from actual operations and branch nodes


	union{

		struct instr{
		char *op;

		char *in1, *in2, *in3;

		//In the last step, when namespace is conserved, the function that keeps track of used/free var names
		//needs to ignore the input if it is a literal.
		int is_lit1, is_lit2, is_lit2;

		char *out;
		} instr;

		struct branch{
			arm_node *condition;
			arm_node *if_code;
			arm_node *else_code;

			//There should be a map of "outside variable name" -> "temporary name" that is made during the first
			//pass after the arm tree is generated. This will used in the joining process, with CMP instruction
			//to load in final values from the temporaries.

		}branch;


	};
};




#endif /* ARM_H_ */
