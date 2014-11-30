Lab 3: AST Construction and Semantic Checking

 Mengye Ren           998905430
 Viktor Riabtsev      997544420

In the lab we finished the following components:
1. Construction of AST using bison grammar.
2. Define tree node structure and print AST.
3. Semantic Checking.
4. Symbol Table.

Viktor Riabtsev did part (1) and (3). Mengye Ren did part (2) and (4).
We summarize the approach, challenges, and novelties in the following.

1. Construction of AST
1.1 Approach
Each parser grammar rule generates a node in the tree, or in the 
case of grammar rules that comeout of empty rules (like 
declarations -> empty), the pointer of that node is made NULL, 
to recongnize that there is nothing actually there. This also 
applies for the statement - > ';' generation rules, as that too, 
has no real further data in it.

The relevant grammar rules made their node pointer come from the 
ast_alloc() function, to which we would pass the node kind as the 
first argument, then other non-node pointer arguments first as 
necessary, then the pointers to leafs of that tree node. The node 
"kinds" is what Mengey worked on for the structure of the AST 
(section 2) and we both later contributed to, in order to 
differentiate the different grammar rules from each other.

1.2 Challenges
Deciding what is relevant for each type of node was someone of 
a challange. For example, one of the bugs we fixed was when we 
were passing INT or FLOAT literal values to their nodes, but 
forgetting to set the relevant type information for those nodes. 
Other than that, the only hiccup there was that passing the 
dimension of the vector was shifted by 1, i.e a vec2 would pass 
1, vec3 pass 2 and so on, which gave us strange errors at one 
point.

1.3 Novelties
There is little to point to as novelties in this section, as 
it's purpose was basic.


2. Define tree node structure and print AST
2.1 Approach
For defining tree node, I followed paradigm of the existing node. 
Each node kind will occupy a struct in the union of the tree node.
For printing AST, I designed a generic tree traversal function. 
The function will take function pointers for pre-traverse and post-
traverse. So these functions will handle the tree node when it is 
first entering the node and when it is leaving the node. This 
function is very useful as many of the later tasks will involve tree 
traversal. Then I wrote two functions for pre-traverse and 
post-traverse for tree printing. Most of the printing is in the 
pre-traverse and post-traverse is for closing up the bracket. In each 
of the functions, I simply have a switch-case statement differentiating 
different node type.

2.2 Challenges
One challenge is to define the type of each node. Since only expression 
node have type, first we put a type field in the structure of an 
expression type. We found that it was too hard to check for type for 
every type of expression, so we decided to put type into the node 
structure directly.
A tricky part is to check whether the children is null before traversing 
into the children. Sometimes the parents node may have optional child so 
it is important to check those as well. Pre/post traverse functions may 
also be passed in null pointer.

2.3 Novelties
For making the output easier to read, I make the printing output of the 
tree have indent. Some larger tree node will have a new line and 
indented with proper number of spaces. It is tricky to define the 
printing rule for each node because sometimes a lot of new lines is 
obstructing the vision. So I designed smarter indent and new line rules 
for different node kind. And the pre/post traverse function pointers 
really help make each part of the code independent and modular.


3. Semantic Checking
3.1 Approach
As with the other section, the semantic check function involved 
checking what kind of node you are at and using a switch statement 
to handle it. Unlike other things I worked on previously for this 
lab, this function was actually very involved and not alwayes straight 
forward. For one thing, one must first note that the actual overall 
semantic check requires two functions. One that is invoked in the 
pre-post order and the other in post-order, both of these executing 
during one transversal. The pre-post function ast_scope_generator() 
is simple in that it only detects when we are about to enther a new 
scope, and calls a function that pushes that one on a stack and prepares 
for a new symbol table to be formed for just that scope. This way we 
keep all the symbols we have entered so far. The other much more 
involved function, ast_sementic_check(), would be the antithesis for 
those scopes, as everytime this post-order encounter a scope, it would 
destroy that scope's symbol table and scope itself. I could describe 
every node and its function, but essentially it does what the manual 
asks for. 

3.2 Challenges
It was a challange making all the "expression" type nodes inherit 
their types from their children, or setting them as "invisisble" with 
-1. Invisible types were ignored during type checking. This way some 
errors didnt propagate uselessly. Dealing with predefined varibles 
during assignments or variable references was a also quite difficult. 

3.3 Novelties
If we had time we had more time we could've perhaps implemented the 
constant propagation that was mentioned. We did however implement 
the initialization tracking bonus. 


4. Symbol Table
4.1 Approach
The way we implemented the symbol table is a linked list of a linked 
list. Each symbol table entry represents a variable. It has a child 
and a sibling. The last variable of current scope has a child that 
points to the first variable of next scope. The variables have 
sibling points to the next variable in the same scope. This is a 
succint way of defining symbol table with only two pointers. And 
adding variable, entering scope, and exiting scope are very straight 
forward. The only thing is that we did not use hashtable so the 
performance of the searching is O(n). But for project like this, 
this implementation is fast enough.

4.2 Challenges
One challenge is that it is hard to define the starting symbol entry. 
So to solve the problem I added an empty entry in each of the new 
scope as the header entry. And when doing string compare later when 
you want to find the symbol, it is important to skip the header 
entry, otherwise you will end up comparing a null string which 
leads to segmentation fault.

4.3 Novelties
I think the biggest novelty is the compactness. Our implementation 
of the symbol table only takes 80 lines, and it is fully functional. 
Also, for adding a new variable, we defined a struct storing all the 
variable information. The caller of the symbol table can simply 
passing in a variable struct by value, and the symbol table maintains 
the malloc and free internally. This implementation avoids unneeded 
pointer ownership transfer.
