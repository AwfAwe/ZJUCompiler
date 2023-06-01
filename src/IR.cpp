Addr generate_statement(TreeNode *cur){
    Addr addr1,addr2,addr3;
    if(cur->value == "statement_exp"){
       addr1 = generate_statement(cur->child[0]);
       
    }
    else if(cur->value == "statement_dec"){
       addr1 = generate_statement(cur->child[0]);

    }
    else if(cur->value == "statement_comp"){
       addr1 = generate_statement(cur->child[0]);

    }
    else if(cur->value == "statement_sel"){
       addr1 = generate_statement(cur->child[0]);
        
    }
    else if(cur->value == "statement_iter"){
       addr1 = generate_statement(cur->child[0]);
        
    }
    else if(cur->value == "statement_jump"){
       addr1 = generate_statement(cur->child[0]);
        
    }
    else if(cur->value == "statement_return"){
       addr1 = generate_statement(cur->child[0]);
        
    }
    else if(cur->value == "expression_statement"){

    }
    else if(cur->value == "assignment_expression"){

    }
    else if(cur->value == "aconditional_expression"){

    }
    else if(cur->value == "logical_or_expression"){

    }
    else if(cur->value == "logical_and_expression"){

    }
    else if(cur->value == "inclusive_or_expression"){

    }
    else if(cur->value == "exclusive_or_expression"){

    }
    else if(cur->value == "and_expression"){

    }
    else if(cur->value == "equality_expression"){

    }
    else if(cur->value == "relational_expression_lt"){

    }
    else if(cur->value == "relational_expression_gt"){

    }
    else if(cur->value == "relational_expression_le"){

    }
    else if(cur->value == "relational_expression_ge"){

    }
    else if(cur->value == "shift_expression_left"){

    }
    else if(cur->value == "shift_expression_right"){

    }
    else if(cur->value == "additive_expression_add"){

    }
    else if(cur->value == "additive_expression_sub"){

    }
    else if(cur->value == "multiplicative_expression_mul"){

    }
    else if(cur->value == "multiplicative_expression_div"){

    }
    else if(cur->value == "cast_expression"){

    }
    else if(cur->value == "unary_expression_inc"){

    }
    else if(cur->value == "unary_expression_dec"){

    }
    else if(cur->value == "unary_expression_unary"){

    }
    else if(cur->value == "postfix_expression_array"){

    }
    else if(cur->value == "postfix_expression_call"){

    }
    else if(cur->value == "postfix_expression_struct"){

    }
    else if(cur->value == "postfix_expression_arrow"){

    }
    else if(cur->value == "postfix_expression_inc"){

    }
    else if(cur->value == "postfix_expression_dec"){

    }
    else if(cur->value == "argument_expression_list_opt"){

    }
    else if(cur->value == "argument_expression_list"){

    }
    else if(cur->value == "primary_expression_id"){

    }
    else if(cur->value == "primary_expression_constantInt"){

    }
    else if(cur->value == "primary_expression_constantChar"){

    }
    else if(cur->value == "primary_expression_brace"){

    }
    else if(cur->value == "selection_statement"){

    }
    else if(cur->value == "iteration_statement"){
    
    }
    else if(cur->value == "expression_opt"){

    }
    else if(cur->value == "jump_operator_break"){
    
    }
    else if(cur->value == "jump_operator_continue"){

    }
    else if(cur->value == "assignment_operator_assign"){

    }
    else if(cur->value == "assignment_operator_add"){

    }
    else if(cur->value == "assignment_operator_sub"){

    }
    else if(cur->value == "assignment_operator_mul"){

    }
    else if(cur->value == "assignment_operator_div"){

    }
    else if(cur->value == "assignment_operator_mod"){

    }
    else if(cur->value == "assignment_operator_left"){

    }
    else if(cur->value == "assignment_operator_right"){

    }
    else if(cur->value == "assignment_operator_and"){
    
    }
    else if(cur->value == "assignment_operator_xor"){

    }
    else if(cur->value == "assignment_operator_or"){

    }
    else if(cur->value == "unary_operator_addr"){

    }
    else if(cur->value == "unary_operator_ptr"){

    }
    else if(cur->value == "unary_operator_pos"){

    }
    else if(cur->value == "unary_operator_neg"){

    }
    else if(cur->value == "unary_operator_bitneg"){

    }
    else if(cur->value == "unary_operator_not"){

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