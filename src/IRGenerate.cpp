#include "IR.h"

extern IRCode IRCodes;
int Addr_::temp = 0;

int label = 1;
int flag = 0;

string new_label() { return "label_" + to_string(label++); }

Addr generate_statement(TreeNode *cur);
Addr generate_statement_list(TreeNode *cur);
Addr generate_unary(TreeNode *cur);
Addr generate_equality_expression(TreeNode *cur);
Addr generate_tri_equality_expression(TreeNode *cur);

bool operator==(struct InterCode code1, struct InterCode code2) {
    return (code1.kind == code2.kind &&
            ((code1.addr1->name == code2.addr1->name &&
              code1.addr2->name == code2.addr2->name &&
              code1.addr3->name == code2.addr3->name) &&
             (code1.addr1->value == code2.addr1->value &&
              code1.addr2->value == code2.addr2->value &&
              code1.addr3->value == code2.addr3->value)));
}

void generate_declarator_pointer(TreeNode *cur1,TreeNode *cur2){
    //尚未实现多重修饰符
    Addr dest = new Addr_(POINT,cur1->child[0]->child[0]->value);
    InterCode code(POINTDEC,dest);
    IRCodes.insert(code);
}

void generate_declarator(TreeNode *cur,string type){
    if(cur->child.size() == 1){
        // cout<<"direct_declarator->ID"<<endl;
        Addr dest = new Addr_(VARIABLE,cur->child[0]->value);
        Addr typedest = new Addr_(VARIABLE,type);
        InterCode code(DECVARIABLE,typedest,dest);
        IRCodes.insert(code);
    }
    else{  //多维数组尚未处理
    cout<<"direct_declarator->direct_declarator '[' NUM ']'"<<endl;
        Addr dest = new Addr_(ARRAY, cur->child[1]->value,atoi(cur->child[2]->value.c_str()));
        InterCode code(DECARRAY,dest);
        IRCodes.insert(code);
    }
}

Addr generate_direct_declarator(TreeNode *cur,string type){
    if(cur->child.size() == 1){
        // cout<<"declarator->direct_declarator"<<endl;
        generate_declarator(cur->child[0],type);
    }
    else{
        cout<<"declarator->pointer direct_declarator"<<endl;
        generate_declarator_pointer(cur->child[0],cur->child[1]);
    }
}

Addr generate_assign_declarator(TreeNode *cur1,TreeNode *cur2,string type){
    // cout<<cur1->child[0]->child[0]->value<<endl;
    Addr dest = new Addr_(VARIABLEINIT,cur1->child[0]->child[0]->value);
    Addr typedest = new Addr_(VARIABLEINIT,type);
    Addr valuedest = new Addr_(VARIABLEINIT,cur2->child[0]->child[0]->value);
    InterCode code(DECVARIABLEINIT,typedest,dest,valuedest);
    IRCodes.insert(code);
}

Addr generate_val_declarator(string type,TreeNode *cur){
    
    if(cur->child.size() == 1){
    //    cout<<"init_declarator->declarator"<<endl;
       generate_direct_declarator(cur->child[0],type);
    }
    else{
        // cout<<"init_declarator->declarator PURE_ASSIGN initializer"<<endl;
       generate_assign_declarator(cur->child[0],cur->child[2],type);
    }
}
//变量声明，声明数组后需要分配内存空间
//其他类型变量不需要额外生成三地址码
Addr generate_val_list(string type,TreeNode *cur) { 
    if(cur->child.size() == 2){
    //    cout<<"init_declarator_list->init_declarator_list ',' init_declarator"<<endl;
       generate_val_list(type,cur->child[0]);
       generate_val_declarator(type,cur->child[1]);
    }
    else{   
    //    cout<<"init_declarator_list->init_declarator"<<endl;
       generate_val_declarator(type,cur->child[0]);
    }
}

