Lab 1: Scanner

Mengye Ren
Victor Riabtssev

Question: How did we deal with any special problems encourntered?

Answer: There are some problems and we came to practical solutions.

First, we need to skip all the comment sections. This is implemented with 
a loop that skips all the characters starting with /* until reaching */.

Second, we found that, when scanning an integer,  it will be ambiguous to 
include the negative sign in the front as part of the integer, as it may 
be confused with the binary minus operation. Therefore we decided to not 
include the negative sign and have this job completed in the parser. 
Because it is easier to define grammatical rules of the unary operator 
'-'.

Third, we found a bug in our code that the identifier and integer 
scanning does not look ahead a delimiter. For example, 222xxx was 
tokenized into two pieces: integer 222 and identifier xxx. This is 
obviously wrong. In fact, we should aggregated all the characters to an 
identifier until we look ahead a non-alpha numerical character.

Lastly, we wrote a python script "lex_test.py" to unit test our scanner. 
This is a good way to have our code maintainable. The test input file is 
"lex_test.in" and the expectation file is "lex_test.ex".
