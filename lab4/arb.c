void free_result() {
    instr *cur;
    while (cur = result) {
        free(result);
        result = cur->next;
    }
    temp_reg_counter = 0;
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

char *get_instr_str(instr *inst) {
	char code[100];
    if(inst->kind == OPERATION) {
        if (instr->out && instr->in1 && instr->in2 && instr->in3)
            snprintf(code, 100,
                "%s %s, %s, %s, %s;", 
                get_op_str(inst->op),
                inst->out,
                inst->in1,
                inst->in2,
                inst->in3);
        if (instr->out && instr->in1 && instr->in2)
            snprintf(code, 100,
                "%s %s, %s, %s;", 
                get_op_str(inst->op),
                inst->out,
                inst->in1,
                inst->in2);
        if (instr->out && instr->in1)
            snprintf(code, 100,
                "%s %s, %s;", 
                get_op_str(inst->op),
                inst->out,
                inst->in1);
    } else if (inst->kind == DECLARATION) {
        snprintf(code, 100, "TEMP %s;", inst->out);
    }
	return code;
}

instr *generate(node *ast) {
    free_result();
    ast_traverse(ast, 0, NULL, &generate_post);
    return result;
}

void add_instr(
    instr_kind kind,
    op_kind op, 
    char *out, 
    char *in1, 
    char *in2,
    char *in3) {
    instr *inst = malloc(sizeof(instr));
    inst->in1 = in1;
    inst->in2 = in2;
    inst->in3 = in3;
    inst->out = out;
    inst->op = op;
}

char *get_tmp_reg() {
    char *tmp = calloc(10, sizeof(char));
    snprintf(tmp, 10, "tmp_%d", temp_reg_counter++);
}

void enter_if_cond();
void enter_else_cond();
void exit_cond();
char* get_cond();

// in order traversal for if-else.
void generate_in(node *cur, int level) {
}

void generate_post(node *cur, int level) {
    switch(cur->kind) {
    case IF_STATEMENT_NODE:
        char *cond_var = 
        enter_cond();
        break;
    }
}
void generate_post(node *cur, int level) {
    switch(cur->kind) {
    case SCOPE_NODE:
        break;
    case UNARY_EXPRESION_NODE:
        break;
    case BINARY_EXPRESSION_NODE:
        char *tmp = get_tmp_reg();
        add_instr(DECLARATION,0,tmp,0,0,0);
        switch(cur->binary_expr.op) {
            case '+':
                add_instr(
                    OPERATION,
                    ADD,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
            case '-':
                add_instr(
                    OPERATION,
                    MINUS,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
            case '*':
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
            case '/':
                add_instr(
                    OPERATION,
                    RSQ,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
                char *tmp2 = get_tmp_reg();
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    tmp
                    0);
            case '^':
                add_instr(
                    OPERATION,
                    POW,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
        }
        break;
    case INT_NODE:
        /* Do nothing */
        break;
    case FLOAT_NODE:
        /* Do nothing */
        break;
    case TYPE_NODE:
        /* Do nothing */
        break;
    case BOOL_NODE:
        /* Do nothing */
        break;
    case VAR_NODE:
        // Omitting scope new variable edge cases.
        if (cur->var_node.is_array) {
            char *tmp = calloc(20, sizeof(char));
            snprintf(tmp, 20, "%s[%d]", cur->var_node.id, cur->var_node.index);
            cur->tmp_var_name = tmp;
        } else {
            cur->tmp_var_name = cur->var_node.id;
        }
        break;
    case FUNCTION_NODE:
        char *tmp = get_tmp_reg();
        switch(cur->func.name) {
            case 0:
                add_instr(
                    OPERATION,
                    DP3,
                    tmp,
                    cur->func.args->tmp_var_name,
                    0,0);
                break;
            case 1:
                add_instr(
                    OPERATION,
                    RSQ,
                    tmp,
                    cur->func.args->tmp_var_name,
                    0,0);
                break;
            case 2:
                add_instr(
                    OPERATION,
                    LIT,
                    tmp,
                    cur->func.args->tmp_var_name,
                    0,0);
                break;
        }
        break;
    case CONSTRUCTOR_NODE:
        break;
    case ARGUMENTS_NODE:
        // need to store all temporary registers into arguments temp var name.
        char* var_name = calloc(100, sizeof(char));
        if(cur->args.args) {
            snprintf(cur->tmp_var_name, 100, '%s, %s', cur->args.args->tmp_var_name, cur->args.expr->tmp_var_name);
        }
        else {
            snprintf(cur->tmp_var_name, 100, '%s', cur->args.expr->tmp_var_name);
        }
        break;
    case STATEMENTS_NODE:
        /*Do nothing*/
        break;
    case IF_STATEMENT_NODE:
        enter_cond();
        break;
    case ASSIGNMENT_NODE:
        // If not in conditional block.
        if (cond) {
            add_instr(
                OPERATION,
                CMP,
                get_cond(),
                cur->assignment.variable->tmp_var_name,
                cur->assignment.expr->tmp_var_name,
                cur->assignment.variable->tmp_var_name,
                );
        }
        else {
            add_instr(
                OPERATION,
                MOV,
                cur->assignment.variable->tmp_var_name,
                cur->assignment.expr->tmp_var_name,
                0,0);
        }
        break;
    case NESTED_SCOPE_NODE:
        /* Do nothing */
        break;
    case DECLARATION_NODE:
        break;
    case DECLARATIONS_NODE:
        /* Do nothing */
        break;
    default:
        /* Do nothing */
        break;
    }
}

void print_result() {
    instr *cur = result;
    while(cur) {
        printf("%s\n", get_instr_str(cur));
        cur = cur->next;
    }
}