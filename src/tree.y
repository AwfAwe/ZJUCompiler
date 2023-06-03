%{
#define YYSTYPE TreeNode*
//#define YYDEBUG 1
#include<iostream>
#include<algorithm>
#include<fstream>
#include"tree.h"
#include"IR.h"
#include"code_gen.h"
using namespace std;

int yylex();
void yyerror(char* str);
void parse(SyntaxTree cur);	// parse.cpp
void GenerateIR(SyntaxTree cur); //IRGenerate.cpp
SyntaxTree root;
vector<struct Table *> TableStack;
vector<string> errinfo;
IRCode IRCodes;
IRCode IR_Code;
ifstream ifs;
ofstream ofs;
ofstream IRofs;
%}
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%token increment decrement arrow
%token IDENTIFIER INTNUM FLOATNUM CHARACTERS
%token INT FLOAT CHAR VOID
%token CONST VOLATILE
%token PURE_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%token LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token EQUAL NOTEQUAL LESSEQUAL GREATEQUAL LESSTHAN GREATTHAN AND OR LEFT_SHIFT RIGHT_SHIFT ANDBIT ORBIT
%token IF DO WHILE FOR SWITCH BREAK CONTINUE RETURN

%right PURE_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%left '+' '-'
%left '*' '/'


%start program

%%
program : declaration_list {
	// printf("WTF");
	root=new TreeNode("program");
	$$=root;
	$$->AddNode($1);
	$$->row = $1->row;
}
;

declaration_list : external_declaration {
			$$=new TreeNode("declaration_list");
			$$->AddNode($1);
			$$->row = $1->row;

		}
        | declaration_list external_declaration {
			$$=new TreeNode("declaration_list");
			$$->AddNode($1)->AddNode($2);
			$$->row = $1->row;
		}
        ;

external_declaration : declaration {
						$$ = new TreeNode("external_declaration_var");
						$$->AddNode($1);
						$$->row = $1->row;
					}
                     | function_definition {
						$$ = new TreeNode("external_declaration_fun");
						$$->AddNode($1);
						$$->row = $1->row;
					 }
                     ;

declaration : type_specifier init_declarator_list ';' {
				$$ = new TreeNode("declaration");
				$$->AddNode($1)->AddNode($2);
				$$->row = $1->row;
			}
            ;

init_declarator_list : init_declarator {
						$$ = new TreeNode("init_declarator_list");
						$$->AddNode($1);
						$$->row = $1->row;						
					}
                     | init_declarator_list ',' init_declarator {
						$$ = new TreeNode("init_declarator_list");
						$$->AddNode($1)->AddNode($3);
						$$->row = $1->row;
					 }
                     ;

init_declarator : declarator {
					$$ = new TreeNode("init_declarator");
					$$->AddNode($1);
					$$->row = $1->row;						
				}
                | declarator PURE_ASSIGN initializer {
					$$ = new TreeNode("init_declarator");
					$$->AddNode($1)->AddNode($2)->AddNode($3);
					$$->row = $1->row;
				}
                ;

type_specifier : INT {
					$$ = new TreeNode("type_specifier_int");
					$$->AddNode($1);
					$$->row = $1->row;
				}
               | FLOAT {
					$$ = new TreeNode("type_specifier_float");
					$$->AddNode($1);
					$$->row = $1->row;
			   }
               | CHAR {
					$$ = new TreeNode("type_specifier_char");
					$$->AddNode($1);
					$$->row = $1->row;				
			   }
			   | VOID {
					$$ = new TreeNode("type_specifier_void");
					$$->AddNode($1);
					$$->row = $1->row;				
			   }
               ;

declarator : direct_declarator {
				$$ = new TreeNode("declarator");
				$$->AddNode($1);
				$$->row = $1->row;
			}
           | pointer direct_declarator {
				$$ = new TreeNode("declarator");
				$$->AddNode($1)->AddNode($2);
				$$->row = $1->row;
		   }
           ;


