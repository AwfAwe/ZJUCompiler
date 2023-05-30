%{
/*
yyin: lex输入文件，默认为stdin
yyout: lex输出文件，默认为stdout
*/
#include <iostream>
#include <string>
#include "tree.h"
#define YYSTYPE TreeNode*
#include "y.tab.h"
using namespace std;

int cur_row=1;
int cur_column=1;

%}



ID ([a-zA-Z]|"_")([a-zA-Z]|[0-9]|"_")*
NUM [0-9]+
RELOP "<="|"<"|">"|">="|"=="|"!="
ADDOP "+"|"-"
MULOP "*"|"/"
COMMENT "/*"([^*]|\*+[^*/])*\*+"/"

%%

";"		{
	yylval = nullptr;
	return *yytext;
}

","		{
	yylval = nullptr;
	return *yytext;
}

"("		{
	yylval = nullptr;
	return *yytext;
}

")"		{
	yylval = nullptr;
	return *yytext;
}

"["		{
	yylval = nullptr;
	return *yytext;
}

"]"		{
	yylval = nullptr;
	return *yytext;
}

"{"		{
	yylval = nullptr;
	return *yytext;
}

"}"		{
	yylval = nullptr;
	return *yytext;
}

"="		{
	yylval = nullptr;
	return *yytext;
}

"//"[^n]*   { }


"if"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "IF";
    return IF;
}

"else"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "ELSE";
    return ELSE;
}

"while"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "WHILE";
    return WHILE;
}

"int"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "INT";
    return INT;
}

"void"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "VOID";
    return VOID;
}

"return"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "RETURN";
    return RETURN;
}

{ID}    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "ID";
    yylval->value = yytext;
    return ID;
}

{NUM}    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "NUM";
    yylval->value = yytext;
    yylval->attr.type= Type::Int;
    yylval->attr.numval = atoi(yytext);
    return NUM;
}


{RELOP}		{
	yylval = new TreeNode;
    yylval->row = cur_row;
	//yylval->col = cur_column;
    yylval->type = "RELOP";
	yylval->value = yytext;
	return relop;
}

{ADDOP}		{
	yylval = new TreeNode;
    yylval->row = cur_row;
	//yylval->col = cur_column;
	yylval->type = "ADDOP";
	yylval->value = yytext;
	return addop;
}

{MULOP}		{
	yylval = new TreeNode;
    yylval->row = cur_row;
	//yylval->col = cur_column;
	yylval->type = "MULOP";
	yylval->value = yytext;
	return mulop;
}

\n          { cur_row++; }
[^ \n\t]     { printf("Error at line %d: Mysterious characters \'%s\'\n",cur_row,yytext); }

%%

int yywrap()    /*文件结束处理函数*/
{ 
    return 1;
}
