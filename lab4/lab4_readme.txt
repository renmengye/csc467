Lab 4: Code Generation

 Mengye Ren           998905430
 Viktor Riabtsev      997544420

General implementation questions:
-How did you implement non-trivial math operations

-How did you handle boolean types.
We used {1, 1, 1, 1} for simple bool. And for vector bools we'd use 1 where it counted, and 0 elsewhere.

ex: vbvec2 a = vec(true, false);
TEMP a;
TEMP tempVar0;
MOV tempVar0, {1, 0, 0, 0};
MOV a, tempVar0;

-How did you implement if statements.
We would calculate the condition into a temporary register, subtract one from it (in order for CMP to work).
And keep the opposite bool value in a second temporary register, also subtracted 1.

The we simply execute both if and else code sections, except everytime that and extre if block appears, it depends on a cumilative condition, and its own temporary registers.

As assignment operations are encountered, they are replaced with CMP instructions depeneding on the condition.

-How did you implement constants.
Well. Since the semantic check gurantees that contants are infact treated as constants, we avoided the PARAM declaration and instead made all registers be of type TEMP.

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
