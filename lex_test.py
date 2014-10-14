from subprocess import call

ict={
,'myToken1':258
,'myToken2':259
,'UNKNOWN':260
,'ID':261
,'LBRAC':262
,'RBRAC':263
,'LSBRAC':264
,'RSBRAC':265
,'LSCOPE':266
,'RSCOPE':267
,'COMMA':268
,'SEMICOLON':269
,'PLUS':270
,'MINUS':271
,'MUL':272
,'DIV':273
,'POWER':274
,'ASSIGN':275
,'NOT':276
,'AND':277
,'OR':278
,'EQ':279
,'NEQ':280
,'LT':281
,'LE':282
,'GT':283
,'GE':284
,'BOOL_L':285
,'INT_L':286
,'FLOAT_L':287
,'CONST':288
,'IF':289
,'ELSE':290
,'WHILE':291
,'VOID_T':292
,'INT_T':293
,'BOOL_T':294
,'FLOAT_T':295
,'VEC2_T':296
,'VEC3_T':297
,'VEC4_T':298
,'BVEC2_T':299
,'BVEC3_T':300
,'BVEC4_T':301
,'IVEC2_T':302
,'IVEC3_T':303
,'IVEC4_T':304
,'DP3_F':305
,'LIT_F':306
,'RSQ_F':307}

input_filename = "lex_test.in"
trace_filename = "lex_test.tr"

def main
	call(["compiler467", "-I", input_filename, "-R", trace_filename])
	f = open(trace_file, "r");
	for line in f