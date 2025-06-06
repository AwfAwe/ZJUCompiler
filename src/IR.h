#pragma once
#include "tree.h"
#include "code_gen.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

extern ofstream ofs;
extern ofstream IRofs;
extern targetCode targetCodes;

typedef class Addr_ *Addr;

enum AddrKind { LAB, VARIABLE, ARRAY, ARRAYNUM, ARRAYVAR, CONSTANT, STRING, EMPTY, VARIABLEINIT, ARRAYINIT, POINT, OP, BRACEEXP, POSTFIX };

static bool is_skip = false;

class Addr_ {
    static int temp;
    public:
        AddrKind kind;
        string name; //地址名称 
        int value;   //地址值 
        string idx;  //对于数组，存储其索引值 kind是ARRAYNUM表明用立即数作为下标，若是ARRAYVAR表明用临时变量作为下标
        float fvalue;
        char cvalue;
        src_type ty; //当其是立即数时，立即数的类型

        //数组
        Addr_(AddrKind kind, string name, int value) {
            this->kind = kind;
            this->name = name;
            this->value = value;
            // cout<<"type1:"<<kind<<name<<value<<endl;
        }
        Addr_(AddrKind kind, string name, string idx) {
            this->kind = kind;
            this->name = name;
            this->idx = idx;
            // cout<<"type1:"<<kind<<name<<value<<endl;
            ty =  arrayvar;
        }        

        Addr_(string name, int value) {
            this->name = name;
            this->value = value;
            // cout<<"type1:"<<kind<<name<<value<<endl;
            ty =  imm_int;
        }
        Addr_(string name, float fvalue) {
            this->name = name;
            this->fvalue = fvalue;
            ty =  imm_float;
            // cout<<"type1:"<<kind<<name<<value<<endl;
        }
        Addr_(string name, char cvalue) {
            this->name = name;
            this->cvalue = cvalue;
            ty =  imm_char;
            // cout<<"type1:"<<kind<<name<<value<<endl;
        }

        Addr_(string name) {
            this->name = name;
            ty = vart;
            // cout<<"type1:"<<kind<<name<<value<<endl;
        }
        Addr_(AddrKind kind, string name) {
            this->kind = kind;
            this->name = name;
            this->value = 0;
            ty = vart;
        }
        Addr_(AddrKind kind, int value) {
            this->kind = kind;
            this->value = value;
            this->name = "_none_";
            // cout<<"type2:"<<kind<<endl;

        }
        Addr_(AddrKind kind) {
            this->kind = kind;
            this->name = "_none_";
            this->value = 0;
            // cout<<"type3:"<<kind<<endl;
        }

        Addr_() {
            this->kind = VARIABLE;
            this->name = "temp_" + to_string((this->temp)++);
            this->value = 0;
            this->ty = vart;
        }
};

enum OpKind {
    ASSIGNMENT,
    ID,
    CONSTANTNUM,
    CONSTANTCHAR,
    CONSTANTFLOAT,
    BRACE,

    LOGICOR,
    LOGICAND,
    BITOR,
    BITXOR,
    BITAND,

    ADD,
    SUB,
    MUL,
    DIV,

    GOTO,
    LABEL,
    SEL,
    FUNC,
    FUNCBODY,
    FUNCEND,
    ARG,
    

    CALL,
    PARAM,
    EMPTYPARAM,
    POINTDEC,
    ADDR,
    VAL,
    READ,
    WRITE,
    RET,
    DECARRAY,
    DECVARIABLE,
    DECVARIABLEINIT,
    PREINC,
    PREDEC,
    POSTDEC,
    POSTINC,
    LE,
    LT,
    GT,
    GE,
    EQ,
    NE,
    LEFT,
    RIGHT,

    TOINT,
    TOCHAR,
    TOFLOAT,

    GETADDR,
    PTR,
    POS,
    NEG,
    BITNEG,
    NOT,

