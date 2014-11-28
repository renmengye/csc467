#define ZERO "{0,0,0,0}"
#define ONE "{1,1,1,1}"
#include "arb.h"

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

void enter_if_cond(char *condition_var) {
    // true is 1,1,1,1
    // false is 0,0,0,0

    // if => 0,0,0,0 - condition
    // else => condition - 1,1,1,1

    // multiply previous cond name and 0- to get nested condition
    struct _cond *new_cond = malloc(sizeof(struct _cond));
    char *if_name = get_tmp_reg();

    char *nested_condvar;
    if (cur_cond && cur_cond->next) {
        nested_condvar = get_tmp_reg();
        add_instr(DECLARATION,0,nested_condvar,0,0,0);
        add_instr(OPERATION,
            MUL,
            nested_condvar,
            condition_var,
            cur_cond->next->is_in_else ? cur_cond->next->else_name : cur_cond->next->if_name);
        add_instr(OPERATION,
            SUB,
            nested_condvar,
            ZERO,
            nested_condvar
            );
    } else {
        nested_condvar = condition_var;
    }
    add_instr(DECLARATION,0,if_name,0,0,0);
    add_instr(OPERATION,
        SUB,
        if_name,
        ZERO,
        nested_condvar);
    new_cond->if_name = if_name;
    new_cond->is_in_else = 0;

    char *else_name = get_tmp_reg();
    add_instr(DECLARATION,0,else_name,0,0,0);
    add_instr(OPERATION,
        SUB,
        else_name,
        nested_condvar,
        ONE);
    new_cond->else_name = cond;
    new_cond->next = cur_cond;
    cur_cond = new_cond;
}

void enter_else_cond() {
    cur_cond->is_in_else = 1;
}

void exit_cond() {
    if(cur_cond) {
        struct _cond *tmp = cur_cond->next;
        free(cur_cond);
        cur_cond = tmp;
    }
}

char* get_cond() {
    if(cur_cond->is_in_else) {
        return cur_cond->else_name;
    } else {
        return cur_cond->if_name;
    }
}

// in order traversal for if-else.
void generate_in_1(node *cur, int level) {
    switch(cur->kind) {
    case IF_STATEMENT_NODE:
        char *cond_var = cur->if_stmt.condition_expr->tmp_var_name;
        enter_if_cond(cond_var);
        break;
    }
}

// in order traversal for if-else.
void generate_in_2(node *cur, int level) {
    switch(cur->kind) {
    case IF_STATEMENT_NODE:
        enter_else_cond();
        break;
    }
}

void generate_post(node *cur, int level) {
    switch(cur->kind) {
    case SCOPE_NODE:
        break;
    case UNARY_EXPRESION_NODE:
        switch(cur->unary_expr.op) {
            case UMINUS:
            case '!':
        }
        break;
    case BINARY_EXPRESSION_NODE:
        char *tmp = get_tmp_reg();
        add_instr(DECLARATION,0,tmp,0,0,0);
        cur->tmp_var_name = tmp;
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
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
            case '/':
                add_instr(
                    OPERATION,
                    RSP,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    0,0);
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    tmp
                    0);
                break;
            case '^':
                add_instr(
                    OPERATION,
                    POW,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
            case '<':
                // x-y < 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ONE,
                    ZERO);
                break;
            case LEQ:
                // y-x >= 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ZERO,
                    ONE);
                break;
            case '>':
                // y-x < 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ONE,
                    ZERO);
                break;
            case GEQ:
                // x-y >= 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ZERO,
                    ONE);
            case EQ:
                // (x-y >= 0) * (y-x >= 0)
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                char *tmp2 = get_tmp_reg();
                add_instr(DECLARATION,0,tmp2,0,0,0);
                add_instr(
                    OPERATION,
                    SUB,
                    tmp2,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ZERO,
                    ONE);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp2,
                    tmp2,
                    ZERO,
                    ONE);
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    tmp,
                    tmp2,
                    0);
                break;
            case NEQ:
                // (x-y < 0) + (y-x < 0)
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                char *tmp2 = get_tmp_reg();
                add_instr(DECLARATION,0,tmp2,0,0,0);
                add_instr(
                    OPERATION,
                    SUB,
                    tmp2,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ONE,
                    ZERO);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp2,
                    tmp2,
                    ONE,
                    ZERO);
                add_instr(
                    OPERATION,
                    ADD,
                    tmp,
                    tmp,
                    tmp2,
                    0);
                break;
            case AND:
                // x*y
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                break;
            case OR:
                //abs(0-x+y)
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    ZERO,
                    cur->binary_expr.left->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    ADD,
                    tmp,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    0);
                add_instr(
                    OPERATION,
                    ABS,
                    tmp,
                    tmp
                    0);
                break;

        }
        break;
    case INT_NODE:
        char *tmp = get_tmp_reg();
        add_instr(
            DECLARATION,0,tmp,0,0,0);
        char *lit = calloc(100, sizeof(char));
        snprintf(lit, 100, 
            "{%d,%d,%d,%d}", 
            cur->int_val, 
            cur->int_val,
            cur->int_val,
            cur->int_val);
        add_instr(
            OPERATION,
            MOV,
            tmp,
            lit,0,0);
        cur->tmp_var_name = tmp;
        break;
    case FLOAT_NODE:
        char *tmp = get_tmp_reg();
        add_instr(
            DECLARATION,0,tmp,0,0,0);
        char *lit = calloc(100, sizeof(char));
        snprintf(lit, 100, 
            "{%f,%f,%f,%f}", 
            cur->float_val, 
            cur->float_val,
            cur->float_val,
            cur->float_val);
        add_instr(
            OPERATION,
            MOV,
            tmp,
            lit,0,0);
        cur->tmp_var_name = tmp;
        break;
    case TYPE_NODE:
        /* Do nothing */
        break;
    case BOOL_NODE:
        char *tmp = get_tmp_reg();
        add_instr(
            DECLARATION,0,tmp,0,0,0);
        char *lit = calloc(100, sizeof(char));
        int val = cur->bool_val?1:0;
        snprintf(lit, 100, 
            "{%d,%d,%d,%d}", 
            val, 
            val,
            val,
            val);
        add_instr(
            OPERATION,
            MOV,
            tmp,
            lit,0,0);
        cur->tmp_var_name = tmp;
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
        add_instr(DECLARATION,0,tmp,0,0,0);
        cur->tmp_var_name = tmp;
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
        char *tmp = get_tmp_reg();
        add_instr(
            DECLARATION,0,tmp,0,0,0);
        char *lit = calloc(100, sizeof(char));
        snprintf(lit, 100, 
            "{%s}", cur->ctor.args->tmp_var_name);
        add_instr(
            OPERATION,
            MOV,
            tmp,
            lit,0,0);
        cur->tmp_var_name = tmp;
        break;
    case ARGUMENTS_NODE:
        // need to store all temporary registers into arguments temp var name.
        char* tmp = calloc(100, sizeof(char));
        if(cur->args.args) {
            snprintf(tmp, 100, '%s, %s', cur->args.args->tmp_var_name, cur->args.expr->tmp_var_name);
        }
        else {
            snprintf(tmp, 100, '%s', cur->args.expr->tmp_var_name);
        }
        cur->tmp_var_name = tmp;
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
            // Assign variable to itself if the condition is not met.
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
        /* Do nothing */
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