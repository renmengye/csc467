#define ZERO "{0.0, 0.0, 0.0, 0.0}"
#define ONE "{1.0, 1.0, 1.0, 1.0}"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "arb.h"
#include "parser.tab.h"
#include "semantic.h"
#include "symbol.h"
#include "reg_conserve.h"

int is_in_ctor = 0;

struct _cond {
    char *if_name;
    char *else_name;
    int is_in_else;
    struct _cond *next;
};

struct _cond *cur_cond;
instr *result;
instr *head;
int temp_reg_counter = 0;


void free_result() {
    instr *p = head;
    while (p) {
        instr *tmp = p;
        free(p);
        p = tmp->next;
    }
    head = NULL;
    temp_reg_counter = 0;
}

void free_cond() {
    struct _cond *p;
    while (p = cur_cond) {
        cur_cond = cur_cond->next;
        free(p);
    }
}

const char *get_op_str(op_kind k) {
    switch (k) {
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
	char *code = (char *)calloc(100, sizeof(char));
    if(inst->kind == OPERATION) {
        if (inst->out != NULL && inst->in1 != NULL  && inst->in2 != NULL  && inst->in3 != NULL) {
            snprintf(code, 100,
                "%s %s, %s, %s, %s;", 
                get_op_str(inst->op),
                inst->out,
                inst->in1,
                inst->in2,
                inst->in3);
        } else if (inst->out != NULL  && inst->in1 != NULL  && inst->in2 != NULL ) {
            snprintf(code, 100,
                "%s %s, %s, %s;", 
                get_op_str(inst->op),
                inst->out,
                inst->in1,
                inst->in2);
        } else if (inst->out != NULL  && inst->in1 != NULL ) {
            snprintf(code, 100,
                "%s %s, %s;", 
                get_op_str(inst->op),
                inst->out,
                inst->in1);
        }
    } else if (inst->kind == DECLARATION) {
        snprintf(code, 100, "TEMP %s;", inst->out);
    }
	return code;
}

void add_instr(
    instr_kind kind,
    op_kind op, 
    char *out, 
    char *in1, 
    char *in2,
    char *in3) {
    instr *inst = (instr *)malloc(sizeof(instr));
    inst->kind = kind;
    inst->in1 = in1;
    inst->in2 = in2;
    inst->in3 = in3;
    inst->out = out;
    inst->op = op;

    if(result) {
        result->next = inst;
    } else {
        head = inst;
    }
    result = inst;
}

char *get_tmp_reg() {
    char *tmp = (char *)calloc(10, sizeof(char));
    snprintf(tmp, 10, "tempVar%d", temp_reg_counter++);
    return tmp;
}

const char *get_index(int i){
    if(i==0)
        return "x";
    if(i==1)
        return "y";
    if(i==2)
        return "z";
    if(i==3)
        return "w";
    return "unknown";
}

char *get_var_reg(char *id) {
    symbol_table_entry *entry = symbol_find(id);
    if(entry){
        char *var_reg = (char *) calloc(20, sizeof(char));
        snprintf(var_reg, 20, "_%s_%d", entry->id, entry->scope_id);
        return var_reg;
    }else {
        return NULL;
    }
}

void enter_if_cond(char *condition_var) {
    // true is 1,1,1,1
    // false is 0,0,0,0

    // if => 0,0,0,0 - condition
    // else => condition - 1,1,1,1
    // nested_if => prev_if_else * condition
    // nested_else => condition - nested_if
    struct _cond *new_cond = (struct _cond *)malloc(sizeof(struct _cond));

    if (cur_cond) {
        // nested
        char *if_name = get_tmp_reg();
        add_instr(DECLARATION,ADD,if_name,NULL,NULL,NULL);
        add_instr(OPERATION,
            MUL,
            if_name,
            condition_var,
            cur_cond->is_in_else ? cur_cond->else_name : cur_cond->if_name,
            NULL);
        new_cond->if_name = if_name;

        char *else_name = get_tmp_reg();
        add_instr(DECLARATION,ADD,else_name,NULL,NULL,NULL);
        add_instr(OPERATION,
            SUB,
            else_name,
            condition_var,
            if_name,
            NULL);
        new_cond->else_name = else_name;
    } else {
        // no nested
        char *if_name = get_tmp_reg();
        add_instr(DECLARATION,ADD,if_name,NULL,NULL,NULL);
        add_instr(OPERATION,
            SUB,
            if_name,
            ZERO,
            condition_var,
            NULL);
        new_cond->if_name = if_name;

        char *else_name = get_tmp_reg();
        add_instr(DECLARATION,ADD,else_name,NULL,NULL,NULL);
        add_instr(OPERATION,
            SUB,
            else_name,
            condition_var,
            ONE,
            NULL);
        new_cond->else_name = else_name;
    }
    new_cond->next = cur_cond;
    new_cond->is_in_else = 0;
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
    case IF_STATEMENT_NODE:{
        char *cond_var = cur->if_stmt.condition_expr->tmp_var_name;
        enter_if_cond(cond_var);
        break;}
    default:
        break;
    }
}