    VISITARRAY,
    EMPTYARG,
    ARGEND,

    // BREAK,
    // CONTINUE,
    INTINPUT,
    FLOATINPUT,
    CHARINPUT,
    INTOUTPUT,
    FLOATOUTPUT,
    CHAROUTPUT,
    SPACEOUTPUT,
    LINEOUTPUT
};

bool operator==(struct InterCode code1, struct InterCode code2);

//三地址码形式：四元组式
class InterCode {

    public:
        OpKind kind;
        Addr addr1, addr2, addr3;
        InterCode(OpKind opkind, Addr addr1, Addr addr2, Addr addr3) {
            this->kind = opkind;
            this->addr1 = addr1;
            this->addr2 = addr2;
            this->addr3 = addr3;
        }
        InterCode(OpKind opkind, Addr addr1, Addr addr2) {
            this->kind = opkind;
            this->addr1 = addr1;
            this->addr2 = addr2;
            this->addr3 = new Addr_(EMPTY);
        }
        InterCode(OpKind opkind, Addr addr1) {
            this->kind = opkind;
            this->addr1 = addr1;
            this->addr2 = new Addr_(EMPTY);
            this->addr3 = new Addr_(EMPTY);
        }
        InterCode(OpKind opkind) {
            this->kind = opkind;
            this->addr1 = new Addr_(EMPTY);
            this->addr2 = new Addr_(EMPTY);
            this->addr3 = new Addr_(EMPTY);
        }
        InterCode() {}
};

typedef struct InterCodes *IRNode;
typedef IRNode IRList;

//双向链表存储生成的三地址码
struct InterCodes {
    InterCode code;
    IRNode prev, next;
};

//实现对链表的操作
class IRCode {
    IRList IR_list;

    public:
        IRCode() {
            IR_list = new struct InterCodes;
            IR_list->prev = IR_list->next = NULL;
        }
        //插入到链表头
        void insert(InterCode code) {
            IRNode node = new struct InterCodes;
            node->code = code;
            node->prev = node->next = NULL;

            if (!IR_list->next) {
                IR_list->next = node;
                node->prev = IR_list;
            } else {
                IR_list->next->prev = node;
                node->next = IR_list->next;
                IR_list->next = node;
                node->prev = IR_list;
            }
        }
        IRNode search(InterCode code) {
            for (IRNode node = IR_list->next; node != NULL; node = node->next)
                if (node->code == code)
                    return node;
        }

        string str_addr(Addr addr) {
            return (addr->kind == CONSTANT) ? to_string(addr->value) : addr->name;
        }

