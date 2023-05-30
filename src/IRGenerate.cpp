#include "IR.h"

extern IRCode IRCodes;
int Addr_::temp = 0;

int label = 1;

string new_label() { return "label_" + to_string(label++); }

bool operator==(struct InterCode code1, struct InterCode code2) {
    return (code1.kind == code2.kind &&
            ((code1.addr1->name == code2.addr1->name &&
              code1.addr2->name == code2.addr2->name &&
              code1.addr3->name == code2.addr3->name) &&
             (code1.addr1->value == code2.addr1->value &&
              code1.addr2->value == code2.addr2->value &&
              code1.addr3->value == code2.addr3->value)));
}

//变量声明，声明数组后需要分配内存空间
//其他类型变量不需要额外生成三地址码
void generate_val_dec(TreeNode *cur) {
    //declaration-> type_specifier init_declarator_list ';'
    if (cur->child.size() == 2) {
        Addr dest = new Addr_(ARRAY, cur->child[1]->value,
                              atoi(cur->child[2]->value.c_str()));
        InterCode code(DEC, dest);
        IRCodes.insert(code);
    }
}

void generate_arg_list(TreeNode *cur) {
    // // cout << "DEBUG: generate_arg_list" << endl;
    Addr dest = new Addr_;
    if (cur->child.size() == 2) {
        dest = generate_exp(cur->child[1]);
        InterCode code(OpKind::ARG, dest);
        IRCodes.insert(code);
        generate_arg_list(cur->child[0]);

    } else {
        dest = generate_exp(cur->child[0]);
        InterCode code(OpKind::ARG, dest);
        IRCodes.insert(code);
    }
}

void generate_arg(TreeNode *cur) {
    // // cout << "DEBUG: generate_arg" << endl;
    if (cur->child.size() == 1)
        generate_arg_list(cur->child[0]);
}

void generate_call(TreeNode *cur) {
    // // cout << "DEBUG: generate_call" << endl;
    generate_arg(cur->child[1]);
    Addr dest = new Addr_(STRING, cur->child[0]->value);
    InterCode code(OpKind::CALL, dest);
    IRCodes.insert(code);
}

Addr generate_var(TreeNode *cur) {
    // cout << "DEBUG: generate_var" << endl;
    Addr dest = new Addr_;
    if (cur->child.size() == 2) {
        Addr array = new Addr_(ARRAY, cur->child[0]->value);
        Addr index = generate_exp(cur->child[1]);
        Addr elem_size = new Addr_(CONSTANT, 4);
        Addr temp = new Addr_;
        InterCode code1(MUL, temp, index, elem_size);
        IRCodes.insert(code1);
        InterCode code2(ADDR, temp, array, temp);
        IRCodes.insert(code2);
        InterCode code3(VAL, dest, temp);
        IRCodes.insert(code3);
    } else {
        dest->kind = VARIABLE;
        dest->name = cur->child[0]->value;
    }
    return dest;
}

Addr generate_factor(TreeNode *cur) {
    // cout << "DEBUG: generate_factor " << cur->child[0]->value << endl;
    Addr dest = new Addr_;
    if (cur->child.size() == 3)
        dest = generate_exp(cur->child[1]);
    else if (cur->child[0]->value == "var")
        dest = generate_var(cur->child[0]);
    else if (cur->child[0]->value == "call")
        generate_call(cur->child[0]);
    else if (cur->child[0]->type == "NUM") {
        dest->kind = CONSTANT;
        dest->value = atoi(cur->child[0]->value.c_str());
        // // cout << "DEBUG:" << dest->value << endl;
    }

    return dest;
}

Addr generate_term(TreeNode *cur) {
    // cout << "DEBUG: generate_term" << endl;
    Addr dest = new Addr_;
    if (cur->child.size() == 3) {
        Addr addr1 = generate_term(cur->child[0]);
        OpKind mulop;
        if (cur->child[1]->value == "*")
            mulop = OpKind::MUL;
        else if (cur->child[1]->value == "/")
            mulop = OpKind::DIV;
        Addr addr2 = generate_factor(cur->child[2]);
        InterCode code(mulop, dest, addr1, addr2);
        IRCodes.insert(code);
    } else
        dest = generate_factor(cur->child[0]);
    return dest;
}

