
#include <stdio.h>
#include "semantic.h"
#include "ast.h"



int type_of_vector_element(int vec_type){
	if(vec_type == BOOL_T ||
	   vec_type == FLOAT_T ||
	   vec_type == INT_T)
		return vec_type;

	if(vec_type == BVEC_T)
		return BOOL_T;
	else if(vec_type == VEC_T)
		return FLOAT_T;
	else
		return INT_T;
}



void ast_check_semantics(){
	if(DEBUG_SEMANTIC){
		printf("Semantic check starting\n");
		int i = 0;
		for(i = 0; i < 20; i++){
			printf("-");
		}
		printf("\n");
	}

	if(ast == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Main scope not found.\n");
		return;
	}
	else{
		ast_traverse(ast, 0, &ast_scope_generator, &ast_sementic_check);

	}

}

void ast_sementic_check(node* cur, int x){ //Done bottom-up.
	if(cur == NULL){
		errorOccurred = 1;
		fprintf(errorFile,"Semantic function visited a NULL node, should not have happened\n");
		return;
	}

	node_kind kind = cur->kind;

	if(DEBUG_SEMANTIC){
		printf("kind: %s\n", node_name(kind));
		int i = 0;
		for(i = 0; i < 20; i++){
			printf("-");
		}
		printf("\n");

	}
	switch(kind) {

	  case SCOPE_NODE: //Everything in this scope has been dealt with, need to exit the scope.
		  scope_exit();
		  break;

	  case DECLARATIONS_NODE:
		  break;

	  case STATEMENTS_NODE:
		  break;

		  /* In the case of a declaration being initialised, we need to know the type of
		   * that expression.  */
	  case DECLARATION_NODE:

		  if(symbol_exists_in_this_scope(cur->declaration.id)){
			  fprintf(errorFile,"Variable with ID: %s, already exists in this scope.\n", cur->declaration.id);
			  break;
		  }

		  //Checking that we'arnt trying to declare a variable from the predifined list
		  if(strcmp(cur->declaration.id, "gl_FragColor") == 0 			||
		     strcmp(cur->declaration.id, "gl_FragDepth") == 0 			||
		     strcmp(cur->declaration.id, "gl_FragCoord") == 0 			||

		     strcmp(cur->declaration.id, "gl_TextCoord") == 0 			||
		     strcmp(cur->declaration.id, "gl_Color") == 0 				||
		     strcmp(cur->declaration.id, "gl_Secondary") == 0 			||
		     strcmp(cur->declaration.id, "gl_FogFragCoord") == 0 		||

		     strcmp(cur->declaration.id, "gl_Light_Half") == 0 			||
		     strcmp(cur->declaration.id, "gl_Light_Ambient") == 0 		||
		     strcmp(cur->declaration.id, "gl_Material_Shininess") == 0	||

		     strcmp(cur->declaration.id, "env1") == 0 					||
		     strcmp(cur->declaration.id, "env2") == 0 					||
		     strcmp(cur->declaration.id, "env3") == 0){
			  fprintf(errorFile,"Tried to declare a predefined variable: %s\n", cur->declaration.id);
			  break;
		  }

		  symbol_table_entry new_entry;

		  if(DEBUG_SEMANTIC){
			printf("Declaration %d, %d\n", cur->declaration.type_node->type.type_code, cur->declaration.type_node->type.vec);
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }
		  /* When a declared variable is also assigned a value. */
		  if(cur->declaration.expr){
			  //type mismatch/ only if not any on the right.
			  //Any type should have no error
			  if(cur->declaration.expr->type.type_code != -1 &&
				 !(cur->declaration.type_node->type.type_code == cur->declaration.expr->type.type_code &&
				   cur->declaration.type_node->type.vec == cur->declaration.expr->type.vec)){
				  fprintf(errorFile,"Declaration of %s, expecting type: %s, getting type: %s\n",
						  cur->declaration.id,
						  get_type_str(&(cur->declaration.type_node->type)),
						  get_type_str(&(cur->declaration.expr->type)));

			  }

			  //If const variable, it can only be assigned a literal or a uniform variable
			  if(cur->declaration.is_const){
				  if(!cur->declaration.expr->type.is_const){
					  fprintf(errorFile,"const variables can only be initialised with either a literal or a uniform predefined variable.\n");

				  }
			  }

			  new_entry.is_init = 1;
		  }
		  /* Pure declaration, uninitialised */
		  else{
			  new_entry.is_init = 0;
		  }

		  new_entry.id = cur->declaration.id;
		  new_entry.is_const = cur->declaration.is_const;
		  new_entry.type_code = cur->declaration.type_node->type.type_code;
		  new_entry.vec = cur->declaration.type_node->type.vec;

		  symbol_add(new_entry);

		  break;

		  //Statement grammar
	  case ASSIGNMENT_NODE:{

		  symbol_table_entry *varEntry = symbol_find(cur->assignment.variable->var_node.id);

		  //Actual variables
		  if(varEntry){

			  //If individual array access assignment
			  if(cur->assignment.variable->var_node.is_array){
				  //checking index that is ok is done in variable node

				  //Check for changing a const already initialised
				  if(varEntry->is_const &&
					 (varEntry->is_init & (1 << (cur->assignment.variable->var_node.index - 1))) ){
					  fprintf(errorFile,"Attempting to change a const variable.\n");
					  break;
				  }
			  }
			  else{
				  //Check for changing a const already initialised
				  if(varEntry->is_const && (varEntry->is_init == 0b1111 >> (4 - varEntry->vec)) ){
					  fprintf(errorFile,"Attempting to change a const variable.\n");
					  break;
				  }

			  }

			  //Type check
			  if(cur->assignment.expr->type.type_code != -1 &&
				 !(cur->assignment.variable->type.type_code == cur->assignment.expr->type.type_code &&
				   cur->assignment.variable->type.vec == cur->assignment.expr->type.vec)){
				  fprintf(errorFile,"Assignment of %s, expecting type: %s, getting type: %s\n",
						  cur->assignment.variable->var_node.id,
						  get_type_str(&(cur->assignment.variable->type)),
						  get_type_str(&(cur->assignment.expr->type)));
				  break;
			  }

			  //Check if const, can only be assigned literals or a uniform value
			  if(varEntry->is_const){
				  if(!cur->assignment.expr->type.is_const){
					  fprintf(errorFile,"const variables can only be initialised with either a literal or a uniform predefined variable.\n");
					  break;
				  }
			  }


			  //Checks complete, valid assignment

			  //Setting as initialised.
			  //If array access assignment, need to set correct bit
			  if(cur->assignment.variable->var_node.is_array){
				  varEntry->is_init = varEntry->is_init | (1 << (cur->assignment.variable->var_node.index - 1));

			  }
			  else{//else, assign appropriate value to init
				  //Check for changing a const already initialised
				  varEntry->is_init = 0b1111 >> (4 - varEntry->vec);

			  }

		  }
		  //Var ID not found in symbol table.
		  else{
			  //Predefined cases
			  if(strcmp(cur->assignment.variable->var_node.id, "gl_FragColor") == 0 ||
				 strcmp(cur->assignment.variable->var_node.id, "gl_FragDepth") == 0 ||
				 strcmp(cur->assignment.variable->var_node.id, "gl_FragCoord") == 0   ){
				  //These can only be modified in the main scope
				  if(!scope_is_in_main()){
					  fprintf(errorFile,"Predefined variables of type result can only be changed in the main scope\n");
					  break;
				  }

				  //Type check
				  if(cur->assignment.expr->type.type_code != -1 &&
					 !(cur->assignment.variable->type.type_code == cur->assignment.expr->type.type_code &&
					   cur->assignment.variable->type.vec == cur->assignment.expr->type.vec)){
					  fprintf(errorFile,"Assignment of %s, expecting type: %s, getting type: %s\n",
							  cur->assignment.variable->var_node.id,
							  get_type_str(&(cur->assignment.variable->type)),
							  get_type_str(&(cur->assignment.expr->type)));
					  break;
				  }

				  //Checks Complete, valid assignment
				  //Don't need to do anything
			  }
			  else{

				 if( strcmp(cur->assignment.variable->var_node.id, "gl_TextCoord") == 0 			||
					 strcmp(cur->assignment.variable->var_node.id, "gl_Color") == 0 				||
					 strcmp(cur->assignment.variable->var_node.id, "gl_Secondary") == 0 			||
					 strcmp(cur->assignment.variable->var_node.id, "gl_FogFragCoord") == 0 		||

					 strcmp(cur->assignment.variable->var_node.id, "gl_Light_Half") == 0 			||
					 strcmp(cur->assignment.variable->var_node.id, "gl_Light_Ambient") == 0 		||
					 strcmp(cur->assignment.variable->var_node.id, "gl_Material_Shininess") == 0	||

					 strcmp(cur->assignment.variable->var_node.id, "env1") == 0 					||
					 strcmp(cur->assignment.variable->var_node.id, "env2") == 0 					||
					 strcmp(cur->assignment.variable->var_node.id, "env3") == 0){
					 fprintf(errorFile,"Trying to modify a predefined variable that should not be: %s.\n", cur->assignment.variable->var_node.id);
					 break;
				 }
				  fprintf(errorFile,"Trying to assign to an undeclared variable %s.\n", cur->assignment.variable->var_node.id);
				  break;
			  }
		  }
		  break;
	  }

	  case IF_STATEMENT_NODE:
		  //Need to make sure the expression is of boolean type or any
		  if(cur->if_stmt.condition_expr->type.type_code != -1 &&
			 !(cur->if_stmt.condition_expr->type.type_code == BOOL_T &&
			   cur->if_stmt.condition_expr->type.vec == 1)){
			  fprintf(errorFile,"if condition, expecting type: BOOL_T, getting type: %s\n",
					  get_type_str(&(cur->if_stmt.condition_expr->type)));
			  break;
		  }

		  break;

	  case NESTED_SCOPE_NODE:
		  break;
		  //End of Statement grammar

	  case TYPE_NODE: //Leaf. Do nothing.
		  break;

		  //Expression grammar
	  case CONSTRUCTOR_NODE:{

		  //Inherit type
		  cur->type.is_const = cur->ctor.type_node->type.is_const;
		  cur->type.type_code = cur->ctor.type_node->type.type_code;
		  cur->type.vec = cur->ctor.type_node->type.vec;

		  int cur_vec = cur->type.vec;
		  node *ptr = cur->ctor.args;

          //Void input not allowed for constructor
          if(ptr == NULL){
        	  fprintf(errorFile,"Constructor is not allowed a void input\n");
              break;
          }

		  if(DEBUG_SEMANTIC){
			printf("Constructor Node: %s\n", get_type_str(&(cur->type)));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }

          do{
        	  ptr = ptr->args.args; //node with first valid expression

        	  if(ptr->type.is_const == 0){
        		  cur->type.is_const = 0;
        	  }

        	  //type check
        	  if(ptr->type.type_code != -1 &&
				 !(type_of_vector_element(cur->type.type_code) == ptr->type.type_code &&
				   1 == ptr->type.vec)){
        		  struct type_s t;
        		  t.type_code = type_of_vector_element(cur->type.type_code);
        		  t.vec = 1;
        		  fprintf(errorFile,"Constructor cell at index %d: expecting type: %s, getting type: %s\n",
        				  cur_vec - 1,
        				  get_type_str(&(t)),
        				  get_type_str(&(ptr->type)));
        	  }

        	  cur_vec--;

          } while(cur_vec > 0 && ptr->args.args);

          //Not enough arguments in the constructor.
          if(cur_vec > 0){
    		  fprintf(errorFile,"Constructor expecting %d arguments, but received only %d.\n",
    				  cur->type.vec, cur->type.vec - cur_vec);

          }

          //Too many arguments
          if(ptr->args.args){
    		  fprintf(errorFile,"Constructor expecting %d arguments, but received more.\n",
    				  cur->type.vec);
          }

          //Inheritance happened, so we are all good.

		  break;
      }
	  case FUNCTION_NODE:{

		  //DP3 = 0, FLOAT_T if VEC4 or VEC3 arguments, INT_T if IV (VEC4, VEC4) or (VEC3, VEC3)
		  //LIT = 1, VEC4, (VEC4)
		  //RSQ = 2  FLOAT, (float) or (int)


		  cur->type.is_const = 0;

		  int func = cur->func.name;

		  if(func == 1)
			  cur->type.vec = 4;
		  else
			  cur->type.vec = 1;

    	  if(cur->func.name == 0)
    		  cur->type.type_code = -1;
    	  else if(cur->func.name == 1)
    		  cur->type.type_code = VEC_T;
    	  else
    		  cur->type.type_code = FLOAT_T;

		  node *ptr = cur->func.args;

          //Void input not allowed for functions
          if(ptr == NULL){
        	  fprintf(errorFile,"The three predefined functions do not allow a void input\n");
              break;
          }

		  ptr = ptr->args.args; //node with first valid expression

		  if(func == 0){

			  int type = ptr->type.type_code;
			  int vec = ptr->type.vec;

			  if(type != -1 &&
				 (!(type == VEC_T	||
				   type == IVEC_T)	||
				  !(vec == 3 ||
				    vec == 4))){
				  const char *a;
				  if(ptr->args.args == NULL)
					  a = "1st";
				  else
					  a = "2nd";

				  fprintf(errorFile,"Predefined function:dp3 expecting %s argument as type:IVEC_T or VEC_T with dimension of 3 or 4, getting type: %s\n",
						  a,
						  get_type_str(&(ptr->type)));
			  }

			  ptr = ptr->args.args;

			  if(type == VEC_T && (vec == 3 || vec == 4)){
				  cur->type.type_code = FLOAT_T;

				  if(ptr == NULL){
		    		  fprintf(errorFile,"Function dp3 expecting 2 arguments, but received only 1.\n");
		    		  break;
				  }

				  //Type consistency check
				  if(ptr->type.type_code != -1 &&
					 !(ptr->type.type_code == type &&
					   ptr->type.vec == vec)){
					  struct type_s t;
					  t.type_code = type;
					  t.vec = vec;
	        		  fprintf(errorFile,"Function dp3 expecting 1st argument as type: %s, getting type: %s\n",
	        				  get_type_str(&(t)),
	        				  get_type_str(&(ptr->type)));
				  }


			  }
			  else if(type == IVEC_T && (vec == 3 || vec == 4)){
				  cur->type.type_code = INT_T;

				  if(ptr == NULL){
		    		  fprintf(errorFile,"Function dp3 expecting 2 arguments, but received only 1.\n");
		    		  break;
				  }

				  //Type consistency check
				  if(ptr->type.type_code != -1 &&
					 !(ptr->type.type_code == type &&
					   ptr->type.vec == vec)){
					  struct type_s t;
					  t.type_code = type;
					  t.vec = vec;
	        		  fprintf(errorFile,"Function dp3 expecting first argument as type: %s, getting type: %s\n",
	        				  get_type_str(&(t)),
	        				  get_type_str(&(ptr->type)));
				  }
			  }
			  else{

				  if(ptr == NULL){
		    		  fprintf(errorFile,"Function dp3 expecting 2 arguments, but received only 1.\n");
		    		  break;
				  }

				  if(ptr->type.type_code != -1 &&
					 (!(ptr->type.type_code == VEC_T	||
					    ptr->type.type_code == IVEC_T)	||
					  !(ptr->type.vec == 3 ||
					    ptr->type.vec == 4))){
					  fprintf(errorFile,"Predefined function:dp3 expecting first argument as type:IVEC_T or VEC_T with dimension of 3 or 4, getting type: %s\n",
							  get_type_str(&(ptr->type)));
				  }

			  }
	          //Too many arguments
	          if(ptr->args.args){
	    		  fprintf(errorFile,"Function dp3 expects 2 arguments, but received more.\n");
	          }

		  }
		  else if(func == 1){
			  if(ptr->type.type_code != -1 &&
				 !(ptr->type.type_code == VEC_T &&
				   ptr->type.vec == 4)){
				  fprintf(errorFile,"Predefined function:lit expecting argument as type:VEC_T with dimension of 4, getting type: %s\n",
						  get_type_str(&(ptr->type)));
			  }

	          //Too many arguments
	          if(ptr->args.args){
	    		  fprintf(errorFile,"Function lit expects 1 argument, but received more.\n");
	          }
		  }
		  else{
			  int type = ptr->type.type_code;
			  int vec = ptr->type.vec;

			  if(type != -1 &&
				 (!(type == FLOAT_T || type == INT_T) ||
				    vec != 1)){
				  fprintf(errorFile,"Predefined function:rsq expecting it's argument as type:FLOAT_T or INT_T, getting type: %s\n",
						  get_type_str(&(ptr->type)));
			  }

			  ptr = ptr->args.args;


	          //Too many arguments
	          if(ptr->args.args){
	    		  fprintf(errorFile,"Function lit expects 1 argument, but received more.\n");
	          }
		  }

          //Inheritance happened, so we are all good.
		  break;
	  }
	  case UNARY_EXPRESION_NODE:
		  cur->type.is_const = 0;
		  cur->type.vec = cur->unary_expr.right->type.vec;

		  if(cur->unary_expr.op == '-'){
			  if(cur->unary_expr.right->type.type_code == BOOL_T || cur->unary_expr.right->type.type_code == BVEC_T){
	    		  fprintf(errorFile,"Unary operator -, expects an arithmetic type, getting: %s.\n",
	    				  get_type_str(&(cur->unary_expr.right->type)));
	    		  cur->type.type_code = -1;
	    		  break;

			  }
			  cur->type.type_code = cur->unary_expr.right->type.type_code;

		  }
		  //'!'
		  else{
			  if(cur->unary_expr.right->type.type_code != BOOL_T || cur->unary_expr.right->type.type_code != BVEC_T ||
				 cur->unary_expr.right->type.type_code != -1 ){
	    		  fprintf(errorFile,"Unary operator !, expects a logical type, getting: %s.\n",
	    				  get_type_str(&(cur->unary_expr.right->type)));
	    		  cur->type.type_code = -1;
	    		  break;

			  }
			  cur->type.type_code = cur->unary_expr.right->type.type_code;
		  }



		  break;

	  case BINARY_EXPRESSION_NODE:{


		  cur->type.is_const = 0;

		  int left_vec = cur->binary_expr.left->type.vec;
		  int right_vec = cur->binary_expr.right->type.vec;

		  cur->type.vec = (left_vec > right_vec) ?
		  	  	  	  	  left_vec : right_vec;

		  int left_type = cur->binary_expr.left->type.type_code;
		  int right_type = cur->binary_expr.right->type.type_code;
		  int op = cur->binary_expr.op;

		  //Binary
		  if(op == AND || op == OR){
			  //Ensuring logical inputs
			  if((left_type == -1 || type_of_vector_element(left_type) == BOOL_T) &&
				 (right_type == -1 || type_of_vector_element(right_type) == BOOL_T)){

				  if(left_vec == right_vec){
					  //type equality already assured if sizes equal
					  cur->type.type_code = (left_type == -1) ? right_type : left_type;

				  }
				  else{
		    		  fprintf(errorFile,"Binary operator %s, expects inputs of equal dimension, getting: %s and %s\n",
		    				  get_op_str(op),
		    				  get_type_str(&(cur->binary_expr.left->type)),
		    				  get_type_str(&(cur->binary_expr.right->type)));

		    		  cur->type.type_code = -1;
				  }

			  }
			  else{
	    		  fprintf(errorFile,"Binary operator %s, expects logical type inputs, getting: %s and %s\n",
	    				  get_op_str(op),
	    				  get_type_str(&(cur->binary_expr.left->type)),
	    				  get_type_str(&(cur->binary_expr.right->type)));

	    		  cur->type.type_code = -1;
			  }
		  }
		  //Arithmetic
		  else{
			  //Ensuring arithmetic inputs
			  if(type_of_vector_element(left_type) != BOOL_T && type_of_vector_element(right_type) != BOOL_T){
				  if(op == '+' || op == '-' || op == EQ || op == NEQ){
					  if(left_vec == right_vec && (left_type == -1 || right_type == -1 || left_type == right_type)){
						  cur->type.type_code = (left_type == -1) ? right_type : left_type;

					  }
					  else{
			    		  fprintf(errorFile,"Binary operator %s, expects inputs of equal dimension and same type, getting: %s and %s\n",
			    				  get_op_str(op),
			    				  get_type_str(&(cur->binary_expr.left->type)),
			    				  get_type_str(&(cur->binary_expr.right->type)));

			    		  cur->type.type_code = -1;
					  }
				  }
				  else if(op == '*'){
					  if(left_type == -1 || right_type == -1 || type_of_vector_element(left_type) == type_of_vector_element(right_type)){
						  cur->type.type_code = (left_vec > right_vec) ?
								  	  	   	     left_type : right_type;
					  }
					  else{
						  fprintf(errorFile,"Binary operator %s, expects arguments of the same basic type, getting: %s and %s\n",
								  get_op_str(op),
								  get_type_str(&(cur->binary_expr.left->type)),
								  get_type_str(&(cur->binary_expr.right->type)));
						  cur->type.type_code = -1;
					  }
				  }
				  else{// /, ^, <, <=, >, >=
					  if(left_vec == 1 && right_vec == 1 &&
						 (left_type == -1 || right_type == -1 || left_type == right_type)){

						  cur->type.type_code = (left_vec > right_vec) ?
								  	  	   	     left_type : right_type;
					  }
					  else{
						  fprintf(errorFile,"Binary operator %s, expects arguments of the same type and dimension 1, getting: %s and %s\n",
								  get_op_str(op),
								  get_type_str(&(cur->binary_expr.left->type)),
								  get_type_str(&(cur->binary_expr.right->type)));
						  cur->type.type_code = -1;
					  }
				  }
			  }
			  else{
				  fprintf(errorFile,"Binary operator %s, expects arithmetic type inputs, getting: %s and %s\n",
						  get_op_str(op),
						  get_type_str(&(cur->binary_expr.left->type)),
						  get_type_str(&(cur->binary_expr.right->type)));

				  cur->type.type_code = -1;
			  }
		  }
	  }
		  break;

	  case BOOL_NODE:
		  cur->type.is_const = 1;
		  cur->type.type_code = BOOL_T;
		  cur->type.vec = 1;

		  break;

	  case INT_NODE:
		  cur->type.is_const = 1;
		  cur->type.type_code = INT_T;
		  cur->type.vec = 1;

		  break;

	  case FLOAT_NODE:
		  cur->type.is_const = 1;
		  cur->type.type_code = FLOAT_T;
		  cur->type.vec = 1;
	  	  break;

	  case NESTED_EXPRESSION_NODE:
		  cur->type.is_const = cur->nested_expr.expr->type.is_const;
		  cur->type.type_code = cur->nested_expr.expr->type.type_code;
		  cur->type.vec = cur->nested_expr.expr->type.vec;
		  break;

	  case EXP_VAR_NODE:
		  cur->type.is_const = cur->exp_var_node.var_node->type.is_const;
		  cur->type.type_code = cur->exp_var_node.var_node->type.type_code;
		  cur->type.vec = cur->exp_var_node.var_node->type.vec;
		  break;
		  //End of expression grammar


	  case VAR_NODE:{


		  if(DEBUG_SEMANTIC){
			printf("Entering code for %s\n", node_name(kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }
		  char *var_id = cur->var_node.id;
		  //Checking if we are accessing a predefine variable.
		  if(strcmp(var_id, "gl_FragColor") == 0 			||
		     strcmp(var_id, "gl_FragDepth") == 0 			||
		     strcmp(var_id, "gl_FragCoord") == 0){
			  fprintf(errorFile,"Tried to access a predefined write-only (result) variable\n");
			  cur->type.is_const = 0;
			  cur->type.type_code = -1;
			  cur->type.vec = 1;

			  break;
		  }

		  if(strcmp(var_id, "gl_TextCoord") == 0 			||
		     strcmp(var_id, "gl_Color") == 0 				||
		     strcmp(var_id, "gl_Secondary") == 0 			||
		     strcmp(var_id, "gl_FogFragCoord") == 0){
			  cur->type.is_const = 0;
			  cur->type.type_code = VEC_T;
			  cur->type.vec = 4;
			  break;
		  }

		  if(strcmp(var_id, "gl_Light_Half") == 0 			||
		     strcmp(var_id, "gl_Light_Ambient") == 0 		||
		     strcmp(var_id, "gl_Material_Shininess") == 0	||

		     strcmp(var_id, "env1") == 0 					||
		     strcmp(var_id, "env2") == 0 					||
		     strcmp(var_id, "env3") == 0){
			  cur->type.is_const = 1;
			  cur->type.type_code = VEC_T;
			  cur->type.vec = 4;
			  break;
		  }


		  if(DEBUG_SEMANTIC){
			printf("Pre symbol search %s\n", node_name(kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }

		  //At this point, we know this access isn't a predefined variable.

		  //Check if this symbol exists:
		  symbol_table_entry *var_entry = symbol_find(var_id);


		  if(DEBUG_SEMANTIC){
			printf("Post symbol search %s\n", node_name(kind));
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }

		  if(var_entry == NULL){
			  fprintf(errorFile,"Variable with id: %s has not been declared.\n",
					  var_id);

			  cur->type.is_const = 0;
			  cur->type.type_code = -1;
			  cur->type.vec = 1;
			  break;
		  }

		  if(var_entry->is_init == 0){
			  fprintf(errorFile,"Variable with id: %s has not been initialized.\n",
					  var_id);
		  }

		  if(cur->var_node.is_array){
			  if(cur->var_node.index < 0 || cur->var_node.index >= var_entry->vec){
				  fprintf(errorFile,"Array access of variable with id: %s is out of bounds.\n",
						  var_id);

			  }
			  cur->type.vec = 1;
		  }
		  else{
			  cur->type.vec = var_entry->vec;
		  }

		  cur->type.is_const = var_entry->is_const;
		  cur->type.type_code = var_entry->type_code;
		  break;
	  }
	  case ARGUMENTS_NODE:
		  if(cur->args.expr){
			  cur->type.is_const = cur->args.expr->type.is_const;
			  cur->type.type_code = cur->args.expr->type.type_code;
			  cur->type.vec = cur->args.expr->type.vec;
		  }
		  else{
			  cur->type.is_const = 0;
			  cur->type.type_code = -1;
			  cur->type.vec = 1;
		  }

		  if(DEBUG_SEMANTIC){
			printf("Arguments Node: %d, %d\n", cur->type.type_code, cur->type.vec);
			int i = 0;
			for(i = 0; i < 20; i++){
				printf("-");
			}
			printf("\n");

		  }

		  break;

	  default: break; //Error?
	  }


}


void ast_scope_generator(node *cur, int x){ //Done pre-post.

	if(cur->kind == SCOPE_NODE){
		scope_enter();
	}


}