Addr generate_val(TreeNode *cur){
    cout<<"declaration->type_specifier init_declarator_list"<<endl;
    return generate_val_list(cur->child[0]->child[0]->value,cur->child[1]);
}

Addr generate_cast(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"*->unary"<<endl;
        return generate_unary(cur);
    }
}

Addr generate_mul(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"*->->"<<endl;
        return generate_cast(cur);
    }
}

Addr generate_add(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"+->*"<<endl;
        return generate_mul(cur);
    }
}

Addr generate_shift_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"shift->add"<<endl;
        return generate_add(cur);
    }
}

Addr generate_relational_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"relational->shift"<<endl;
        return generate_shift_expression(cur);
    }
    else if(cur->value == "relational_expression_lt"){
        cout<<"relational_expression lt shift_expression"<<endl;
        // generate_tri_relational_lt_expression(cur);
    }
    else if(cur->value == "relational_expression_lt"){
        cout<<"relational_expression gt shift_expression"<<endl;
        // generate_tri_relational_gt_expression(cur);
    }
    else if(cur->value == "relational_expression_lt"){
        cout<<"relational_expression le shift_expression"<<endl;
        // generate_tri_relational_le_expression(cur);
    }
    else if(cur->value == "relational_expression_lt"){
        cout<<"relational_expression ge shift_expression"<<endl;
        // generate_tri_relational_ge_expression(cur);
    }
}

Addr generate_equality_expression(TreeNode *cur){
    cout<<"equality       "<<cur->value<<endl;
    if(cur->value == "equlity_expression_eq"){
        cout<<"equality_expression EQUAL relational_expression"<<endl;
        // return generate_tri_equality_expression(cur);
    }
    else if(cur->value == "equlity_expression_ne"){
        cout<<"equality_expression NOTEQUAL relational_expression"<<endl;
        // generate_tri_notequality_expression(cur);
    }
    else{
        cout<<"=->relational"<<endl;
        return generate_relational_expression(cur);
    }
}

Addr generate_tri_equality_expression(TreeNode *cur){
    Addr dest1 = generate_equality_expression(cur->child[0]);
    Addr dest2 = generate_relational_expression(cur->child[2]);
    cout<<"equal:"<<cur->child[0]->value<<"  "<<cur->child[2]->value<<endl;
}

Addr generate_and_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"and->="<<endl;
        return generate_equality_expression(cur);
    }
    else{
        cout<<"and_expression '&' equality_expression"<<endl;
        // generate_tri_and_expression(cur);
    }
}

Addr generate_logical_exclusive_or_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"exclusive_or->and"<<endl;
        return generate_and_expression(cur);
    }
    else{
        cout<<"exclusive_or_expression '^' and_expression"<<endl;
        // generate_tri_exclusive_or_expression(cur);
    }
}

Addr generate_logical_inclusive_or_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"inclusive or->exclusive_or"<<endl;
        return generate_logical_exclusive_or_expression(cur);
    }
    else{
        cout<<"inclusive_or_expression '|' exclusive_or_expression"<<endl;
        // generate_tri_inclusive_or_expression(cur);
    }
}

Addr generate_logical_and_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"and->inclusive or"<<endl;
        return generate_logical_inclusive_or_expression(cur);
    }
    else{
        cout<<"logical_and_expression AND inclusive_or_expression"<<endl;
        // generate_tri_and_expression(cur);
    }
}

Addr generate_logical_or_expression(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"or->and"<<endl;
        return generate_logical_and_expression(cur);
    }
    else{
        cout<<"logical_or_expression OR logical_and_expression"<<endl;
        // generate_tri_or_expression(cur);
    }
}

//简单表达式生成
Addr generate_conditional(TreeNode *cur) {
    Addr dest = new Addr_;
    // cout<<cur->value<<endl;
    if(cur->child.size() == 1){
        cout<<"conditional->logical_or_exoression"<<endl;
        return generate_logical_or_expression(cur);
    }
    else{
        cout<<"conditional->logical_or_expression '?' assignment_expression ':' conditional_expression"<<endl;
        // return generate_tri_conditional(cur);
    }
}