Addr generate_additive_exp(TreeNode *cur) {
    // cout << "DEBUG: generate_additive_exp" << endl;
    Addr dest = new Addr_;
    // // cout << "DEBUG: " << dest->name << endl;
    if (cur->child.size() == 3) {
        Addr addr1 = generate_additive_exp(cur->child[0]);
        OpKind addop;
        if (cur->child[1]->value == "+")
            addop = OpKind::ADD;
        else if (cur->child[1]->value == "-")
            addop = OpKind::SUB;
        Addr addr2 = generate_term(cur->child[2]);
        InterCode code(addop, dest, addr1, addr2);
        IRCodes.insert(code);

    } else
        dest = generate_term(cur->child[0]);

    return dest;
}

//简单表达式生成
Addr generate_simple_exp(TreeNode *cur) {
    // cout << "DEBUG: generate_simple_exp" << endl;
    Addr dest = new Addr_;
    // simple -> additive relop additive
    if (cur->child.size() == 3) {
        Addr addr1 = generate_additive_exp(cur->child[0]);
        OpKind relop;
        if (cur->child[1]->value == ">")
            relop = OpKind::G;
        else if (cur->child[1]->value == ">=")
            relop = OpKind::GEQ;
        else if (cur->child[1]->value == "<=")
            relop = OpKind::LEQ;
        else if (cur->child[1]->value == "<")
            relop = OpKind::L;
        else if (cur->child[1]->value == "==")
            relop = OpKind::EQ;
        else if (cur->child[1]->value == "!=")
            relop = OpKind::NE;
        Addr addr2 = generate_additive_exp(cur->child[2]);

        InterCode code(relop, dest, addr1, addr2);
        IRCodes.insert(code);
    }
    // simple -> additive
    else
        dest = generate_additive_exp(cur->child[0]);
    return dest;
}

//表达式
Addr generate_exp(TreeNode *cur) {
    // cout << "DEBUG: generate_exp" << endl;
    // simple
    if (cur->child.size() == 1) {
        return generate_simple_exp(cur->child[0]);
    } else {
        Addr dest = generate_var(cur->child[0]);
        Addr addr1 = generate_exp(cur->child[1]);
        InterCode code(ASSIGN, dest, addr1);
        IRCodes.insert(code);
        return dest;
    }
}

void generate_statement_list(TreeNode *cur) {
    if (cur->child.size() == 2) {
        generate_statement_list(cur->child[0]);
        cur->child[0]->attr.isdone = 1;
        generate_statement(cur->child[1]);
    }
}

void generate_exp_stmt(TreeNode *cur) {
    // cout << "DEBUG: generate_exp_stmt" << endl;
    if (cur->child.size() == 1)
        generate_exp(cur->child[0]);
}

void generate_ret_stmt(TreeNode *cur) {
    // cout << "DEBUG: generate_ret_stmt" << endl;
    if (cur->child.size() == 1) {
        InterCode code(RET);
        IRCodes.insert(code);
    } else {
        Addr dest = generate_exp(cur->child[1]);
        InterCode code(RET, dest);
        IRCodes.insert(code);
    }
}

void generate_comp_stmt(TreeNode *cur) {
    generate_statement_list(cur->child[1]);
    cur->child[1]->attr.isdone = 1;
}

void generate_if_stmt(TreeNode *cur) {
    // cout << "DEBUG: generate_if_stmt" << endl;
    Addr exp = generate_exp(cur->child[1]);
    Addr label1 = new Addr_(AddrKind::LAB, new_label());
    Addr label2;
    InterCode code1(SEL, exp, label1);
    IRCodes.insert(code1);
    if (cur->child.size() == 5)
        generate_statement(cur->child[4]);

    label2 = new Addr_(AddrKind::LAB, new_label());
    InterCode code2(GOTO, label2);
    IRCodes.insert(code2);

    InterCode code3(LABEL, label1);
    IRCodes.insert(code3);
    generate_statement(cur->child[2]);
    InterCode code4(LABEL, label2);
    IRCodes.insert(code4);
}

