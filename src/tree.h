#pragma once
#include "symbolTable.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Attr {
    Type type;  //类型，只有函数和变量有
    int numval; //数值
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
        attr.numval = 0;
        attr.isdone = 0;
    }

    TreeNode *AddNode(TreeNode *node) {
        child.push_back(node);
        return this;
    }

    int row;
    // int col;
    string type;              //类型
    string value;             //字符值
    struct Attr attr;         //属性
    vector<TreeNode *> child; //子节点
};

typedef TreeNode *SyntaxTree;