Addr generate_primary(TreeNode *cur){
    cout<<"primary:"<<cur->value<<endl;
    Addr dest;
    if(cur->value == "primary_expression_id"){
        dest = new Addr_(VARIABLE,cur->child[0]->value);
        return dest;
    }
    else if(cur->value == "primary_expression_constantInt"){
        dest = new Addr_(CONSTANT,cur->child[0]->value);
        return dest;
    }
    else if(cur->value == "primary_expression_constantChar"){
        dest = new Addr_(CONSTANT,cur->child[0]->value);
        return dest;
    }
    else if(cur->value == "primary_expression_brace"){
        dest = new Addr_(BRACEEXP,cur->child[0]->value);
        return dest;
    }
}

Addr generate_postfix(TreeNode *cur){
    if(cur->child.size() == 1){
        cout<<"postfix->primary"<<endl;
        return generate_primary(cur);
    }
    else{

    }
}

Addr generate_unary(TreeNode *cur){
    cout<<cur->child.size()<<endl;
    if(cur->child.size() == 1){
        cout<<"unary->postfix:"<<cur->value<<endl;
        return generate_postfix(cur);
    }
    else {
     if(cur->child[0]->value == "unary_expression_inc"){
        cout<<"unary->++"<<endl;
        Addr dest = generate_unary(cur->child[1]);
        Addr res;
        cout<<dest->name<<endl;
        string temp = "++" + dest->name;
        res = new Addr_(POSTFIX,temp);
        return res;
     }
     else if(cur->child[0]->value == "unary_expression_dec"){
        // Addr dest = generate_unary(cur->child[1]);
        // Addr res;
        // cout<<"unary->--"<<endl;
        // InterCode code(POSTFIXDEC,dest);
        // IRCodes.insert(code);
     }    
    //  else if(cur->child[0]->value == "unary_expression_unary"){
    //     cout<<"unary-> ->"<<endl;
    //     InterCode code(POSTFIXUNARY,dest);
    //  }
    }
}

Addr generate_assignment_operator(TreeNode *cur){
    if(cur->value == "assignment_operator_assign"){
        Addr dest = new Addr_(OP,"EQ");
        return dest;
    }
    else if(cur->value == "assignment_operator_add"){
        Addr dest = new Addr_(OP,"ADD");
        return dest;
    }
    else if(cur->value == "assignment_operator_sub"){
        Addr dest = new Addr_(OP,"SUB");
        return dest;
    }
        else if(cur->value == "assignment_operator_mul"){
        Addr dest = new Addr_(OP,"MUL");
        return dest;
    }
        else if(cur->value == "assignment_operator_div"){
        Addr dest = new Addr_(OP,"DIV");
        return dest;
    }
        else if(cur->value == "assignment_operator_mod"){
        Addr dest = new Addr_(OP,"MOD");
        return dest;
    }
        else if(cur->value == "assignment_operator_left"){
        Addr dest = new Addr_(OP,"LEFT");
        return dest;
    }
        else if(cur->value == "assignment_operator_right"){
        Addr dest = new Addr_(OP,"RIGHT");
        return dest;
    }
        else if(cur->value == "assignment_operator_and"){
        Addr dest = new Addr_(OP,"AND");
        return dest;
    }
        else if(cur->value == "assignment_operator_xor"){
        Addr dest = new Addr_(OP,"XOR");
        return dest;
    }
        else if(cur->value == "assignment_operator_or"){
        Addr dest = new Addr_(OP,"OR");
        return dest;
    }
}