        void show(IRNode node) {
            if (node == NULL)
                ;
            else {
                show(node->next);
                string IRcode;
                string temp = "";
                switch (node->code.kind) {
                    case ASSIGNMENT:
                        if(node->code.addr2->name == "PURE_ASSIGN"){
                            temp = "=";
                            targetCode.asm_assign("add", node->code.addr1->name, node->code.addr3->name, "zero", node->code.addr3->ty, src_type::vart);
                        }
                        if(node->code.addr2->name == "ADD_ASSIGN"){
                            temp = "+=";
                            targetCode.asm_assign("add", node->code.addr1->name, node->code.addr3->name, node->code.addr1->name, node->code.addr3->ty, src_type::vart);
                        }
                        if(node->code.addr2->name == "SUB_ASSIGN"){
                            temp = "-=";
                            targetCode.asm_assign("sub", node->code.addr1->name, node->code.addr3->name, node->code.addr1->name, node->code.addr3->ty, src_type::vart);
                        }
                        if(node->code.addr2->name == "MUL_ASSIGN"){
                            temp = "*=";
                            targetCode.asm_assign("mul", node->code.addr1->name, node->code.addr3->name, node->code.addr1->name, node->code.addr3->ty, src_type::vart);
                        }
                        if(node->code.addr2->name == "DIV_ASSIGN"){
                            temp = "'\'=";
                            targetCode.asm_assign("div", node->code.addr1->name, node->code.addr3->name, node->code.addr1->name, node->code.addr3->ty, src_type::vart);
                        }
                        if(node->code.addr2->name == "MOD_ASSIGN"){
                            temp = "%=";
                        }
                        if(node->code.addr2->name == "LEFT_ASSIGN"){
                            temp = "<<=";
                        }
                        if(node->code.addr2->name == "RIGHT_ASSIGN"){
                            temp = ">>=";
                        }
                        if(node->code.addr2->name == "AND_ASSIGN"){
                            temp = "&=";
                        }
                        if(node->code.addr2->name == "XOR_ASSIGN"){
                            temp = "^=";
                        }
                        if(node->code.addr2->name == "OR_ASSIGN"){
                            temp = "|=";
                        }
                        IRcode = node->code.addr1->name + temp + node->code.addr3->name;
                        // targetCode.asm_assign_add();

                        break;
                    case ID:
                        IRcode = str_addr(node->code.addr1);
                        break;
                    case CONSTANTNUM:
                        IRcode = str_addr(node->code.addr1);
                        break;
                    case CONSTANTFLOAT:
                        IRcode = str_addr(node->code.addr1);
                        break;
                    case CONSTANTCHAR:
                        IRcode = str_addr(node->code.addr1);
                        break;
                    case BRACE:
                        IRcode = "(" + str_addr(node->code.addr1) + ")";
                        break;
                    case PREINC:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                "++" + str_addr(node->code.addr2);
                        break;
                    case PREDEC:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                "--" + str_addr(node->code.addr2);
                        break;
                    case POSTINC:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + "++";
                        break;
                    case POSTDEC:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + "--";
                        break;

                    case GETADDR:
                        IRcode = str_addr(node->code.addr1) + " = " +
                        " GET_ADDRESS " + str_addr(node->code.addr2);
                        break;
                    case PTR:
                        IRcode = str_addr(node->code.addr1) + " = " +
                        " MEM " + str_addr(node->code.addr2);
                        break;
                    case POS:
                        IRcode = str_addr(node->code.addr1) + " = " +
                        " + " + str_addr(node->code.addr2);
                        break;
                    case NEG:
                        IRcode = str_addr(node->code.addr1) + " = " +
                        " - " + str_addr(node->code.addr2);
                        break;
                    case BITNEG:
                        IRcode = str_addr(node->code.addr1) + " = " +
                        " ~ " + str_addr(node->code.addr2);
                        break;
                    case NOT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                        " NOT " + str_addr(node->code.addr2);
                        break;
                    case LOGICAND:
                        IRcode = str_addr(node->code.addr1) + "=" + str_addr(node->code.addr2)
                         + "&&" + str_addr(node->code.addr3);
                        break;
                    case LOGICOR:
                        IRcode = str_addr(node->code.addr1) + "=" + str_addr(node->code.addr2)
                         + "||" + str_addr(node->code.addr3);
                        break;
                    case BITAND:
                        IRcode = str_addr(node->code.addr1) + "=" + str_addr(node->code.addr2)
                         + "&" + str_addr(node->code.addr3);
                        break;
                    case BITOR:
                        IRcode = str_addr(node->code.addr1) + "=" + str_addr(node->code.addr2)
                         + "|" + str_addr(node->code.addr3);
                        break;
                    case BITXOR:
                        IRcode = str_addr(node->code.addr1) + "=" + str_addr(node->code.addr2)
                         + "^" + str_addr(node->code.addr3);
                        break;
                    case TOINT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                "(int)" + str_addr(node->code.addr2);
                        // targetCodes.asm_assign_add(str_addr(node->code.addr1), 
                        //                            str_addr(node->code.addr2), 
                        //                            str_addr(node->code.addr3),
                        //                            (node->code.addr2->kind == CONSTANT),
                                                //    (node->code.addr3->kind == CONSTANT));
                        break;
                    case TOCHAR:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                "(char)" + str_addr(node->code.addr2);
                        // targetCodes.asm_assign_add(str_addr(node->code.addr1), 
                        //                            str_addr(node->code.addr2), 
                        //                            str_addr(node->code.addr3),
                        //                            (node->code.addr2->kind == CONSTANT),
                                                //    (node->code.addr3->kind == CONSTANT));
                        break; 
                    case TOFLOAT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                "(float)" + str_addr(node->code.addr2);
                        // targetCodes.asm_assign_add(str_addr(node->code.addr1), 
                        //                            str_addr(node->code.addr2), 
                        //                            str_addr(node->code.addr3),
                        //                            (node->code.addr2->kind == CONSTANT),
                                                //    (node->code.addr3->kind == CONSTANT));
                        break;
                    case ADD:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + " + " +
                                str_addr(node->code.addr3);
                        targetCodes.asm_assign_add(str_addr(node->code.addr1), 
                                                   str_addr(node->code.addr2), 
                                                   str_addr(node->code.addr3),
                                                   (node->code.addr2->kind == CONSTANT),
                                                   (node->code.addr3->kind == CONSTANT));
                        break;
                    case SUB:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + " - " +
                                str_addr(node->code.addr3);
                        targetCodes.asm_assign_sub(str_addr(node->code.addr1), 
                                                   str_addr(node->code.addr2), 
                                                   str_addr(node->code.addr3),
                                                   (node->code.addr2->kind == CONSTANT),
                                                   (node->code.addr3->kind == CONSTANT));
                        break;
                    case MUL:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + " * " +
                                str_addr(node->code.addr3);
                        targetCodes.asm_assign_mul(str_addr(node->code.addr1), 
                                                   str_addr(node->code.addr2), 
                                                   str_addr(node->code.addr3),
                                                   (node->code.addr2->kind == CONSTANT),
                                                   (node->code.addr3->kind == CONSTANT));
                        break;         
                    case DIV:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + " / " +
                                str_addr(node->code.addr3);
                        targetCodes.asm_assign_div(str_addr(node->code.addr1), 
                                                   str_addr(node->code.addr2), 
                                                   str_addr(node->code.addr3),
                                                   (node->code.addr2->kind == CONSTANT),
                                                   (node->code.addr3->kind == CONSTANT));
                        break;
                    case LEFT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + " << " +
                                str_addr(node->code.addr3);
                        targetCodes.asm_assign_div(str_addr(node->code.addr1), 
                                                   str_addr(node->code.addr2), 
                                                   str_addr(node->code.addr3),
                                                   (node->code.addr2->kind == CONSTANT),
                                                   (node->code.addr3->kind == CONSTANT));
                        break;
                    case RIGHT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                str_addr(node->code.addr2) + " >> " +
                                str_addr(node->code.addr3);
                        targetCodes.asm_assign_div(str_addr(node->code.addr1), 
                                                   str_addr(node->code.addr2), 
                                                   str_addr(node->code.addr3),
                                                   (node->code.addr2->kind == CONSTANT),
                                                   (node->code.addr3->kind == CONSTANT));
                        break;


                    case GOTO:
                        IRcode = "GOTO " + str_addr(node->code.addr1);
                        targetCodes.asm_goto_label(str_addr(node->code.addr1));
                        break;
                    case SEL:
                        IRcode = "IF " + str_addr(node->code.addr1) + 
                                " GOTO " + str_addr(node->code.addr2);
                        targetCodes.asm_branch_condition(str_addr(node->code.addr1),
                                                         str_addr(node->code.addr2));
                        break;
                    case LABEL:
                        IRcode = "LABEL " + str_addr(node->code.addr1);
                        targetCodes.asm_label(str_addr(node->code.addr1));	
                        break;
                    case FUNC:
                        IRcode = "\nFUNC " + str_addr(node->code.addr1) + " " + str_addr(node->code.addr2);
                        if(str_addr(node->code.addr1) == "input" || str_addr(node->code.addr1) == "output")
                        {
                            is_skip = true;
                            break;
                        }
                        is_skip = false;
                        targetCodes.asm_function(str_addr(node->code.addr1));
                        break;
                    case FUNCBODY:
                        IRcode = "BODY";
                        break;
                    case FUNCEND:
                        IRcode = str_addr(node->code.addr1)+" END";
                        break;
                    case EMPTYPARAM:
                        IRcode = "NO PARAM";
                        break;
                    case EMPTYARG:
                        IRcode = "NO ARGUMENT";
                        break;
                    case ARGEND:
                        IRcode = "ARGUMENT END";
                        break;
                    case ARG:
                        IRcode = "ARG " + str_addr(node->code.addr1);
                        targetCodes.asm_function_argument(str_addr(node->code.addr1));	
                        break;
                    case CALL:
                        IRcode = "CALL " + str_addr(node->code.addr1);
                        targetCodes.asm_function_call(str_addr(node->code.addr1));	
                        break;
                    case PARAM:
                        if (node->code.addr1->kind == ARRAY)
                            IRcode = "PARAM *" + str_addr(node->code.addr1);
                        else if(node->code.addr1->kind == VARIABLE)
                            IRcode = "PARAM " + str_addr(node->code.addr1) + " " + str_addr(node->code.addr2);
                        else if(node->code.addr1->kind == ARRAYINIT)
                            IRcode = "PARAM *" + str_addr(node->code.addr1) + '=' + str_addr(node->code.addr2);
                        else if(node->code.addr1->kind == VARIABLEINIT)
                            IRcode = "PARAM " + str_addr(node->code.addr1) + " " + str_addr(node->code.addr2) + '=' + str_addr(node->code.addr3);
                        if(is_skip == false)
                        {
                            targetCodes.asm_function_parameter(str_addr(node->code.addr1));
                        }
                        break;
                    case POINTDEC:
                        IRcode = "DEC *" + str_addr(node->code.addr1);
                        break;
                    case ADDR:
                        IRcode = str_addr(node->code.addr1) + " = &" + str_addr(node->code.addr2);
                        if(str_addr(node->code.addr3) == "0"){

                        targetCodes.asm_get_base_address(str_addr(node->code.addr1),
                                                         str_addr(node->code.addr2));			
                        }
                        if (str_addr(node->code.addr3) != "0") {
                            IRcode = IRcode + " + " + str_addr(node->code.addr3);
                            targetCodes.asm_get_offset_address(str_addr(node->code.addr1),
                                                               str_addr(node->code.addr2),
                                                               str_addr(node->code.addr3));
                        }
                        break;
                    case VAL:
                        IRcode = str_addr(node->code.addr1) + " = *" + str_addr(node->code.addr2);
                        targetCodes.asm_get_value(str_addr(node->code.addr1), 
                                                 str_addr(node->code.addr2));
                        break;
                    //case READ:
                    //    IRcode = "CALL read";
                    //    targetCodes.asm_call_read();
                    //    break;
                    //case WRITE:
                    //    IRcode = "CALL write";
                    //    targetCodes.asm_call_write();
                    //    break;
                    case RET:
                        if (node->code.addr1->kind != EMPTY) {
                            IRcode = "RET " + str_addr(node->code.addr1);
                            if(is_skip == false)
                            {
                                targetCodes.asm_return_value(str_addr(node->code.addr1));	
                            }
                        } else {
                            IRcode = "RET";	
                            if(is_skip == false)
                            {
                                targetCodes.asm_return_void();	
                            }															
                        }
                        break;
                    case DECARRAY:
                        IRcode = "DEC " + node->code.addr1->name + " " + node->code.addr2->name + "[" + to_string(node->code.addr3->value) + "]";
                        targetCodes.asm_declaration(str_addr(node->code.addr1));
                        break;
                    case DECVARIABLEINIT:
                        IRcode = "DEC " + node->code.addr1->name + " " + node->code.addr2->name + '=' + node->code.addr3->name;
                        break;
                    case DECVARIABLE:
                        IRcode = "DEC " + node->code.addr1->name + " " + node->code.addr2->name;
                        targetCodes.asm_declaration(str_addr(node->code.addr1));
                        break;
                    case LE:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + " <= " + 
                                 str_addr(node->code.addr3);
                        targetCodes.asm_set_less_equal(str_addr(node->code.addr1),
                                                       str_addr(node->code.addr2),
                                                       str_addr(node->code.addr3),
                                                       (node->code.addr2->kind == CONSTANT),
                                                       (node->code.addr3->kind == CONSTANT));
                        break;
                    case LT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + " < " +
                                 str_addr(node->code.addr3);
                        targetCodes.asm_set_less_than(str_addr(node->code.addr1),
                                                      str_addr(node->code.addr2),
                                                      str_addr(node->code.addr3),
                                                      (node->code.addr2->kind == CONSTANT),
                                                      (node->code.addr3->kind == CONSTANT));
                        break;
                    case GT:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + " > " +
                                 str_addr(node->code.addr3);
                        targetCodes.asm_set_greater_than(str_addr(node->code.addr1),
                                                         str_addr(node->code.addr2),
                                                         str_addr(node->code.addr3),
                                                         (node->code.addr2->kind == CONSTANT),
                                                         (node->code.addr3->kind == CONSTANT));
                        break;
                    case GE:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + " >= " + 
                                 str_addr(node->code.addr3);
                        targetCodes.asm_set_greater_equal(str_addr(node->code.addr1),
                                                          str_addr(node->code.addr2),
                                                          str_addr(node->code.addr3),
                                                          (node->code.addr2->kind == CONSTANT),
                                                          (node->code.addr3->kind == CONSTANT));
                        break;
                    case EQ:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + " == " + 
                                 str_addr(node->code.addr3);
                        targetCodes.asm_set_equal(str_addr(node->code.addr1),
                                                  str_addr(node->code.addr2),
                                                  str_addr(node->code.addr3),
                                                  (node->code.addr2->kind == CONSTANT),
                                                  (node->code.addr3->kind == CONSTANT));
                        break;
                    case NE:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + " != " + 
                                 str_addr(node->code.addr3);
                        targetCodes.asm_set_not_equal(str_addr(node->code.addr1),
                                                      str_addr(node->code.addr2),
                                                      str_addr(node->code.addr3),
                                                      (node->code.addr2->kind == CONSTANT),
                                                      (node->code.addr3->kind == CONSTANT));
                        break;
                    
                    case VISITARRAY:
                        IRcode = str_addr(node->code.addr1) + " = " +
                                 str_addr(node->code.addr2) + "[" + 
                                 str_addr(node->code.addr3) + "]";
                        targetCodes.asm_assign(str_addr(node->code.addr1),
                                                      str_addr(node->code.addr2),
                                                      str_addr(node->code.addr3),
                                                      (node->code.addr2->kind == CONSTANT),
                                                      (node->code.addr3->kind == CONSTANT));
                        break;

                    // case BREAK:
                    //     IRcode = "BREAK";
                    //     break;
                    // case CONTINUE:
                    //     IRcode = "CONTINUE";

                    //     break;
                    }   
                cout << IRcode << endl;
                IRofs << IRcode << endl;
            }
        }
        void printIR() { 
            show(IR_list->next); 
        }

        void remove(InterCode code) {
            IRNode node = search(code);
            if (node) {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                delete node;
            }
        }
};

Addr generate_exp(TreeNode *cur);
Addr generate_statement(TreeNode *cur);