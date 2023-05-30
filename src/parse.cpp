#include "symbolTable.h"
#include "tree.h"
#include <iostream>
using namespace std;

extern vector<struct Table *> TableStack;
extern vector<string> errinfo;

void ck_var_decl(TreeNode *cur)
{
    SymbolTable T = SymbolTable();
    Entry *E;
    string id = cur->child[1]->value;
    string err;

    if (cur->child[0]->child[0]->value == "VOID")
    {
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":变量类型不能为void";
        errinfo.push_back(err);
    }
    else if (T.lookup(id) != nullptr) //查询符号表，若出现重复定义
    {
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":变量";
        err += cur->child[1]->value;
        err += "重定义";
        errinfo.push_back(err);
    }
    else
    {
        //将变量添加至符号表
        E = new Entry;
        E->id = id;
        E->kind = Variable;
        if (cur->child.size() == 2)
        {
            E->type = Type::Int;
            E->size = 0;
        }
        else if (cur->child.size() == 3)
        {
            E->type = Type::Array;
            E->size = cur->child[2]->attr.numval;
        }
        T.insert_Entry(E);
        // cout << "DEBUG: insert " << id << "...";
        //  if(T.lookup(id)!=nullptr)
        //      cout << "变量已插入" << endl;
    }
}

void analyze_params(TreeNode *cur)
{
    SymbolTable T = SymbolTable();
    Entry *E;
    string err;
    // cout << "DEBUG: 3" << endl;
    // cout << "DEBUG: cur_val = " << cur->value << endl;
    if (cur->value == "param_int")
    {
        E = new Entry;
        // cout << "DEBUG: 4, " << cur->child[0]->child[0]->value << endl;
        if (cur->child[0]->child[0]->value == "VOID")
        {
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":变量类型不能为void";
            errinfo.push_back(err);
            E->type = Type::Void;
        }
        else
        {
            E->type = Type::Int;
        }
        E->kind = Kind::Parameters;
        // cout << "DEBUG: 5, " << cur->child[1]->value << endl;

        E->id = cur->child[1]->value;

        E->size = 0;

        T.insert_param(E);
        // cout << "DEBUG: insert " << E->id << "...";
        //  if (T.lookup(E->id) != nullptr)
        //      cout << "变量已插入" << endl;
    }
    else if (cur->value == "param_array")
    {
        E = new Entry;
        if (cur->child[0]->child[0]->value == "VOID")
        {
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":变量类型不能为void";
            errinfo.push_back(err);
            E->type = Type::Void;
        }
        else
        {
            E->type = Type::Array;
        }
        E->kind = Kind::Parameters;
        E->id = cur->child[1]->value;
        E->size = 0;
        T.insert_param(E);
        // cout << "DEBUG: insert " << E->id << "...";
        //  if (T.lookup(E->id) != nullptr)
        //      cout << "变量已插入" << endl;
    }
    else
    {
        if (cur->child.empty() == false)
        {
            // 遍历子节点
            auto next = cur->child.begin();
            for (; next < cur->child.end(); next++)
            {
                analyze_params(*next);
            }
        }
    }
}

bool ck_fun_decl(TreeNode *cur)
{
    Entry *E = new Entry;
    SymbolTable T = SymbolTable();
    string id = cur->child[1]->value;
    string return_type = cur->child[0]->child[0]->value; //获取返回值类型
    string err;

    // cout << "DEBUG: 1" << return_type << endl;
    //查询符号表，若出现重复定义
    if (T.look_up_global(id) != nullptr)
    {
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":函数";
        err += cur->child[1]->value;
        err += "重定义";
        errinfo.push_back(err);
        return false;
    }

    // T.create_table();

    E->id = id;
    if (return_type == "INT")
        E->type = Int;
    if (return_type == "VOID")
        E->type = Void;

    E->kind = Function;
    E->size = 0;

    T.insert_Entry(E);
    // cout << "DEBUG: insert " << E->id << "...";
    //  if (T.lookup(E->id) != nullptr)
    //      cout << "变量已插入" << endl;

    //分析参数列表
    if (cur->child[2]->child[0]->value == "VOID")
    {
        //没有参数
        // cout << "DEBUG: 函数" << id << "没有参数"<<endl;
    }
    else
        analyze_params(cur->child[2]);
    return true;
}

void ck_ret_stmt(TreeNode *cur)
{
    string id;
    SymbolTable T = SymbolTable();
    string err;
    // cout << "DEBUG: 7" << endl;
    Type return_type = T.get_curEntry()->type; //函数返回值
    // cout << "DEBUG: 7" << endl;
    if (cur->child.size() == 2) //如果存在返回值
    {
        id = cur->child[1]->value;
        if (T.lookup(id) != nullptr) //查询符号表，若变量未定义
        {
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":变量";
            err += id;
            err += "在使用时未经定义";
            errinfo.push_back(err);
        }
        else if (return_type != Int) //查询符号表，若函数返回值不是int
        {
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":void类型的函数不允许有返回值";
            errinfo.push_back(err);
        }
    }
    else //不存在返回值
    {
        if (return_type != Void) //查询符号表，若函数返回值不是void
        {
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":return语句缺少返回值";
            errinfo.push_back(err);
        }
    }
}

