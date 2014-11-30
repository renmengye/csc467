Lab 4: Code Generation

 Mengye Ren           998905430
 Viktor Riabtsev      997544420

General implementation questions:
-How did you implement non-trivial math operations
1. ADD: Use the same assembly instruction.
2. 

-How did you handle boolean types.
We used {1, 1, 1, 1} for simple bool. And for vector bools we'd use 1 where it counted, and 0 elsewhere.
Boolean instructions are converted to math operations.
1. A AND B = A * B
2. A OR B = 

-How did you implement if statements.
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

After coming up with the structures that would be involed in our solution, we split the work as follows.
1. Generating basic assembly.
2. Conserving register names.

Mengye focused on part 1, while Viktor worked on part 2.


1. Generating basic assembly.



2. Conserving register names.

This task initially seemed very simple. One keeps a list of names declared so far, and as you traverse the assembly instruction list, you alwayes ask, will this register that I just used, never be used again? If so, then you set that register name as "free". That way, if someone tries to declare say another temporary register, then that instruction is thrown out, and that "free" register name is now used everywhere instead.

The biggest challange that presented it self is the use of array index accesses. i.e regName.x, regName.y and so on. Where as before I simply swapped char*'s around, now I had rewrite the originals with the remapped name + .x or .y so on. The checks used to see if a varible is referenced later had to all be changed from simple strcmp() == 0 on each name, to considering cases with .x and .y so on.

In the end, the phrong.frag assembly went from using 11 temp registers to 1.