//表达式
Addr generate_assignment(TreeNode *cur) {
    cout<<cur->value<<endl;
    if (cur->child.size() == 3) {
        cout<<"assignment->assignment->unary_expression assignment_operator assignment_expression"<<endl;
        Addr unary = generate_unary(cur->child[0]);
        cout<<"op"<<endl;
        Addr op = generate_assignment_operator(cur->child[1]);
        Addr ass = generate_assignment(cur->child[2]);
        cout<<unary->name<<" "<<op->name<<" "<<ass->name<<endl;
        InterCode code(ASSIGNMENT,unary,op,ass);
        IRCodes.insert(code);
    } else {
        return generate_conditional(cur);
    }
}

Addr generate_exp_stmt(TreeNode *cur) {
    if (cur->child.size() == 1){
        cout<<"expression->assignment"<<endl;
        return generate_assignment(cur->child[0]);
    }
    else{
        cout<<"expression->;"<<endl;
        ;
    }
}

Addr generate_ret_stmt(TreeNode *cur){
        Addr dest = generate_exp_stmt(cur->child[1]);
        InterCode code(RET, dest);
        cout<<"ret:"<<dest->name<<endl;
        IRCodes.insert(code);
        // flag = 0;
}

Addr generate_if_stmt(TreeNode *cur){
    if(cur->child.size() == 3){
        cout<<"if then"<<endl;
        Addr label1 = new Addr_(AddrKind::LAB, new_label());

        cout<<label1<<endl;
        Addr ass = generate_equality_expression(cur->child[1]);

        cout<<"flag1"<<endl;
        Addr state = generate_statement(cur->child[2]);

        cout<<"flag2"<<endl;
        InterCode code2(LABEL,label1);
        InterCode code1(SEL,ass,label1);
        IRCodes.insert(code1);
        IRCodes.insert(code2);
    }
    if(cur->child.size() == 4){
        // LABEL label1 = new_label();
        Addr ass = generate_assignment(cur->child[1]);
        Addr state = generate_statement(cur->child[2]);
        Addr elsestmt = generate_statement(cur->child[4]);
        InterCode code(SEL,ass,state,elsestmt);
        IRCodes.insert(code);
    }
}

Addr generate_statement(TreeNode *cur) {
    cout << cur->value << endl;
    if (cur->value == "statement_exp"){
        return generate_exp_stmt(cur->child[0]);
    }
    else if(cur->value == "statement_dec"){
        cur->child[0]->attr.isdone = 1;
        return generate_val(cur->child[0]);
    }
    else if (cur->value == "statement_comp"){
        cur->child[1]->attr.isdone = 1;
        return generate_statement_list(cur->child[0]);
    }
    else if (cur->value == "statement_sel"){
        return generate_if_stmt(cur->child[0]);
    }
    else if (cur->value == "statement_iter"){
        // generate_while_stmt(cur->child[0]);
    }
    else if (cur->value == "statement_jump"){
        // generate_jump_stmt(cur->child[0]);
    }
    else if (cur->value == "statement_return"){
        flag = 0;
        return generate_ret_stmt(cur->child[0]);
    }
}

Addr generate_statement_list(TreeNode *cur) {
    if (cur->child.size() == 2) {
        generate_statement_list(cur->child[0]);
        cur->child[0]->attr.isdone = 1;
        generate_statement(cur->child[1]);
    }
    else{
        cout<<"statement_list->statement"<<endl;
        cur->child[0]->attr.isdone = 1;
        return generate_statement(cur->child[0]);
    }
}

