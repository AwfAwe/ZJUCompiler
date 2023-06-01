#pragma once
#include "symbolTable.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Attr {
    Type type;  //类型，只有函数和变量有
    int numval; //数值
    char cnumval;
    float fnumval;
    int isdone;
};

class TreeNode {
  public:
    TreeNode() {}
    ~TreeNode() {}
    TreeNode(string t_value) {
        type = "GENERATOR";
        value = t_value;
        attr.type = Void;
        attr.isdone = 0;
        attr.numval = 0;
        attr.fnumval = 0;
        attr.cnumval = 0;
    }
    void setNode(int r, string v, enum Type ty, string nv){
        row = r;
        value = v;
        attr.type = ty;
        if(ty == Char){
            type = "CHARACTER";
            attr.cnumval = nv.at(0);
        }
    }
    TreeNode *AddNode(TreeNode *node) {
        child.push_back(node);
        return this;
    }

    int row;
    string type;              //类型
    string value;             //字符值
    string scope;             //有效范围（所属函数名，全局变量则是“$”）
    struct Attr attr;         //属性
    vector<TreeNode *> child; //子节点
};

typedef TreeNode *SyntaxTree;
