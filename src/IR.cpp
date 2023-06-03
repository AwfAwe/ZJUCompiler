#include "IR.h"

extern IRCode IRCodes;
int Addr_::temp = 0;

int label = 1;

string new_label() { return "label_" + to_string(label++); }

Addr generate_statement(TreeNode *cur);
Addr generate_statement_list(TreeNode *cur);
Addr generate_unary(TreeNode *cur);
Addr generate_equality_expression(TreeNode *cur);
Addr generate_tri_equality_expression(TreeNode *cur);
void GenerateIR(SyntaxTree cur);

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
    cout<<"direct_declarator->direct_declarator '[' NUM ']'"<<cur->child[0]->child[0]->value<<endl;
        Addr typedest = new Addr_(VARIABLE,type);
        Addr dest = new Addr_(ARRAY, cur->child[0]->child[0]->value);
        Addr length = new Addr_(ARRAY,atoi(cur->child[1]->value.c_str()));
        InterCode code(DECARRAY,typedest,dest,length);
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
    if(cur2->value == "initializer"){
        Addr valuedest = new Addr_(VARIABLEINIT,cur2->child[0]->child[0]->value);
        InterCode code(DECVARIABLEINIT,typedest,dest,valuedest);
        IRCodes.insert(code);
    }
    else if(cur2->value == "initializer_inputint"){
        Addr valuedest = new Addr_("INPUTINT");
        InterCode code(INTINPUT,typedest,dest,valuedest);
        IRCodes.insert(code);
    }
    else if(cur2->value == "initializer_inputfloat"){
        Addr valuedest = new Addr_("INPUTFLOAT");
        InterCode code(FLOATINPUT,typedest,dest,valuedest);
        IRCodes.insert(code);
    }
    else if(cur2->value == "initializer_inputchar"){
        Addr valuedest = new Addr_("INPUTCHAR");
        InterCode code(CHARINPUT,typedest,dest,valuedest);
        IRCodes.insert(code);
    }
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