void generate_while_stmt(TreeNode *cur) {
    Addr label1 = new Addr_(AddrKind::LAB, new_label());
    InterCode code1(GOTO, label1);
    IRCodes.insert(code1);

    Addr label2 = new Addr_(AddrKind::LAB, new_label());
    InterCode code2(LABEL, label2);
    IRCodes.insert(code2);

    generate_statement(cur->child[2]);

    InterCode code3(LABEL, label1);
    IRCodes.insert(code3);

    Addr exp = generate_exp(cur->child[1]);

    InterCode code4(SEL, exp, label2);
    IRCodes.insert(code4);
}

void generate_statement(TreeNode *cur) {
    // cout << "DEBUG: generate_stmt" << endl;
    if (cur->child[0]->value == "expression_stmt")
        generate_exp_stmt(cur->child[0]);
    else if (cur->child[0]->value == "compound_stmt")
        generate_comp_stmt(cur->child[0]);
    else if (cur->child[0]->value == "selection_stmt")
        generate_if_stmt(cur->child[0]);
    else if (cur->child[0]->value == "iteration_stmt")
        generate_while_stmt(cur->child[0]);
    // else if (cur->child[0]->value == "iteration_stmt")
    //     generate_while_stmt(cur->child[0]);
    else if (cur->child[0]->value == "return_stmt")
        generate_ret_stmt(cur->child[0]);
}

void generate_param(TreeNode *cur) {
    //parameter_declaration -> type_specifier declarator
    if(cur->value == "parameter_declarations"){
        Addr dest;
        if (cur->value == "param_array")
            dest = new Addr_(ARRAY, cur->child[1]->value);
        else
            dest = new Addr_(VARIABLE, cur->child[1]->value);
        InterCode code(PARAM, dest);
        IRCodes.insert(code);
    }
    else{ //parameter_declaration -> type_specifier declarator PURE_ASSIGN initializer
        Addr dest;
        if (cur->value == "param_array")
            dest = new Addr_(ARRAYINIT, cur->child[1]->value,cur->child[2]->value,cur->child[3]->value);
        else
            dest = new Addr_(VARIABLEINIT, cur->child[1]->value,cur->child[2]->value,cur->child[3]->value);
        InterCode code(PARAM, dest);
        IRCodes.insert(code);
    }

}

void generate_param_list(TreeNode *cur) {
    //parameter_list->parameter_declaration
    if (cur->child.size() == 1)
        generate_param(cur->child[0]);
    else {  //parameter_list ',' parameter_declaration
        generate_param_list(cur->child[0]);
        generate_param(cur->child[1]);
    }
}

void generate_params(TreeNode *cur) {
    // if (cur->child[0]->value == "VOID")
    //     ;
    // else
    //     generate_param_list(cur->child[0]);
    if(cur->child.size == 0)     //parameter_list_opt-> 
      ;
    else      //parameter_list_opt->parameter_list
      generate_param_list(cur->child[0]);
}

void generate_func(TreeNode *cur) {  //包括函数指针和普通函数
    //function_definition->declarator(1)->direct_declarator
    if(cur->child[1]->child.size == 1)  //ID(declartor)
       Addr dest = new Addr_(STRING, cur->child[1]->value); 
    //function_definition->declarator(1)->pointer(0) direct_declarator
    else{                               //ID(pointer declartor)
       Addr dest = new Addr_(STRING, "*"+cur->child[1]->child[0]->child[1]->child[0]->value+" "+cur->child[1]->value);
    }                               
       
    InterCode code(OpKind::FUNC, dest);
    IRCodes.insert(code);
    generate_params(cur->child[2]);  //parameter_list_opt
}

void GenerateIR(SyntaxTree cur) {
    //对于数组，需要申请内存空间
    // cout << "=================generate IR==================" << endl;
    if (cur->value == "function_declaration")
        generate_func(cur);
    else if (cur->value == "declaration")
        generate_val_dec(cur);
    else if (cur->value == "statement_list")
        if (!cur->attr.isdone)
            generate_statement_list(cur);

    if (!cur->child.empty()) {
        // 遍历子节点
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++) {
            GenerateIR(*next);
        }
    }
}