direct_declarator : IDENTIFIER {
						$$ = new TreeNode("direct_declarator");
						$$->AddNode($1);
						$$->row = $1->row;
					}
                  | direct_declarator '[' INTNUM ']' {
						$$ = new TreeNode("direct_declarator");
						$$->AddNode($1)->AddNode($3);
						$$->row = $1->row;					
				  }
                  ;


pointer : '*' type_qualifier_list {
			$$ = new TreeNode("pointer");
			$$->AddNode($2);
			$$->row = $2->row;			
		}
        ;

type_qualifier_list : type_qualifier {
						$$ = new TreeNode("type_qualifier_list");
						$$->AddNode($1);
						$$->row = $1->row;	
					}
                    | type_qualifier_list type_qualifier {
						$$ = new TreeNode("type_qualifier_list");
						$$->AddNode($1)->AddNode($2);
						$$->row = $1->row;						
					}
                    ;

type_qualifier : CONST {
					$$ = new TreeNode("type_qualifier_const");
					$$->AddNode($1);
					$$->row = $1->row;	
				}
               | VOLATILE {
					$$ = new TreeNode("type_qualifier_volatile");
					$$->AddNode($1);
					$$->row = $1->row;	
			   }
               ;

function_definition : type_specifier declarator '(' parameter_list_opt ')' compound_statement {
						$$ = new TreeNode("function_definition");
						$$->AddNode($1)->AddNode($2)->AddNode($4)->AddNode($6);
						$$->row = $1->row;	
					}
                    ;

parameter_list_opt : parameter_list {
						$$ = new TreeNode("parameter_list_opt");
						$$->AddNode($1);
						$$->row = $1->row;	
					}
                   | {
						$$ = new TreeNode("parameter_list_opt");
				   }
                   ;

parameter_list : parameter_declaration {
					$$ = new TreeNode("parameter_list");
					$$->AddNode($1);
					$$->row = $1->row;	

				}
               | parameter_list ',' parameter_declaration {
					$$ = new TreeNode("parameter_list");
					$$->AddNode($1)->AddNode($3);
					$$->row = $1->row;	
			   }
               ;

parameter_declaration : type_specifier declarator {
							$$ = new TreeNode("parameter_declaration");
							$$->AddNode($1)->AddNode($2);
							$$->row = $1->row;	
						}
						| type_specifier declarator PURE_ASSIGN initializer {
							$$ = new TreeNode("init_declarator");
							$$->AddNode($1)->AddNode($2)->AddNode($3)->AddNode($4);
							$$->row = $1->row;
						}
					  
					  ;

initializer : assignment_expression {
					$$ = new TreeNode("initializer");
					$$->AddNode($1);
					$$->row = $1->row;	
				}
            ;

compound_statement : '{' '}' {
						$$ = new TreeNode("compound_statement");
					}
                   | '{' statement_list '}' {
						$$ = new TreeNode("compound_statement");
						$$->AddNode($2);
						$$->row = $2->row;	
				   }
                   ;

statement_list : statement {
					$$ = new TreeNode("statement_list");
					$$->AddNode($1);
					$$->row = $1->row;	
				}
               | statement_list statement {
					$$ = new TreeNode("statement_list");
					$$->AddNode($1)->AddNode($2);
					$$->row = $1->row;	

			   }
               ;

statement : expression_statement {
    $$ = new TreeNode("statement_exp");
    $$->AddNode($1);
    $$->row = $1->row;
}
| declaration {
    $$ = new TreeNode("statement_dec");
    $$->AddNode($1);
    $$->row = $1->row;
}
| compound_statement {
    $$ = new TreeNode("statement_comp");
    $$->AddNode($1);
    $$->row = $1->row;
}
| selection_statement {
    $$ = new TreeNode("statement_sel");
    $$->AddNode($1);
    $$->row = $1->row;
}
| iteration_statement {
    $$ = new TreeNode("statement_iter");
    $$->AddNode($1);
    $$->row = $1->row;
}
| jump_statement {
    $$ = new TreeNode("statement_jump");
    $$->AddNode($1);
    $$->row = $1->row;
}
| return_statement {
    $$ = new TreeNode("statement_return");
    $$->AddNode($1);
    $$->row = $1->row;
}
;