Addr generate_assignment_operator(TreeNode *cur){
    if(cur->value == "assignment_operator_assign"){
        Addr dest = new Addr_(OP,"PURE_ASSIGN");
        return dest;
    }
    else if(cur->value == "assignment_operator_add"){
        Addr dest = new Addr_(OP,"ADD_ASSIGN");
        return dest;
    }
    else if(cur->value == "assignment_operator_sub"){
        Addr dest = new Addr_(OP,"SUB_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_mul"){
        Addr dest = new Addr_(OP,"MUL_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_div"){
        Addr dest = new Addr_(OP,"DIV_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_mod"){
        Addr dest = new Addr_(OP,"MOD_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_left"){
        Addr dest = new Addr_(OP,"LEFT_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_right"){
        Addr dest = new Addr_(OP,"RIGHT_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_and"){
        Addr dest = new Addr_(OP,"AND_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_xor"){
        Addr dest = new Addr_(OP,"XOR_ASSIGN");
        return dest;
    }
        else if(cur->value == "assignment_operator_or"){
        Addr dest = new Addr_(OP,"OR_ASSIGN");
        return dest;
    }
}


void generate_arg(TreeNode *cur) {
    Addr a1 = generate_exp(cur);
    InterCode code(OpKind::ARG, a1);
    IRCodes.insert(code);
}

void generate_arg_list(TreeNode *cur) {
    if (cur->child.size() == 1){
        generate_arg(cur->child[0]);
    }
    else {  
        cout<<"parameter_list ',' parameter_declaration"<<endl;
        generate_arg_list(cur->child[0]);
        generate_arg(cur->child[1]);
    }
}

void generate_args(TreeNode *cur){
    if(cur->child.size() == 0)     {
    //   cout<<"parameter_list_opt-> "<<endl;
      Addr dest = new Addr_(STRING,"empty");
      InterCode code(OpKind::EMPTYARG,dest);
      IRCodes.insert(code);
    }
    else {
    //   cout<<"parameter_list_opt->parameter_list"<<endl;
      generate_arg_list(cur->child[0]);
      Addr dest = new Addr_(STRING,"arg_end");
      InterCode code(OpKind::ARGEND,dest);
      IRCodes.insert(code);
    }   
}

Addr generate_lv(TreeNode *cur){
    Addr a1, a2, a3;
    if(cur->child.size()>1){
        a2 = generate_exp(cur->child[1]);
        a1 = generate_exp(cur->child[0]);
        if(a2->ty==vart){
            a3 = new Addr_(ARRAYVAR,a1->name,a2->name);
        }else if(a2->ty==imm_int){
            a3 = new Addr_(ARRAYNUM,a1->name,a2->name);
        }

        return a3;
    }
    else{
        a1 = new Addr_(cur->child[0]->value);
        cout<<cur->child[0]->value<<endl;
        return a1;
    }
}

Addr generate_exp(TreeNode *cur){
    Addr a1, a2, a3;
    if(cur->value == "assignment_expression"){
        // cout<<"assign"<<endl;
        a1 = generate_lv(cur->child[0]);
        a2 = generate_assignment_operator(cur->child[1]);
        a3 = generate_exp(cur->child[2]);
        cout<<a1->name<<"  "<<a3->value<<endl;
        InterCode code(ASSIGNMENT, a1, a2, a3);
        IRCodes.insert(code);
        return a1;
    }
    else if(cur->value == "conditional_expression"){
        //toDO
        a1 = generate_exp(cur->child[0]);
        a2 = generate_exp(cur->child[1]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(ASSIGNMENT, a1, a2, a3);
        IRCodes.insert(code);
        return a2;
    }
    else if(cur->value == "logical_or_expression"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(LOGICOR, a1, a2, a3);
        IRCodes.insert(code);
        return a1;       
    }
    else if(cur->value == "logical_and_expression"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(LOGICAND, a1, a2, a3);
        IRCodes.insert(code);
        return a1;       
    }
    else if(cur->value == "inclusive_or_expression"){
        a1 = new Addr_();
        // cout<<"BITOR"<<endl;
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(BITOR, a1, a2, a3);
        IRCodes.insert(code);
        return a1;         
    }
    else if(cur->value == "exclusive_or_expression"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(BITXOR, a1, a2, a3);
        IRCodes.insert(code);
        return a1;
    }
    else if(cur->value == "and_expression"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(BITAND, a1, a2, a3);
        IRCodes.insert(code);
        return a1;
    }
    else if(cur->value == "equality_expression_eq"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(EQ, a1, a2, a3);
        IRCodes.insert(code);
        return a1;        
    }else if(cur->value == "equality_expression_ne"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(NE, a1, a2, a3);
        IRCodes.insert(code);
        return a1;        
    }
    else if(cur->value == "relational_expression_lt"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(LT, a1, a2, a3);
        IRCodes.insert(code);
        return a1;         
    }
    else if(cur->value == "relational_expression_gt"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(GT, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "relational_expression_le"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(LE, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "relational_expression_ge"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(GE, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "shift_expression_left"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(LEFT, a1, a2, a3);
        IRCodes.insert(code);
        return a1;         
    }
    else if(cur->value == "shift_expression_right"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[2]);
        InterCode code(RIGHT, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "additive_expression_add"){
        cout<<"add"<<endl;
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(ADD, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "additive_expression_sub"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(SUB, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "multiplicative_expression_mul"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(MUL, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "multiplicative_expression_div"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);
        InterCode code(DIV, a1, a2, a3);
        IRCodes.insert(code);
        return a1; 
    }
    else if(cur->value == "cast_expression"){
        a1 = new Addr_();
        cout<<"111"<<endl;
        a2 = generate_exp(cur->child[1]);
        enum OpKind ty;
        cout<<cur->child[0]->child[0]->value<<endl;

        if(cur->child[0]->child[0]->value=="INT")ty=TOINT;
        if(cur->child[0]->child[0]->value=="CHAR")ty=TOCHAR;
        if(cur->child[0]->child[0]->value=="FLOAT")ty=TOFLOAT;
        InterCode code(ty,a1,a2);
        IRCodes.insert(code);
        return a1;
    }
    else if(cur->value == "unary_expression_inc"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[1]);
        InterCode code(PREINC,a1,a2);
        IRCodes.insert(code);
        return a1;       
    }
    else if(cur->value == "unary_expression_dec"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[1]);
        InterCode code(PREDEC,a1,a2);
        IRCodes.insert(code);
        return a1;  
    }
    else if(cur->value == "unary_expression_unary"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[1]);
        enum OpKind ty;
        if(cur->child[0]->value=="unary_operator_addr")ty=GETADDR;
        if(cur->child[0]->value=="unary_operator_ptr")ty=PTR;
        if(cur->child[0]->value=="unary_operator_pos")ty=POS;
        if(cur->child[0]->value=="unary_operator_neg")ty=NEG;
        if(cur->child[0]->value=="unary_operator_bitneg")ty=BITNEG;
        if(cur->child[0]->value=="unary_operator_not")ty=NOT;
        InterCode code(ty,a1,a2);
        IRCodes.insert(code);
        return a1;        
    }
    else if(cur->value == "postfix_expression_array"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        a3 = generate_exp(cur->child[1]);

        InterCode code(VISITARRAY, a1, a2, a3);
        IRCodes.insert(code);
        return a1;         
    }
    else if(cur->value == "postfix_expression_call"){
        cout<<"callfunc"<<endl;
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        cout<<"call"<<endl;
        InterCode code(CALL,a1,a2);
        IRCodes.insert(code);
        generate_args(cur->child[1]);
        cout<<"callfinish"<<endl;
        return a1;        
    }
    else if(cur->value == "postfix_expression_struct"){

    }
    else if(cur->value == "postfix_expression_arrow"){

    }
    else if(cur->value == "postfix_expression_inc"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        InterCode code(POSTINC,a1,a2);
        IRCodes.insert(code);
        return a1;    
    }
    else if(cur->value == "postfix_expression_dec"){
        a1 = new Addr_();
        a2 = generate_exp(cur->child[0]);
        InterCode code(POSTDEC,a1,a2);
        IRCodes.insert(code);
        return a1;    
    }
    else if(cur->value == "primary_expression_id"){
        a1 = new Addr_(cur->child[0]->value);
        return a1;
    }
    else if(cur->value == "primary_expression_constantInt"){
        a1 = new Addr_(cur->child[0]->value, cur->child[0]->attr.numval);
        return a1;
    }
    else if(cur->value == "primary_expression_constantFloat"){
        a1 = new Addr_(cur->child[0]->value, cur->child[0]->attr.fnumval);
        return a1;
    }
    else if(cur->value == "primary_expression_constantChar"){
        a1 = new Addr_(cur->child[0]->value, cur->child[0]->attr.cnumval);
        return a1;
    }
    else if(cur->value == "primary_expression_brace"){
        // cout<<"brace"<<endl;
        a1 = generate_exp(cur->child[0]);
        // cout<<"bracefinish"<<endl;
        return a1;
    }
    else if (cur->value == "expression_opt"){
        a1 = generate_exp(cur->child[0]);
        return a1;
    }
}

Addr generate_ret_stmt(TreeNode *cur){
    cout<<cur->child.size()<<"   "<<cur->child[1]->value<<endl;
    if(cur->child[1]->child.size()==1){
        Addr dest = generate_exp(cur->child[1]->child[0]);
        InterCode code(RET, dest);
        // cout<<"ret:"<<dest->name<<endl;
        IRCodes.insert(code);
    }else{
        Addr dest = new Addr_(EMPTY);
        InterCode code(RET, dest);
        // cout<<"ret:"<<endl;
        IRCodes.insert(code);
    }
}

Addr generate_if_stmt(TreeNode *cur){
    if(cur->child.size() == 3){
        cout<<"if then"<<endl;
        Addr label1 = new Addr_(AddrKind::LAB, new_label());
        Addr label2 = new Addr_(AddrKind::LAB, new_label());
        Addr ass = generate_exp(cur->child[1]);
        InterCode code1(SEL,ass,label1);
        InterCode code2(GOTO,label2);
        InterCode code3(LABEL,label1);
        InterCode code4(LABEL,label2);
        IRCodes.insert(code1);
        IRCodes.insert(code2);
        IRCodes.insert(code3);
        Addr state = generate_statement(cur->child[2]);
        IRCodes.insert(code4);
    }
    if(cur->child.size() == 5){
        // LABEL label1 = new_label();
        Addr ass = generate_exp(cur->child[1]);
        Addr label1 = new Addr_(AddrKind::LAB, new_label());
        Addr label2 = new Addr_(AddrKind::LAB, new_label());
        InterCode code1(SEL,ass,label1);
        InterCode code2(GOTO, label2);
        InterCode code3(LABEL,label1);
        InterCode code4(LABEL,label2);
        IRCodes.insert(code1);        
        Addr elsestmt = generate_statement(cur->child[4]);
        IRCodes.insert(code2);
        IRCodes.insert(code3);
        Addr state = generate_statement(cur->child[2]);
        IRCodes.insert(code4);
    }
}

Addr generate_iter(TreeNode *cur){
    Addr addr1, addr2, addr3;
    if(cur->child.size()==3){
        // while
        Addr label1 = new Addr_(AddrKind::LAB, new_label());
        Addr label2 = new Addr_(AddrKind::LAB, new_label());
        Addr label3 = new Addr_(AddrKind::LAB, new_label());
        InterCode goto1Code(GOTO, label1);
        InterCode goto3Code(GOTO, label3);
        InterCode label1Code(LABEL, label1);
        InterCode label2Code(LABEL, label2);   
        InterCode label3Code(LABEL, label3);   
        
        IRCodes.insert(label1Code);                     // label 1:   
        Addr ass = generate_exp(cur->child[1]);         
        InterCode selCode(SEL, ass, label2);            
        IRCodes.insert(selCode);                        // judge goto label2
        IRCodes.insert(goto3Code);                      // goto label3
        IRCodes.insert(label2Code);                     // label 2:
        generate_statement(cur->child[2]);              // body
        IRCodes.insert(goto1Code);                      // goto label 1
        IRCodes.insert(label3Code);                     // label 3:

    }else if(cur->child.size()==4){
        //Do while
        Addr label1 = new Addr_(AddrKind::LAB, new_label());
        Addr label2 = new Addr_(AddrKind::LAB, new_label());
        InterCode label1Code(LABEL, label1);
        IRCodes.insert(label1Code);                     // label 1:
        generate_statement(cur->child[1]);              // body
        Addr ass = generate_exp(cur->child[3]);        
        InterCode selCode(SEL, ass, label1);            
        IRCodes.insert(selCode);                        // judge goto label 1

    }else if(cur->child.size()==5){
        //for without                   FOR (exp1; judge; exp2) body
        generate_exp(cur->child[1]);                   //      exp1

        Addr label1 = new Addr_(AddrKind::LAB, new_label());
        Addr label2 = new Addr_(AddrKind::LAB, new_label());
        Addr label3 = new Addr_(AddrKind::LAB, new_label());
        InterCode goto1Code(GOTO, label1);
        InterCode goto2Code(GOTO, label2);
        InterCode goto3Code(GOTO, label3);
        InterCode label1Code(LABEL, label1);
        InterCode label2Code(LABEL, label2);           
        InterCode label3Code(LABEL, label3);           

        IRCodes.insert(label1Code);                     // label 1:
        Addr ass = generate_exp(cur->child[2]);         
        InterCode selCode(SEL, ass, label2);        
        IRCodes.insert(selCode);                        // judge goto label 2
        IRCodes.insert(goto3Code);                      // goto label 3
        IRCodes.insert(label2Code);                     // label 2:
        
        // cout<<"body "<<cur->child[4]->child.size()<<endl;
        generate_statement(cur->child[4]);              // body
        generate_exp(cur->child[3]);              // exp2
        IRCodes.insert(goto1Code);                      // goto label 1
        IRCodes.insert(label3Code);                     // label 3:
    }else if(cur->child.size()==6){
        //for with declarator
        Addr label1 = new Addr_(AddrKind::LAB, new_label());
        Addr label2 = new Addr_(AddrKind::LAB, new_label());
        Addr label3 = new Addr_(AddrKind::LAB, new_label());
        InterCode goto1Code(GOTO, label1);
        InterCode goto2Code(GOTO, label2);
        InterCode goto3Code(GOTO, label3);
        InterCode label1Code(LABEL, label1);
        InterCode label2Code(LABEL, label2);           
        InterCode label3Code(LABEL, label3);  
        generate_val_list(cur->child[1]->child[0]->value, cur->child[2]);

        IRCodes.insert(label1Code);                     // label 1:
        Addr ass = generate_exp(cur->child[3]);         
        InterCode selCode(SEL, ass, label2);        
        IRCodes.insert(selCode);                        // judge goto label 2
        IRCodes.insert(goto3Code);                      // goto label 3
        IRCodes.insert(label2Code);                     // label 2:
        
        generate_statement(cur->child[5]);              // body
        generate_exp(cur->child[4]);              // exp2
        IRCodes.insert(goto1Code);                      // goto label 1
        IRCodes.insert(label3Code);                     // label 3:
    }

}

Addr generate_output(TreeNode *cur){
    Addr addr1,addr2,addr3;
    if(cur->value == "output_int"){
        addr1 = generate_exp(cur->child[0]);
        InterCode code(INTOUTPUT,addr1);
        IRCodes.insert(code);
    }
    if(cur->value == "output_float"){
        addr1 = generate_exp(cur->child[0]);
        InterCode code(FLOATOUTPUT,addr1);
        IRCodes.insert(code);
    }
    if(cur->value == "output_char"){
        addr1 = generate_exp(cur->child[0]);
        InterCode code(CHAROUTPUT,addr1);
        IRCodes.insert(code);
    }
    if(cur->value == "output_space"){
        addr1 = new Addr_("space");
        InterCode code(SPACEOUTPUT,addr1);
        IRCodes.insert(code);
    }
    if(cur->value == "output_line"){
        addr1 = new Addr_("line");
        InterCode code(LINEOUTPUT,addr1);
        IRCodes.insert(code);
    }

}

Addr generate_statement(TreeNode *cur){
    Addr addr1,addr2,addr3;
    cout<<"state:"<<cur->value<<endl;
    if(cur->value == "statement_list"){
        generate_statement_list(cur);
    }
    else if(cur->value == "statement_exp"){
        if(cur->child[0]->child.size()==1){
            addr1 = generate_exp(cur->child[0]->child[0]);
        }
    }
    else if(cur->value == "statement_dec"){
       addr1 = generate_val(cur->child[0]);

    }
    else if(cur->value == "statement_comp"){
        if(cur->child.size()==1){
            cout<<"comp"<<endl;
            generate_statement_list(cur->child[0]);
        }
    }
    else if(cur->value == "statement_sel"){
       addr1 = generate_if_stmt(cur->child[0]);
        
    }
    else if(cur->value == "statement_iter"){
       addr1 = generate_iter(cur->child[0]);
    }
    else if(cur->value == "statement_jump"){
    //    addr1 = generate_statement(cur->child[0]);
        if(cur->child[0]->value=="jump_operator_break"){

        }else if(cur->child[0]->value=="jump_operator_continue"){
                    
        }
    }
    else if(cur->value == "statement_return"){
       addr1 = generate_ret_stmt(cur->child[0]);
    }
    else if(cur->value == "statement_output"){
        addr1 = generate_output(cur->child[0]);
    }
    
}

Addr generate_statement_list(TreeNode *cur) {
    if (cur->child.size() == 2) {
        cout<<"statement_list->statement_list "<<endl;
        generate_statement_list(cur->child[0]);
        generate_statement(cur->child[1]);
    }
    else{
        cout<<"statement_list->statement "<<cur->child[0]->child.size()<<endl;
        generate_statement(cur->child[0]);
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
            typedest = new Addr_(VARIABLE,cur->child[0]->child[0]->value);
            dest = new Addr_(ARRAY, cur->child[1]->child[0]->child[0]->value,stoi(cur->child[1]->child[0]->child[1]->value));
        }
        InterCode code(PARAM,typedest,dest);
        IRCodes.insert(code);
    }
    else{ 
        // cout<<"parameter_declaration -> type_specifier declarator PURE_ASSIGN initializer"<<endl;
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
       GenerateIR(cur->child[3]);
       cout<<"111"<<endl;
       InterCode code2(OpKind::FUNCEND, dest);
       IRCodes.insert(code2);
    }
    else{                               //ID(pointer declartor)
    cout<<"function_definition->declarator(1)->pointer(0) direct_declarator"<<endl;
       Addr dest = new Addr_(STRING, "*"+cur->child[1]->child[0]->child[1]->child[0]->value+" "+cur->child[1]->value);
       InterCode code(OpKind::FUNC, dest);
       IRCodes.insert(code);
       generate_params(cur->child[2]);  //parameter_list_opt
       GenerateIR(cur->child[3]);
    }                               
}

void GenerateIR(SyntaxTree cur) {
    //对于数组，需要申请内存空间
    // cout << "IR"<<cur->value << endl;
    
    if (cur->value == "function_definition"){
        generate_func(cur);
        // GenerateIR(cur->child[3]);
        return;
    }
    else if (cur->value == "declaration"){
        // 在此进入的一定是全局变量
        generate_val(cur);
        return;
    }
    else if (cur->value == "statement_list"){
        generate_statement_list(cur);
        cur->attr.isdone = 1;
        return;
    }

    if (!cur->child.empty()) {
        // 遍历子节点
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++) {
            GenerateIR(*next);
        }
    }
}