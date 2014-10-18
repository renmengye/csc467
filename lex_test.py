import sys
from subprocess import call

dict={
'myToken1':258
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
,'RSQ_F':307
,'GL_FRAGCOLOR_V':308
,'GL_FRAGDEPTH_V':309
,'GL_FRAGCOORD_V':310
,'GL_TEXCOORD_V':311
,'GL_COLOR_V':312
,'GL_SECONDARY_V':313
,'GL_FOGFRAGCOORD_V':314
,'GL_LIGHT_HALF_V':315
,'GL_LIGHT_AMBIENT_V':316
,'GL_MATERIAL_SHININESS_V':317
,'ENV1_V':318
,'ENV2_V':319
,'ENV3_V':320
}


def main(argv):
	global dict;
	input_filename = "lex_test.in"
	expect_filename = "lex_test.ex"
	trace_filename = "lex_test.tr"
	call(["compiler467", "-Tn", input_filename, "-R", trace_filename])
	f = open(trace_filename, "r")
	trace_data = f.read()
	trace_data_list = trace_data.split('\n')
	f.close()
	f = open(expect_filename, "r")
	index = 0;
	num_pass = 0;
	num_error = 0;
	for line in f:
		tokens = line.replace('\n','').split(',')
		for token in tokens:
			if ("%d" % dict[token]) in trace_data_list[index]:
				print "pass: %d: %s" % (index, trace_data_list[index])
				num_pass += 1
			else:
				print "error: %d: %s: expected: %s, %d" % (index, trace_data_list[index], token, dict[token])
				num_error += 1
			index += 1
	print "%d tests ran. %d passed. %d failed." % (num_pass + num_error, num_pass, num_error)
	pass

if __name__ == "__main__":
		main(sys.argv)
