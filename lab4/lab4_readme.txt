Lab 4: Code Generation

 Mengye Ren           998905430
 Viktor Riabtsev      997544420

General implementation:
Math operations
1. +: Use the same assembly instruction (ADD).
2. -: Use the same assembly instruction (SUB).
3. *: Use the same assembly instruction (MUL).
4. /: Use RCP to take the reciprocal of the right hand side and then use MUL. If they are integer then take FLR.
5. ^: Use the same assembly instruction (POW).
6. UMINUS: Use zero minus the operand.

Boolean types
We used {1, 1, 1, 1} for simple bool. And for vector bools we'd use 1 where it counted, and 0 elsewhere.
Boolean instructions are converted to math operations.
1. (A AND B) = (A * B)
2. (A OR B) = (A + B - A * B)
3. (NOT A) = ((1,1,1,1) - A)
4. (A < B) = (A - B < 0) (Use CMP to set true on A-B less than zero otherwise false)
5. (A <= B) = (B - A >= 0) (Use CMP to set false on B-A less than zero otherwise true)
6. (A > B) = (B - A < 0) (Use CMP to set true on A-B less than zero otherwise false)
7. (A >= B) = (A - B >= 0) (Use CMP to set false on A-B less than zero otherwise true)
8. (A == B) = ((x-y >= 0) * (y-x >= 0))
   1) Calculate the difference d1 = x-y.
   2) Calculate the difference d2 = y-x.
   3) Use CMP to set d1 to be false if d1 less than zero, otherwise true.
   4) Use CMP to set d2 to be false if d2 less than zero, otherwise true.
   5) Return d1*d2.
9. (A != B) = ((x-y < 0) + (y-x < 0))
   1) Calculate the difference d1 = x-y.
   2) Calculate the difference d2 = y-x.
   3) Use CMP to set d1 to be true if d1 less than zero, otherwise false.
   4) Use CMP to set d2 to be true if d2 less than zero, otherwise false.
   5) Return d1+d2.

If statements
1. First, get the bool value B for condition, true = (1,1,1,1), false = (0,0,0,0).
2. Second, if there is no if-else statement outside then define:
   if-condition = 0 - B.
   else-condition = B - 1.
   Here is a table for all possible outcomes.
   B      if     else
   1      -1        0
   0       0       -1
   So, -1 is used as a conditional "true" for CMP instruction to compare less than zero.
3. Third, if there is nested if-else statement, we need to combine the previous conditions. We define:
   Previous active condition value = V.
   if-condition = V * B.
   else-condition = V - (V * B)
   Here is a table for all possible outcomes.
   
   if-condition:
   V \ B   1   0
   -1     -1   0    
   0       0   0
   
   else-condition:
   V \ B   1   0
   -1      0  -1
   0       0   0
4. Any assignment within the if-else block will use CMP instruction with the current condition variable (regarding in the if or else state)
5. A stack is used to track the current frame of if-else block.
6. In-order tree-traversal are utilized for stack operation. After the condition expression is traversed, the compiler will add one more stack element with correctly computed if/else temporary condition variable name. After the if block statement is traversed and there exists an else block, then compiler will set a flag to make the current state to be in the else block.
   
-How did you implement constants.
Since the semantic check gurantees that contants are infact treated as constants, we avoided the PARAM declaration and instead made all registers be of type TEMP.

-How the code for each type of node was generated.
The code generation involves pre-order, in-order, and post-order traversal.

Pre-order:
Scope: enter scope to track variable names.
Constructor: enter constructor.

In-order:
If statement tracking.

Post-order:
Scope: exit scope.
Unary/binary: Generate assembly code as explained above. Store the output register name in the node.
Int/float/boolean: Store a string "{val,val,val,val}" in the struct if it is not in a constructor. Otherwise just store "val".
Var: Check for system variables to map to ARB fragment variables. Otherwise find the previously defined temporary register name and store in the node. If the var node is an array, then append the corresponding .x, .y, .x, or .w.
Function: Add instruction call with the appropriate names stores in the arguments.
Constructor: Wrap "{" "}" around the arguments list. Exit constructor.
Arguments: Store all register names in its children.
Declaration: Allocate user defined variable register, MOV the initiated value to the register (if has any).

After coming up with the structures that would be involed in our solution, we split the work as follows.
1. Generating basic assembly.
2. Conserving register names.

Mengye focused on part 1, while Viktor worked on part 2.

1. Generating basic assembly.
For each expression node, the task is to compute the register name for storing the output value.
Most of the logic is in the post-order traversal.


2. Conserving register names.

This task initially seemed very simple. One keeps a list of names declared so far, and as you traverse the assembly instruction list, you alwayes ask, will this register that I just used, never be used again? If so, then you set that register name as "free". That way, if someone tries to declare say another temporary register, then that instruction is thrown out, and that "free" register name is now used everywhere instead.

The biggest challange that presented it self is the use of array index accesses. i.e regName.x, regName.y and so on. Where as before I simply swapped char*'s around, now I had rewrite the originals with the remapped name + .x or .y so on. The checks used to see if a varible is referenced later had to all be changed from simple strcmp() == 0 on each name, to considering cases with .x and .y so on.

In the end, the phrong.frag assembly went from using 11 temp registers to 1.
