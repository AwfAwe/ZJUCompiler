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

IDENTIFIER ([a-zA-Z]|'_')([a-zA-Z]|'_'|[0-9])*
FLOATNUM ([0-9]+)\.([0-9]+)
INTNUM [0-9]+
CHARACTERS \'[\x00-\x7F]\'


LEFT_ASSIGN "<<="
RIGHT_ASSIGN ">>="

EQUAL "=="
NOTEQUAL "!="
LESSEQUAL "<="
GREATEQUAL ">="

increment "++"
decrement "--"
arrow "->"

ADD_ASSIGN "+="
SUB_ASSIGN "-="
MUL_ASSIGN "*="
DIV_ASSIGN "/="
MOD_ASSIGN "%="
AND_ASSIGN "&="
OR_ASSIGN "|="
XOR_ASSIGN "^="

LEFT_SHIFT "<<"
RIGHT_SHIFT ">>"

AND "&&"
OR "||"

ORBIT "|"
ANDBIT "&"

LESSTHAN "<"
GREATTHAN ">"
PURE_ASSIGN "="


COMMENT "/*"([^*]|\*+[^*/])*\*+"/"

%%


{LEFT_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "LEFT_ASSIGN";
    yylval->value = yytext;
    return LEFT_ASSIGN;
}

{RIGHT_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "RIGHT_ASSIGN";
    yylval->value = yytext;
    return RIGHT_ASSIGN;
}

{EQUAL} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "EQUAL";
    yylval->value = yytext;
    return EQUAL;
}

{NOTEQUAL} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "NOTEQUAL";
    yylval->value = yytext;
    return NOTEQUAL;
}

{LESSEQUAL} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "LESSEQUAL";
    yylval->value = yytext;
    return LESSEQUAL;
}

{GREATEQUAL} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "GREATEQUAL";
    yylval->value = yytext;
    return GREATEQUAL;
}

{increment} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "increment";
    yylval->value = yytext;
    return increment;
}

{decrement} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "decrement";
    yylval->value = yytext;
    return decrement;
}

{arrow} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "arrow";
    yylval->value = yytext;
    return arrow;
}

{ADD_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "ADD_ASSIGN";
    yylval->value = yytext;
    return ADD_ASSIGN;
}

{SUB_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "SUB_ASSIGN";
    yylval->value = yytext;
    return SUB_ASSIGN;
}
{MUL_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "MUL_ASSIGN";
    yylval->value = yytext;
    return MUL_ASSIGN;
}
{DIV_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "DIV_ASSIGN";
    yylval->value = yytext;
    return DIV_ASSIGN;
}
{MOD_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "MOD_ASSIGN";
    yylval->value = yytext;
    return MOD_ASSIGN;
}
{AND_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "AND_ASSIGN";
    yylval->value = yytext;
    return AND_ASSIGN;
}

{OR_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "OR_ASSIGN";
    yylval->value = yytext;
    return OR_ASSIGN;
}

{XOR_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "XOR_ASSIGN";
    yylval->value = yytext;
    return XOR_ASSIGN;
}

{LEFT_SHIFT} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "LEFT_SHIFT";
    yylval->value = yytext;
    return LEFT_SHIFT;
}

{RIGHT_SHIFT} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "RIGHT_SHIFT";
    yylval->value = yytext;
    return RIGHT_SHIFT;
}

{AND} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "AND";
    yylval->value = yytext;
    return AND;
}

{OR} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "OR";
    yylval->value = yytext;
    return OR;
}

{ANDBIT} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "ANDBIT";
    yylval->value = yytext;
    return ANDBIT;
}

{ORBIT} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "ORBIT";
    yylval->value = yytext;
    return ORBIT;
}

{LESSTHAN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "LESSTHAN";
    yylval->value = yytext;
    return LESSTHAN;
}

{GREATTHAN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "GREATTHAN";
    yylval->value = yytext;
    return GREATTHAN;
}

{PURE_ASSIGN} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "PURE_ASSIGN";
    yylval->value = yytext;
    return PURE_ASSIGN;
}

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

"&"		{
	yylval = nullptr;
	return *yytext;
}

"+"		{
	yylval = nullptr;
	return *yytext;
}

"-"		{
	yylval = nullptr;
	return *yytext;
}

"*"		{
	yylval = nullptr;
	return *yytext;
}

"/"		{
	yylval = nullptr;
	return *yytext;
}

"%"		{
	yylval = nullptr;
	return *yytext;
}


"~"		{
	yylval = nullptr;
	return *yytext;
}

"!"		{
	yylval = nullptr;
	return *yytext;
}

"?"		{
	yylval = nullptr;
	return *yytext;
}

":"		{
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

"for" {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "FOR";
    return FOR;    
}

"do" {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "DO";
    return DO;    
}

"int"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "KEYWORD";
    yylval->value = "INT";
    return INT;
}

"char"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "KEYWORD";
    yylval->value = "CHAR";
    return CHAR;
}

"float"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "KEYWORD";
    yylval->value = "FLOAT";
    return FLOAT;
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

"const"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "KEYWORD";
    yylval->value = "CONST";
    return CONST;
}

"volatile"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "KEYWORD";
    yylval->value = "VOLATILE";
    return VOLATILE;
}

"break"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "KEYWORD";
    yylval->value = "BREAK";
    return BREAK;
}

"continue"    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "KEYWORD";
    yylval->value = "CONTINUE";
    return CONTINUE;
}

{IDENTIFIER}    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    yylval->type = "IDENTIFIER";
    yylval->value = yytext;
    return IDENTIFIER;
}

{INTNUM}    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "INTNUM";
    yylval->value = yytext;
    yylval->attr.type= Type::Int;
    yylval->attr.numval = atoi(yytext);
    return INTNUM;
}

{FLOATNUM}    {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "FLOATNUM";
    yylval->value = yytext;
    yylval->attr.type= Type::Float;
    yylval->attr.fnumval = atoi(yytext);
    return FLOATNUM;
}

{CHARACTERS} {
    yylval = new TreeNode;
    yylval->row = cur_row;
    //yylval->col = cur_column;
    yylval->type = "CHARACTERS";
    yylval->value = yytext;
    yylval->attr.type= Type::Char;
    yylval->attr.cnumval = atoi(yytext);
    return CHARACTERS;
}



\n          { cur_row++; }
[^ \n\t]     { printf("Error at line %d: Mysterious characters \'%s\'\n",cur_row,yytext); }

%%

int yywrap()    /*文件结束处理函数*/
{ 
    return 1;
}