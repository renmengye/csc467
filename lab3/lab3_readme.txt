Lab 3: AST Construction and Semantic Checking

 Mengye Ren           998905430
 Viktor Riabtsev      997544420

In the lab we did the following components:
	1. Construction of AST using bison grammar.
	2. Define tree node structure and print AST.
	3. Semantic Checking.
	4. Symbol Table.

Viktor Riabtsev did part (1) and (3). Mengye Ren did part (2) and (4).
We summarize the approach, challenges, and novelties in the following.

1. Construction of AST
1.1 Approach
1.2 Challenges
1.3 Novelties


2. Define tree node structure and print AST
2.1 Approach
For defining tree node, I followed paradigm of the existing node. Each node kind will occupy a struct in the union of the tree node.
For printing AST, I designed a generic tree traversal function. The function will take function pointers for pre-traverse and post-traverse. So these functions will handle the tree node when it is first entering the node and when it is leaving the node. This function is very useful as many of the later tasks will involve tree traversal. Then I wrote two functions for pre-traverse and post-traverse for tree printing. Most of the printing is in the pre-traverse and post-traverse is for closing up the bracket. In each of the functions, I simply have a switch-case statement differentiating different node type.

2.2 Challenges
One challenge is to define the type of each node. Since only expression node have type, first we put a type field in the structure of an expression type. We found that it was too hard to check for type for every type of expression, so we decided to put type into the node structure directly.
A tricky part is to check whether the children is null before traversing into the children. Sometimes the parents node may have optional child so it is important to check those as well. Pre and post traverse function may also be passed in null pointer.

2.3 Novelties
For making the output easier to read, I make the printing output of the tree have indent. Some larger tree node will have a new line and indented with proper number of spaces.


3. Semantic Checking
3.1 Approach
3.2 Challenges
3.3 Novelties


4. Symbol Table
4.1 Approach
The way we implemented the symbol table is a linked list of a linked list. Each symbol table entry represents a variable. It has a child and a sibling. The last variable of current scope has a child that points to the first variable of next scope. The variables have sibling points to the next variable in the same scope. This is a succint way of defining symbol table with only two pointers. And adding variable, entering scope, and exiting scope is very straight forward. The only thing is that we did not use hash table so the performance of the searching is O(n)
 but for project like this, this implementation is fast enough.

4.2 Challenges


4.3 Novelties