return_statement : RETURN expression_statement {
    $$ = new TreeNode("return_statement");
    $$->AddNode($1)->AddNode($2);
    $$->row = $1->row;
}
;

expression_statement : ';' {
    $$ = new TreeNode("expression_statement");
}
| assignment_expression ';' {
    $$ = new TreeNode("expression_statement");
    $$->AddNode($1);
    $$->row = $1->row;
}
;


assignment_expression : conditional_expression {
    // $$ = new TreeNode("assignment_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| unary_expression assignment_operator assignment_expression {
    $$ = new TreeNode("assignment_expression");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
;

conditional_expression : logical_or_expression {
    // $$ = new TreeNode("conditional_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| logical_or_expression '?' assignment_expression ':' conditional_expression {
    $$ = new TreeNode("conditional_expression");
    $$->AddNode($1)->AddNode($3)->AddNode($5);
    $$->row = $1->row;
}
;

logical_or_expression : logical_and_expression {
    // $$ = new TreeNode("logical_or_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| logical_or_expression OR logical_and_expression {
    $$ = new TreeNode("logical_or_expression");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
;

logical_and_expression : inclusive_or_expression {
    // $$ = new TreeNode("logical_and_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| logical_and_expression AND inclusive_or_expression {
    $$ = new TreeNode("logical_and_expression");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
;

inclusive_or_expression : exclusive_or_expression {
    // $$ = new TreeNode("inclusive_or_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| inclusive_or_expression ORBIT exclusive_or_expression {
    $$ = new TreeNode("inclusive_or_expression");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
;

exclusive_or_expression : and_expression {
    // $$ = new TreeNode("exclusive_or_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| exclusive_or_expression '^' and_expression {
    $$ = new TreeNode("exclusive_or_expression");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
;

and_expression : equality_expression {
    // $$ = new TreeNode("and_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| and_expression '&' equality_expression {
    $$ = new TreeNode("and_expression");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
;

equality_expression : relational_expression {
    // $$ = new TreeNode("equality_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| equality_expression EQUAL relational_expression {
    $$ = new TreeNode("equality_expression_eq");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
| equality_expression NOTEQUAL relational_expression {
    $$ = new TreeNode("equality_expression_ne");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
;

relational_expression : shift_expression {
    // $$ = new TreeNode("relational_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| relational_expression LESSTHAN shift_expression {
    $$ = new TreeNode("relational_expression_lt");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
| relational_expression GREATTHAN shift_expression {
    $$ = new TreeNode("relational_expression_gt");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
| relational_expression LESSEQUAL shift_expression {
    $$ = new TreeNode("relational_expression_le");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
| relational_expression GREATEQUAL shift_expression {
    $$ = new TreeNode("relational_expression_ge");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
;

shift_expression : additive_expression {
    // $$ = new TreeNode("shift_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| shift_expression LEFT_SHIFT additive_expression {
    $$ = new TreeNode("shift_expression_left");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
| shift_expression RIGHT_SHIFT additive_expression {
    $$ = new TreeNode("shift_expression_right");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
;

additive_expression : multiplicative_expression {
    // $$ = new TreeNode("additive_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| additive_expression '+' multiplicative_expression {
    $$ = new TreeNode("additive_expression_add");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
| additive_expression '-' multiplicative_expression {
    $$ = new TreeNode("additive_expression_sub");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
;

multiplicative_expression : cast_expression {
    // $$ = new TreeNode("multiplicative_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| multiplicative_expression '*' cast_expression {
    $$ = new TreeNode("multiplicative_expression_mul");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
| multiplicative_expression '/' cast_expression {
    $$ = new TreeNode("multiplicative_expression_div");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
;

cast_expression : unary_expression {
    // $$ = new TreeNode("cast_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
|  '(' type_specifier ')' cast_expression {
    $$ = new TreeNode("cast_expression");
    $$->AddNode($2)->AddNode($4);
    $$->row = $4->row;
}
;

unary_expression : postfix_expression {
    // $$ = new TreeNode("unary_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
       $$ = $1;
}
| increment unary_expression {
    $$ = new TreeNode("unary_expression_inc");
    $$->AddNode($1)->AddNode($2);
    $$->row = $1->row;
}
| decrement unary_expression {
    $$ = new TreeNode("unary_expression_dec");
    $$->AddNode($1)->AddNode($2);
    $$->row = $1->row;
}
| unary_operator cast_expression {
    $$ = new TreeNode("unary_expression_unary");
    $$->AddNode($1)->AddNode($2);
    $$->row = $1->row;
}
;

postfix_expression : primary_expression {
    // $$ = new TreeNode("postfix_expression");
    // $$->AddNode($1);
    // $$->row = $1->row;
    $$ = $1;
}
| postfix_expression '[' assignment_expression ']' {
    $$ = new TreeNode("postfix_expression_array");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
| postfix_expression '(' argument_expression_list_opt ')' {
    $$ = new TreeNode("postfix_expression_call");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
| postfix_expression '.' IDENTIFIER {
    $$ = new TreeNode("postfix_expression_struct");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
| postfix_expression arrow IDENTIFIER {
    $$ = new TreeNode("postfix_expression_arrow");
    $$->AddNode($1)->AddNode($2)->AddNode($3);
    $$->row = $1->row;
}
| postfix_expression increment {
    $$ = new TreeNode("postfix_expression_inc");
    $$->AddNode($1)->AddNode($2);
    $$->row = $1->row;
}
| postfix_expression decrement {
    $$ = new TreeNode("postfix_expression_dec");
    $$->AddNode($1)->AddNode($2);
    $$->row = $1->row;
}
;

argument_expression_list_opt : argument_expression_list { 
    $$ = new TreeNode("argument_expression_list_opt");
    $$->AddNode($1);
    $$->row = $1->row;
}
|{
    $$ = new TreeNode("argument_expression_list_opt");
}
;

argument_expression_list : assignment_expression {
    $$ = new TreeNode("argument_expression_list");
    $$->AddNode($1);
    $$->row = $1->row;
 
}
| argument_expression_list ',' assignment_expression {
    $$ = new TreeNode("argument_expression_list");
    $$->AddNode($1)->AddNode($3);
    $$->row = $1->row;
}
;

primary_expression : IDENTIFIER {
    $$ = new TreeNode("primary_expression_id");
    $$->AddNode($1);
    $$->row = $1->row;
}
| INTNUM {
    $$ = new TreeNode("primary_expression_constantInt");
    $$->AddNode($1);
    $$->row = $1->row;
}
| FLOATNUM {
    $$ = new TreeNode("primary_expression_constantFloat");
    $$->AddNode($1);
    $$->row = $1->row;
}
| CHARACTERS {
    $$ = new TreeNode("primary_expression_constantChar");
    $$->AddNode($1);
    $$->row = $1->row;
}
| '(' assignment_expression ')' {
    $$ = new TreeNode("primary_expression_brace");
    $$->AddNode($2);
    $$->row = $2->row;
}
;

selection_statement : IF '(' assignment_expression ')' statement %prec LOWER_THAN_ELSE {
    $$ = new TreeNode("selection_statement");
    $$->AddNode($1)->AddNode($3)->AddNode($5);
    $$->row = $1->row;
}
| IF '(' assignment_expression ')' statement ELSE statement {
    $$ = new TreeNode("iteration_statement");
    $$->AddNode($1)->AddNode($3)->AddNode($5)->AddNode($6)->AddNode($7);
    $$->row = $1->row;
}
// | SWITCH '(' assignment_expression ')' statement
;


iteration_statement : WHILE '(' assignment_expression ')' statement {
    $$ = new TreeNode("iteration_statement");
    $$->AddNode($1)->AddNode($3)->AddNode($5);
    $$->row = $1->row;
}
| DO statement WHILE '(' assignment_expression ')' ';' {
    $$ = new TreeNode("iteration_statement");
    $$->AddNode($1)->AddNode($2)->AddNode($3)->AddNode($5);
    $$->row = $1->row;
}
| FOR '(' expression_opt ';' expression_opt ';' expression_opt ')' statement {
    $$ = new TreeNode("iteration_statement");
    $$->AddNode($1)->AddNode($3)->AddNode($5)->AddNode($7)->AddNode($9);
    $$->row = $1->row;
}
| FOR '(' type_specifier init_declarator_list ';' expression_opt ';' expression_opt ')' statement {
    $$ = new TreeNode("iteration_statement");
    $$->AddNode($1)->AddNode($3)->AddNode($4)->AddNode($6)->AddNode($8)->AddNode($10);
    $$->row = $1->row;
}
;


expression_opt : assignment_expression {
    $$ = new TreeNode("expression_opt");
    $$->AddNode($1);
    $$->row = $1->row;
}
| {
    $$ = new TreeNode("expression_opt");
}
;

jump_statement : BREAK ';' {
    $$ = new TreeNode("jump_operator_break");
    $$->AddNode($1);
    $$->row = $1->row;
}
| CONTINUE ';' {
    $$ = new TreeNode("jump_operator_continue");
    $$->AddNode($1);
    $$->row = $1->row;
}
;

assignment_operator : PURE_ASSIGN {
    $$ = new TreeNode("assignment_operator_assign");
    $$->AddNode($1);
    $$->row = $1->row;
}
| ADD_ASSIGN {
    $$ = new TreeNode("assignment_operator_add");
    $$->AddNode($1);
    $$->row = $1->row;
}
| SUB_ASSIGN {
    $$ = new TreeNode("assignment_operator_sub");
    $$->AddNode($1);
    $$->row = $1->row;
}
| MUL_ASSIGN { 
    $$ = new TreeNode("assignment_operator_mul");
    $$->AddNode($1);
    $$->row = $1->row;
}
| DIV_ASSIGN {
    $$ = new TreeNode("assignment_operator_div");
    $$->AddNode($1);
    $$->row = $1->row;
}
| MOD_ASSIGN {
    $$ = new TreeNode("assignment_operator_mod");
    $$->AddNode($1);
    $$->row = $1->row;
}
| LEFT_ASSIGN {
    $$ = new TreeNode("assignment_operator_left");
    $$->AddNode($1);
    $$->row = $1->row;
}
| RIGHT_ASSIGN {
    $$ = new TreeNode("assignment_operator_right");
    $$->AddNode($1);
    $$->row = $1->row;
}
| AND_ASSIGN {
    $$ = new TreeNode("assignment_operator_and");
    $$->AddNode($1);
    $$->row = $1->row;
}
| XOR_ASSIGN {
    $$ = new TreeNode("assignment_operator_xor");
    $$->AddNode($1);
    $$->row = $1->row;
}
| OR_ASSIGN {
    $$ = new TreeNode("assignment_operator_or");
    $$->AddNode($1);
    $$->row = $1->row;
}
;

unary_operator : '&'{
    $$ = new TreeNode("unary_operator_addr");
    // $$->row = $1->row;
}
| '*'{
    $$ = new TreeNode("unary_operator_ptr");

    // $$->row = $1->row;
}
| '+'{
    $$ = new TreeNode("unary_operator_pos");

    // $$->row = $1->row;
}
| '-'{
    $$ = new TreeNode("unary_operator_neg");

    // $$->row = $1->row;
}
| '~'{
    $$ = new TreeNode("unary_operator_bitneg");

    // $$->row = $1->row;
}
| '!'{
    $$ = new TreeNode("unary_operator_not");

    // $$->row = $1->row;
}
;

%%
void ShowSyntaxTree(SyntaxTree cur, int depth)
{
    string pre = "|";
    for (int i = 0; i < depth; i++)
    {
        pre += '-';
    }

    if (cur->type == "IDENTIFIER")
        cout << pre << "ID(" << cur->value << ")";
    else if (cur->type == "INTNUM")
        cout << pre << "NUM(" << cur->value << ")";
    else
        cout << pre << cur->value;
	
	if(cur->attr.type == Int)
    {
		cout << " --row=" << cur->row;
		cout << "; attr=INT" << endl;
	}
	else if(cur->attr.type == Array)
	{
		cout << " --row=" << cur->row;
		cout << "; attr=Arsray" << endl;
	}
	else
	{
		if(cur->row != 0)
			cout << " --row=" << cur->row;
		cout << endl;
	}

    if (!cur->child.empty())
    {
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++)
            ShowSyntaxTree(*next, depth + 1);
    }


}

void SaveSyntaxTree(SyntaxTree cur, int depth, ofstream& ofs)
{
    if (cur->type == "ID")
        ofs << depth << ":ID(" << cur->value << ")" << endl;
    else if (cur->type == "NUM")
        ofs << depth << ":NUM(" << cur->value << ")" << endl;
    else
        ofs << depth << ":" << cur->value << endl;
	
	// if(cur->attr.type == Int)
    // {
	// 	cout << " --row=" << cur->row;
	// 	cout << "; attr=INT" << endl;
	// }
	// else if(cur->attr.type == Array)
	// {
	// 	cout << " --row=" << cur->row;
	// 	cout << "; attr=Array" << endl;
	// }
	// else
	// {
	// 	if(cur->row != 0)
	// 		cout << " --row=" << cur->row;
	// 	cout << endl;
	// }

    if (!cur->child.empty())
    {
        auto next = cur->child.begin();
        for (; next < cur->child.end(); next++)
            SaveSyntaxTree(*next, depth + 1, ofs);
    }
}

bool compare(string a, string b)
{
    int line_a = 0, line_b = 0;
    auto pa = a.begin();
    auto pb = b.begin();
    while (!((*pa >= '0') && (*pa <= '9')))
        pa++;
    while (*pa >= '0' && *pa <= '9')
        line_a = line_a * 10 + (*(pa++) - '0');

    while (!((*pb >= '0') && (*pb <= '9')))
        pb++;
    while (*pb >= '0' && *pb <= '9')
        line_b = line_b * 10 + (*(pb++) - '0');
    return line_a < line_b;
}

void ShowErr(vector<string> &errinfo)
{
    vector<string>::iterator p;
    sort(errinfo.begin(), errinfo.end(), compare);
    for (p = errinfo.begin(); p < errinfo.end(); p++)
    {
        cout << *p << endl;
    }
}

int main()
{
	//printf("WTF");
	SymbolTable T = SymbolTable();
	ofstream ofs;
	ofs.open("SyntaxTree.txt", ios::out);
	// yydebug = 1;
	yyparse();
	// cout<<"======== SyntaxTree ========"<<endl;
	//ShowSyntaxTree(root, 0);
	//return 0;

	cout<<"Start Parsing SyntaxTree..."<<endl;
	T.create_table();
	parse(root);
	if(!errinfo.empty())
		ShowErr(errinfo);
	//cout<<"Finished."<<endl;
	
	//cout<<endl<<"========================== Annotated SyntaxTree ========================="<<endl;
	//ShowSyntaxTree(root, 0);
	SaveSyntaxTree(root, 0, ofs);
	ofs.close();
	if(errinfo.empty())
	{
		GenerateIR(root);
		cout<<endl<<"====================== Intermediate Representation ======================"<<endl;
		
		IRofs.open("pseudo_asm.s", ios::out);
		ofs.open("TargetCode.s", ios::out);
		targetCodes.asm_head();
		//targetCodes.asm_io();
		IRCodes.printIR();
		ofs.close();
		IRofs.close();
		cout<<endl<<"======================= Target Code Representation ======================"<<endl;
		
		ifs.open("TargetCode.s", ios::in);
		targetCodes.printTarget();
		ifs.close();
	}
}

void yyerror(char* str) {
	fprintf(stderr, "%s", str);
}