// in order traversal for if-else.
void generate_in_2(node *cur, int level) {
    switch(cur->kind) {
    case IF_STATEMENT_NODE:
        enter_else_cond();
        break;
    default:
        break;
    }
}

void generate_pre(node *cur, int level) {
    switch(cur->kind) {
    case SCOPE_NODE:
        scope_enter();
        break;
    case CONSTRUCTOR_NODE:
        is_in_ctor = 1;
        break;
    default:
        break;
    }
}

void generate_post(node *cur, int level) {
    switch(cur->kind) {
    case SCOPE_NODE:
        scope_exit();
        break;
    case UNARY_EXPRESION_NODE:{
        switch(cur->unary_expr.op) {
            case UMINUS:
            break;
            case '!':
            break;
            default:
            break;
        }
        break;}
    case BINARY_EXPRESSION_NODE:{
        char *tmp;
        tmp = get_tmp_reg();
        add_instr(DECLARATION,ADD,tmp,NULL,NULL,NULL);
        cur->tmp_var_name = tmp;
        switch(cur->binary_expr.op) {
            case '+':{
                add_instr(
                    OPERATION,
                    ADD,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                break;}
            case '-':{
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                break;}
            case '*':{
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                break;}
            case '/':{
                add_instr(
                    OPERATION,
                    RCP,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    NULL,NULL);
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    tmp,
                    NULL);
                if(cur->binary_expr.right->kind == INT_NODE) {
                    add_instr(
                        OPERATION,
                        FLR,
                        tmp,
                        tmp,
                        NULL,
                        NULL);
                }
                break;}
            case '^':{
                add_instr(
                    OPERATION,
                    POW,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                break;}
            case '<':{
                // x-y < 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ONE,
                    ZERO);
                break;}
            case LEQ:{
                // y-x >= 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    NULL);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ZERO,
                    ONE);
                break;}
            case '>':{
                // y-x < 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    NULL);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ONE,
                    ZERO);
                break;}
            case GEQ:{
                // x-y >= 0
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                add_instr(
                    OPERATION,
                    CMP,
                    tmp,
                    tmp,
                    ZERO,
                    ONE);}
            case EQ:{
                // (x-y >= 0) * (y-x >= 0)
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                char *tmp2;
                tmp2 = get_tmp_reg();
                add_instr(DECLARATION,ADD,tmp2,NULL,NULL,NULL);
                add_instr(
                    OPERATION,
                    SUB,
                    tmp2,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    NULL);
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
                    NULL);
                break;}
            case NEQ:{
                // (x-y < 0) + (y-x < 0)
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                char *tmp2;
                tmp2 = get_tmp_reg();
                add_instr(DECLARATION,ADD,tmp2,NULL,NULL,NULL);
                add_instr(
                    OPERATION,
                    SUB,
                    tmp2,
                    cur->binary_expr.right->tmp_var_name,
                    cur->binary_expr.left->tmp_var_name,
                    NULL);
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
                    NULL);
                break;}
            case AND:{
                // x*y
                add_instr(
                    OPERATION,
                    MUL,
                    tmp,
                    cur->binary_expr.left->tmp_var_name,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                break;}
            case OR:{
                //abs(0-x+y)
                add_instr(
                    OPERATION,
                    SUB,
                    tmp,
                    ZERO,
                    cur->binary_expr.left->tmp_var_name,
                    NULL);
                add_instr(
                    OPERATION,
                    ADD,
                    tmp,
                    tmp,
                    cur->binary_expr.right->tmp_var_name,
                    NULL);
                add_instr(
                    OPERATION,
                    ABS,
                    tmp,
                    tmp,
                    NULL,NULL);
                break;}

        }
        break;}
    case INT_NODE:{
        char *lit  = (char *) calloc(100, sizeof(char));
        if(is_in_ctor) {
            snprintf(lit, 100, 
                "%.1f", 
                cur->int_val);
           cur->tmp_var_name = lit;
        } else {
            char *tmp = get_tmp_reg();
            add_instr(DECLARATION,ADD,tmp,NULL,NULL,NULL);
            snprintf(lit, 100, 
                "{%.1f, %.1f, %.1f, %.1f}", 
                (float) cur->int_val, 
                (float) cur->int_val,
                (float) cur->int_val,
                (float) cur->int_val);
            add_instr(
                OPERATION,
                MOV,
                tmp,
                lit,NULL,NULL);
            cur->tmp_var_name = tmp;
        }
        break;}
    case FLOAT_NODE:{
        char *lit  = (char *) calloc(100, sizeof(char));
        if(is_in_ctor) {
            snprintf(lit, 100, 
                "%.1f", 
                cur->float_val);
           cur->tmp_var_name = lit;
        } else {
            char *tmp = get_tmp_reg();
            add_instr(DECLARATION,ADD,tmp,NULL,NULL,NULL);
            snprintf(lit, 100, 
                "{%.1f, %.1f, %.1f, %.1f}", 
                cur->float_val, 
                cur->float_val,
                cur->float_val,
                cur->float_val);
            add_instr(
                OPERATION,
                MOV,
                tmp,
                lit,NULL,NULL);
           cur->tmp_var_name = tmp;
        }
        break;}
    case TYPE_NODE:
        /* Do nothing */
        break;
    case BOOL_NODE:{
        int val = cur->bool_val?1:0;
        char *lit  = (char *) calloc(100, sizeof(char));
        if(is_in_ctor) {
            snprintf(lit, 100, 
                "%.1f", 
                val);
           cur->tmp_var_name = lit;
        } else {
            char *tmp = get_tmp_reg();
            add_instr(DECLARATION,ADD,tmp,NULL,NULL,NULL);
            snprintf(lit, 100, 
                "{%.1f, %.1f, %.1f, %.1f}", 
                (float) val, 
                (float) val,
                (float) val,
                (float) val);
            add_instr(
                OPERATION,
                MOV,
                tmp,
                lit,NULL,NULL);
           cur->tmp_var_name = tmp;
        }
        break;}
    case EXP_VAR_NODE:
        cur->tmp_var_name = cur->exp_var_node.var_node->tmp_var_name;
        break;
    case VAR_NODE:{
        // Omitting scope new variable edge cases.
        if (!cur->var_node.id){
            cur->tmp_var_name = "";
            break;
        }
        char *id = cur->var_node.id;
        if (strcmp(id, "gl_FragColor") == 0) {
            id = "result.color";
        } else if (strcmp(id, "gl_FragDepth") == 0) {
            id = "result.depth";
        } else if (strcmp(id, "gl_FragCoord") == 0) {
            id = "fragment.position";
        } else if (strcmp(id, "gl_TexCoord") == 0) {
            id = "fragment.texcoord";
        } else if (strcmp(id, "gl_Color") == 0) {
            id = "fragment.color";
        } else if (strcmp(id, "gl_Secondary") == 0) {
            id = "fragment.color.secondary";
        } else if (strcmp(id, "gl_FogFragCoord") == 0) {
            id = "fragment.fogcoord";
        } else if (strcmp(id, "gl_Light_Half") == 0) {
            id = "state.light[0].half";
        } else if (strcmp(id, "gl_Light_Ambient") == 0) {
            id = "state.lightmodel.ambient";
        } else if (strcmp(id, "gl_Material_Shininess") == 0) {
            id = "state.material.shininess";
        } else if (strcmp(id, "env1") == 0) {
            id = "program.env[1]";
        } else if (strcmp(id, "env2") == 0) {
            id = "program.env[2]";
        } else if (strcmp(id, "env3") == 0) {
            id = "program.env[3]";
        } else {
            id = get_var_reg(cur->var_node.id);
        }

        if (cur->var_node.is_array) {
            char *tmp;
            tmp = (char *)calloc(100, sizeof(char));
            snprintf(tmp, 100, "%s.%s", id, get_index(cur->var_node.index));
            cur->tmp_var_name = tmp;
        } else {
            cur->tmp_var_name = id;
        }
        break;}
    case FUNCTION_NODE:{
        char *tmp;
        tmp = get_tmp_reg();
        add_instr(DECLARATION,ADD,tmp,NULL,NULL,NULL);
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
        break;}
    case CONSTRUCTOR_NODE:{
        char *tmp;
        tmp = get_tmp_reg();
        add_instr(
            DECLARATION,ADD,tmp,NULL,NULL,NULL);
        if (cur->ctor.args) {
            char *lit;
            lit = (char *)calloc(100, sizeof(char));
            snprintf(lit, 100, 
                "{%s}", cur->ctor.args->tmp_var_name);
            add_instr(
                OPERATION,
                MOV,
                tmp,
                lit,NULL,NULL);
            cur->tmp_var_name = tmp;
        }
        is_in_ctor = 0;

        break;}
    case ARGUMENTS_NODE:{
        // need to store all temporary registers into arguments temp var name.
        char* tmp = (char *)calloc(100, sizeof(char));

        if(cur->args.args != NULL && cur->args.expr != NULL) {
            snprintf(tmp, 100, "%s, %s", cur->args.args->tmp_var_name, cur->args.expr->tmp_var_name);
        } else if (cur->args.args) {
            snprintf(tmp, 100, "%s", cur->args.args->tmp_var_name);
        } else if (cur->args.expr) {
            snprintf(tmp, 100, "%s", cur->args.expr->tmp_var_name);
        } else {
            tmp = "";
        }
        cur->tmp_var_name = tmp;
        break;}
    case STATEMENTS_NODE:
        /*Do nothing*/
        break;
    case IF_STATEMENT_NODE:
        exit_cond();
        break;
    case ASSIGNMENT_NODE:{
        // If not in conditional block.
        if (cur_cond) {
            // Assign variable to itself if the condition is not met.
            add_instr(
                OPERATION,
                CMP,
                get_cond(),
                cur->assignment.variable->tmp_var_name,
                cur->assignment.expr->tmp_var_name,
                cur->assignment.variable->tmp_var_name
                );
        }
        else {
            add_instr(
                OPERATION,
                MOV,
                cur->assignment.variable->tmp_var_name,
                cur->assignment.expr->tmp_var_name,
                NULL,NULL);
        }
        break;}
    case NESTED_SCOPE_NODE:
        //scope_exit();
        break;
    case DECLARATION_NODE: {
        symbol_table_entry entry;
        entry.id = cur->declaration.id;
        symbol_add(entry);
        char *reg = get_var_reg(cur->declaration.id);
        add_instr(
           DECLARATION,
           ADD,
           reg,
           NULL,
           NULL,
           NULL);
        if (cur->declaration.expr) {
            add_instr(
                OPERATION,
                MOV,
                reg,
                cur->declaration.expr->tmp_var_name,
                NULL,
                NULL);
        }
        break;}
    case DECLARATIONS_NODE:
        /* Do nothing */
        break;
    default:
        /* Do nothing */
        break;
    }
}

instr *generate(node *ast) {
    free_result();
    symbol_reset();
    ast_traverse(ast, 0, &generate_pre, &generate_post, &generate_in_1, &generate_in_2);
    //conserve_reg(head);

    return head;
}

void print_result() {
    instr *cur = head;
    while(cur) {
        printf("%s\n", get_instr_str(cur));
        cur = cur->next;
    }
}