void generate_param(TreeNode *cur) {
    if(cur->value == "parameter_declaration"){
    cout<<"parameter_declaration -> type_specifier declarator"<<endl;
        Addr dest;
        Addr typedest;
        if (cur->child[1]->child[0]->child.size() == 1){
            // cout<<"variable:"<<cur->child[1]->child[0]->child[0]->value<<endl;
            typedest = new Addr_(VARIABLE,cur->child[0]->child[0]->value);
            dest = new Addr_(VARIABLE, cur->child[1]->child[0]->child[0]->value);
        }
        else{
            cout<<"array"<<endl;
            dest = new Addr_(ARRAY, cur->child[1]->child[0]->child[0]->value);
        }
        InterCode code(PARAM,typedest,dest);
        IRCodes.insert(code);
    }
    else{ 
        cout<<"parameter_declaration -> type_specifier declarator PURE_ASSIGN initializer"<<endl;
        // cout<<cur->value<<endl;
        Addr dest;
        Addr valuedest;
        Addr typedest;
        if (cur->child[1]->child[0]->child.size() == 1){
            // cout<<"variableinit:"<<cur->child[1]->child[0]->child[0]->value<<endl;
            // cout<<"variableinit:"<<stoi(cur->child[3]->child[0]->child[0]->value)<<endl;
            dest = new Addr_(VARIABLEINIT,cur->child[1]->child[0]->child[0]->value);
            typedest = new Addr_(VARIABLEINIT,cur->child[0]->child[0]->value);
            valuedest = new Addr_(VARIABLEINIT,cur->child[3]->child[0]->child[0]->value);
        }
        else{
            // dest = new Addr_(ARRAYINIT, cur->child[1]->value,cur->child[3]->value);
        }
        InterCode code(PARAM,typedest,dest,valuedest);
        IRCodes.insert(code);
    }
}

void generate_param_list(TreeNode *cur) {
    if (cur->child.size() == 1){
    // cout<<"parameter_list->parameter_declaration"<<endl;
        generate_param(cur->child[0]);
    }
    else {  
        cout<<"parameter_list ',' parameter_declaration"<<endl;
        generate_param_list(cur->child[0]);
        generate_param(cur->child[1]);
    }
}

void generate_params(TreeNode *cur) {
    // if (cur->child[0]->value == "VOID")
    //     ;
    // else
    //     generate_param_list(cur->child[0]);
    if(cur->child.size() == 0)     {
    //   cout<<"parameter_list_opt-> "<<endl;
      Addr dest = new Addr_(STRING,"empty");
      InterCode code(OpKind::EMPTYPARAM,dest);
      IRCodes.insert(code);
    }
    else {
    //   cout<<"parameter_list_opt->parameter_list"<<endl;
      generate_param_list(cur->child[0]);
      Addr dest = new Addr_(STRING,"BODY");
      InterCode code(OpKind::FUNCBODY,dest);
      IRCodes.insert(code);
    }     
}

void generate_func(TreeNode *cur) {  //包括函数指针和普通函数
    if(cur->child[1]->child.size() == 1){  //ID(declartor)
    // cout<<"function_definition->declarator(1)->direct_declarator"<<endl;
       Addr dest = new Addr_(STRING, cur->child[1]->child[0]->child[0]->value);   
       Addr typedest = new Addr_(STRING,cur->child[0]->child[0]->value);     
       InterCode code(OpKind::FUNC, typedest, dest);
       IRCodes.insert(code);
       generate_params(cur->child[2]);  //parameter_list_opt
    }
    else{                               //ID(pointer declartor)
    cout<<"function_definition->declarator(1)->pointer(0) direct_declarator"<<endl;
       Addr dest = new Addr_(STRING, "*"+cur->child[1]->child[0]->child[1]->child[0]->value+" "+cur->child[1]->value);
       InterCode code(OpKind::FUNC, dest);
       IRCodes.insert(code);
       generate_params(cur->child[2]);  //parameter_list_opt
    }                               
}

void GenerateIR(SyntaxTree cur) {
    //对于数组，需要申请内存空间
    // cout << "IR"<<cur->value << endl;
    
    if (cur->value == "function_definition"){
        flag = 1;
        generate_func(cur);
    }
    else if (cur->value == "declaration" && flag == 0 && !cur->attr.isdone)
        generate_val(cur);
    else if (cur->value == "statement_list")
        if (!cur->attr.isdone){
            generate_statement_list(cur);
            cur->attr.isdone = 1;
        }

    if (!cur->child.empty()) {
        // 遍历子节点
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++) {
            GenerateIR(*next);
        }
    }
}

