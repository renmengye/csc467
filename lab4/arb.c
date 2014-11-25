

void free_result() {
	instr *cur;
	while (cur = result) {
		free(result);
		result = cur->next;
	}
}

void free_cond() {
	cond *cur;
	while (cur = cond) {
		free(cond);
		cond = cur->next;
	}
}

const char *get_op_str(op_kind k) {
	switch k {
		case ABS: return "ABS";
		case ADD: return "ADD";
		case COS: return "COS";
		case CMP: return "CMP";
		case DP3: return "DP3";
		case DP4: return "DP4";
		case DPH: return "DPH";
		case FLR: return "FLR";
		case FRC: return "FRC";
		case KIL: return "KIL";
		case LG2: return "LG2";
		case LIT: return "LIT";
		case LRP: return "LRP";
		case MAD: return "MAD";
		case MAX: return "MAX";
		case MIN: return "MIN";
		case MOV: return "MOV";
		case MUL: return "MUL";
		case POW: return "POW";
		case RCP: return "RCP";
		case RSQ: return "RSQ";
		case SCS: return "SCS";
		case SGE: return "SGE";
		case SIN: return "SIN";
		case SLT: return "SLT";
		case SUB: return "SUB";
		case SWZ: return "SWZ";
		case TEX: return "TEX";
		case TXB: return "TXB";
		case TXP: return "TXP";
		case XPD: return "XPD";
		default: return "UNK";
	}
}

char *generate_code(instr *inst) {
	char code[100];
	if (instr->out && instr->in1 && instr->in2 && instr->in3)
		snprintf(code, 100,
			"%s %s, %s, %s, %s", 
			get_op_str(inst->op),
			inst->in1,
			inst->in2,
			inst->in3,
			inst->out);
	if (instr->out && instr->in1 && instr->in2)
		snprintf(code, 100,
			"%s %s, %s, %s", 
			get_op_str(inst->op),
			inst->in1,
			inst->in2,
			inst->out);
	if (instr->out && instr->in1)
		snprintf(code, 100,
			"%s %s, %s", 
			get_op_str(inst->op),
			inst->in1,
			inst->out);
	return code;
}

instr *generate(node *ast) {
	free_result();
	ast_traverse(ast, 0, NULL, &generate_post);
	return result;
}

void generate_post(node *ast, int level) {

}