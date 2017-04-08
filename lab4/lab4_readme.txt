Lab 4: Code Generation

Mengye Ren           998905430
Viktor Riabtsev      997544420

Responsibility: Mengye worked on generating assembly from AST, while Viktor work
ed on register conservation. Both of us discussed together about the math and bo
olean operations.

I. Math operations
1. +: Use the same assembly instruction (ADD).
2. -: Use the same assembly instruction (SUB).
3. *: Use the same assembly instruction (MUL).
4. /: Use RCP to take the reciprocal of the right hand side and then use MUL. If
they are integer then take FLR.
5. ^: Use the same assembly instruction (POW).
6. UMINUS: Use zero minus the operand.

II. Boolean operations
For scalar boolean, (1,1,1,1) is true and (0,0,0,0) is false.
For vector boolean, 1 and 0 are placed in different positions to denote true or 
false.
1. (A AND B) = (A * B)
2. (A OR B) = (A + B - A * B)
3. (NOT A) = ((1,1,1,1) - A)
4. (A < B) = (A - B < 0) (Use CMP to set true on A-B less than zero otherwise fa
lse)
5. (A <= B) = (B - A >= 0) (Use CMP to set false on B-A less than zero otherwise
true)
6. (A > B) = (B - A < 0) (Use CMP to set true on A-B less than zero otherwise fa
lse)
7. (A >= B) = (A - B >= 0) (Use CMP to set false on A-B less than zero otherwise
true)
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
  
III. If statements
1. First, get the bool value B for condition, true = (1,1,1,1), false = (0,0,0,0
).
2. Second, if there is no if-else statement outside then define:
  if-condition = 0 - B.
  else-condition = B - 1.
  Here is a table for all possible outcomes.
  B      if     else
  1      -1        0
  0       0       -1
  So, -1 is used as a conditional "true" for CMP instruction to compare less th
an zero.
3. Third, if there is nested if-else statement, we need to combine the previous 
conditions. We define:
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
4. Any assignment within the if-else block will use CMP instruction with the cur
rent condition variable (regarding in the if or else state). If CMP fails, then
the register is assigned to itself.
5. A stack is used to track the current frame of if-else block.
6. In-order tree-traversal are utilized for stack operation. After the condition
expression is traversed, the compiler will add one more stack element with corr
ectly computed if/else temporary condition variable name. After the if block sta
tement is traversed and there exists an else block, then compiler will set a fla
g to make the current state to be in the else block.
  
IV. Constants
Since the semantic check gurantees that contants are infact treated as constants
, we avoided the PARAM declaration and instead made all registers be of type TEM
P.

V. Code generation
The code generation involves pre-order, in-order, and post-order traversal.

Pre-order:
Scope: Enter scope to track user defined variable names in the inner scope.
Constructor: Enter constructor and set the flag. This affects how int/float/bool
nodes serialize them into strings. If not in a constructor, they look like "{0,
0,0,0}" but if in a constructor they look like "0".

In-order:
If statement tracking. Details are explained above.

Post-order:
Scope: exit scope.
Unary/binary: Generate assembly code as explained above. Store the output regist
er name in the node.
Int/float/boolean: Store a string "{val,val,val,val}" in the struct if it is not
in a constructor. Otherwise just store "val".
Var: Check for system variables to map to ARB fragment variables. Otherwise find
the previously defined temporary register name (from the symbol table) and stor
e in the node. If the var node is an array, then append the corresponding .x, .y
, .x, or .w.
Function: Add instruction call with the appropriate names stores in the argument
s.
Constructor: Wrap "{" "}" around the arguments list. Exit constructor.
Arguments: Store all register names in its children.
Declaration: Allocate user defined variable register, MOV the initiated value to
the register (if has any). Add the newly defined variable to symbol table (impl
emented in the last lab).

Symbol table:
A symbol table is implemented from last lab. We still need to use it as we need 
to differentiate variables with same name but declared in different scopes.
Thus, a unique scope id is assigned to each variable and reflected in its corres
ponding assembly register name.

Storage details:
A linked list is used for storing the instructions. Each instruction is stored i
n a struct.
A formatting function is written to convert the struct into a nice-looking assem
bly string.
To serialize the entire program, simply traverse the linked list from the head n
ode and format each node.

VI. Register conservation
This task initially seemed very simple. One keeps a list of names declared so fa
r, and as you traverse the assembly instruction list, you alwayes ask, will this
register that I just used, never be used again? If so, then you set that regist
er name as "free". That way, if someone tries to declare say another temporary r
egister, then that instruction is thrown out, and that "free" register name is n
ow used everywhere instead.
The biggest challange that presented it self is the use of array index accesses.
i.e regName.x, regName.y and so on. Where as before I simply swapped char*'s ar
ound, now I had rewrite the originals with the remapped name + .x or .y so on. T
he checks used to see if a varible is referenced later had to all be changed fro
m simple strcmp() == 0 on each name, to considering cases with .x and .y so on.
In the end, the phrong.frag assembly went from using 11 temp registers to 1.