void ck_var(TreeNode *cur)
{
    SymbolTable T = SymbolTable();
    Entry *E;
    string id = cur->child[0]->value;
    string err;

    // cout << "DEBUG: id = " << id << endl;

    //在符号表中查找var_name
    E = T.look_up_global(id);
    if (E == nullptr)
    {
        // cout << "DEBUG: 10" << endl;
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":变量";
        err += id;
        err += "在使用时未经定义";
        errinfo.push_back(err);
    }
    else if (E->type == Int &&
             cur->child.size() == 2) //如果是变量但是误用了[]
    {
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":变量";
        err += id;
        err += "不是数组";
        errinfo.push_back(err);
    }
    else //正确使用
    {
        // cout << "DEBUG: " << id << "=" << E->type << ";child.size=" <<
        // cur->child.size() << endl;
        if (E->type == Array && cur->child.size() == 2)
        {
            cur->attr.type = Int;
        }
        else
        {
            cur->attr.type = E->type;
        }
    }
}

void ck_exp(TreeNode *cur)
{
    string err;
    if (cur->child[0]->value == "var")
    {
        if (cur->child[0]->attr.type == cur->child[1]->attr.type)
        {
            cur->attr.type = cur->child[0]->attr.type;
        }
        else
        {
            cur->attr.type = Void;
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":赋值号左右类型不匹配";
            errinfo.push_back(err);
        }
    }
    else
    {
        cur->attr.type = cur->child[0]->attr.type;
    }
}

void ck_bin_exp(TreeNode *cur)
{
    string err;
    if (cur->child.size() == 1)
    {
        cur->attr.type = cur->child[0]->attr.type;
    }
    else
    {
        if (cur->child[0]->attr.type == Int &&
            cur->child[2]->attr.type == Int)
        {
            cur->attr.type = cur->child[0]->attr.type;
        }
        else
        {
            cur->attr.type = Void;
            err = "Error at line ";
            err += to_string(cur->row);
            err += ":操作数类型错误";
            errinfo.push_back(err);
        }
    }
}

void ck_factor(TreeNode *cur) { cur->attr.type = cur->child[0]->attr.type; }

bool ck_call(TreeNode *cur)
{
    SymbolTable T = SymbolTable();
    Entry *E;
    string id = cur->child[0]->value;
    string err;

    //查询符号表
    E = T.look_up_global(id);

    if (E == nullptr) //若函数未定义
    {
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":函数";
        err += id;
        err += "在使用时未经定义";
        errinfo.push_back(err);
        return false;
    }
    else if (E->kind != Function) //若ID不是函数
    {
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":";
        err += id;
        err += "不是函数";
        errinfo.push_back(err);
        return false;
    }

    //函数名正常
    return true;
}

void cal_arg_num(TreeNode *cur, vector<Type> &arg_list)
{
    if (cur->value == "expression")
    {
        arg_list.push_back(cur->attr.type);
    }
    else if (cur->child.empty() == false)
    {
        // 遍历子节点
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++)
        {
            cal_arg_num(*next, arg_list);
        }
    }
}

void ck_call_params(TreeNode *cur)
{
    vector<Type> arg_list;
    SymbolTable T = SymbolTable();
    Entry *E;
    string id = cur->child[0]->value;
    string err;
    int arg_num;
    bool flag = true; //正确

    //查询符号表
    E = T.look_up_global(id);

    //参数列表是否匹配
    cal_arg_num(cur, arg_list);

    vector<Entry *> parameters = E->parameters;
    if (arg_list.size() != parameters.size())
        flag = false;
    else
    {
        vector<Entry *>::iterator p;
        vector<Type>::iterator q = arg_list.begin();
        for (p = parameters.begin(); p < parameters.end(); p++)
        {
            if ((*p)->type != (*q))
            {
                flag = false;
                break;
            }
            q++;
        }
    }

    if (flag == false)
    {
        cur->attr.type = Void;
        err = "Error at line ";
        err += to_string(cur->row);
        err += ":函数调用时实参与形参不匹配";
        errinfo.push_back(err);
    }

    cur->attr.type = E->type;
}
//前序遍历？
void parse(SyntaxTree cur)
{
    SymbolTable T = SymbolTable();

    //检查定义中的语义错误，计算叶子节点的属性
    if (cur->value == "var_declaration") // 检查变量定义
        ck_var_decl(cur);
    else if (cur->value == "fun_declaration") // 检查函数定义
    {
        if (ck_fun_decl(cur) == false)
            return;
    }
    else if (cur->value == "compound_stmt")
    { //新建符号表
        T.create_table();
        T.load_param();
    }
    else if (cur->value == "var") //检查变量引用
        ck_var(cur);
    else if (cur->value == "call") //检查函数调用
    {
        if (ck_call(cur) == false)
            return;
    }

    // 遍历子节点
    if (!cur->child.empty())
    {
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++)
        {
            parse(*next);
        }
    }

    //计算树中所有节点的属性，进行属性验证
    if (cur->value == "expression")
        ck_exp(cur);
    else if (cur->value == "simple_expression")
        ck_bin_exp(cur);
    else if (cur->value == "additive_expression")
        ck_bin_exp(cur);
    else if (cur->value == "term")
        ck_bin_exp(cur);
    else if (cur->value == "factor")
        ck_factor(cur);
    else if (cur->value == "call") //检查参数
        ck_call_params(cur);
    else if (cur->value == "return_stmt")
        ck_ret_stmt(cur);
    else if (cur->value == "compound_stmt")
        T.delete_current_table();
}
