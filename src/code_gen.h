#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;
// #include "symbolTable.h"
enum ValueType {
    TypeVoid,
    TypeInt,
    TypeArray,
    TypeChar,
    TypeFloat
};
// extern enum Type;
extern ifstream ifs;
extern ofstream ofs;
// extern ofstream IRofs
enum regs_enum {
	zero = 0,	at = 1,    	v0 = 2,    	v1 = 3,
	a0 = 4,    	a1 = 5,    	a2 = 6,    	a3 = 7,
	t0 = 8,    	t1 = 9,    	t2 = 10,    t3 = 11,
	t4 = 12,    t5 = 13,    t6 = 14,    t7 = 15,
	s0 = 16,    s1 = 17,    s2 = 18,    s3 = 19,
	s4 = 20,    s5 = 21,    s6 = 22,    s7 = 23,
	t8 = 24,    t9 = 25,    k0 = 26,    k1 = 27,
	gp = 28,    sp = 29,    fp = 30,    ra = 31
};

/*
typedef struct position {
    bool allocated;
    int reg;
    int offset;
} position;

#define NullPos (struct position) { false, -1, -1 }

static position* ptable = NULL;

int get_ope_index(string ope)
{
	return 0;
}
*/

static vector<string> temp; // 临时变量表
static vector<string> variable; // 临时变量表
static int immediate_count = 0; // 立即数编号
static int argument_count = 0; // 函数参数编号
static bool return_switch = false; // 函数返回开关
static string last_argument;
static string last_function_declaration;
static string last_function_call;
static bool is_last_call = false;
static map<string, bool> function_return_table; // 函数返回值字典

enum src_type{
	vart,
	arrayvar,
	arraynum,
	imm_int,
	imm_char,
	imm_float,
};
static int current_global_offset = 0;
static int current_local_offset = 0;

class varEntry {

  public:
    string id; //符号标识
    ValueType type; //变量类型（对于函数来说，是返回值类型）
    int size;  //数组大小
	int offset; //在内存中的位置
	int regid;
	bool inReg; //是否在寄存器中
	bool dirty; //是否寄存器已写

    //数组类型声明
    varEntry(string id, ValueType type, int size) {
        this->id = id;
        this->type = type;
        this->size = size;
		offset = current_global_offset;
		current_global_offset += size*4;
    }
    //单变量类型
    varEntry(string id, ValueType type) {
        this->id = id;
        this->type = type;
        this->size = 0;
		offset = current_global_offset;
		current_global_offset += 4;
    }
    varEntry() {}

};

// 记录当前寄存器中的变量名以及当前寄存器中的值，当需要替换时，
// 将替换出去的寄存器中的变量吗名在table中找到对应entry得到其在内存中对应的offset 然后将其存进去
// 

static map<string, string> nameTotype;

static string current_reg_variable[12];
// int current_reg_var_value[12];
static int current_reg_variable_times[12];
static bool current_reg_variable_dirty[12];

static string current_reg_temp[7];
// int current_reg_temp_value[7];
static bool current_reg_temp_busy[7];

static string current_float_reg_variable[12];
// float current_float_reg_var_value[12];
static bool current_float_reg_variable_dirty[12];
static int current_float_reg_variable_times[12];

static string current_float_reg_temp[12];
static bool current_float_reg_temp_busy[12];
// int current_reg_temp_value[7];
static int nowtimes;
struct varTable{
	map<string, varEntry *> table;
};
static vector<struct varTable *> varTableStack;

class varTableFun {
  public:
    void create_table() {
        struct varTable *newTable = new varTable;
        varTableStack.push_back(newTable);
    }

    //插入表项
    bool insert_Entry(varEntry *e) {
		if(varTableStack.size() == 0){
			struct varTable *newTable = new varTable;
            varTableStack.push_back(newTable);
		}
		//   create_table();
        struct varTable *curTable = varTableStack[varTableStack.size() - 1];

        //在当前符号表中查找是否存在与需要插入的项的标识符相同的项
        if (!this->lookup(e->id)) {
            (curTable->table)[e->id] = e;
            // curTable->varEntry = (curTable->table)[e->id];
            return true;
        } else
            return false;
    }

    //在所有符号表中进行查找
    varEntry *lookup(string name) {
        for (int i = varTableStack.size() - 1; i >= 0; i--) {
            struct varTable *t = varTableStack[i];
            map<string, varEntry *>::iterator e = (t->table).find(name);
            if (e != (t->table).end())
                return e->second;
        }
        return NULL;
    }

    //在所有符号表中寻找当前寄存器中的变量 
    varEntry *lookup(string name, int idx) {
        for (int i = varTableStack.size() - 1; i >= 0; i--) {
            struct varTable *t = varTableStack[i];
            map<string, varEntry *>::iterator e = (t->table).find(name);
            if (e != (t->table).end() && e->second->inReg && e->second->regid==idx)
                return e->second;
        }
        return NULL;
    }


    //删除符号表
    void delete_current_table() {
        struct varTable *curTable = varTableStack[varTableStack.size() - 1];
        //在当前符号表中查找是否存在与需要插入的项的标识符相同的项
        struct varTable *t = curTable;
        varTableStack.pop_back();
        delete curTable;
    }

};

class targetCode
{
    public:
		string variable_reg[12] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11"};
		string temp_reg[7] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"}; 
		string argument_reg[8] = {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};
		
		string variable_float_reg[12] = {"fs0", "fs1", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7", "fs8", "fs9", "fs10", "fs11"};
		string temp_float_reg[12]= {"ft0", "ft1", "ft2", "ft3", "ft4","ft5","ft6","ft7","ft8","ft9","ft10","ft11"};
		string argument_float_reg[8] = {"fa0", "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7"};
		
		/* 寄存器分配的思想

			遇到temp变量直接在寄存器数组里进行替换, 因为temp变量的使用一定是相对连续的，
			一个temp变量只会使用两次，一次作为左值，一次作为右值，作为右值时代表它的生命周期结束，
			这样可以用一个busy数组来标记，
			当前的想法是如果temp变量不进入内存，但是这样会导致如果表达式过于复杂（超过寄存器数量个运算，会产生寄存器不够用的问题）
		   	但仔细想想不会出现这种情况，因为表达式部分的语法树一定是二叉树，最多同时用三个临时变量寄存器，因为作为右值的两个临时变量会被当场释放掉，不会出现一直占用的情况。
			所以在我们是是线上，temp只需要3个寄存器就够了

		   遇到var变量，
		   直接去符号表中去找，然后看对应的entry，是不是已经在寄存器上了，没有就先提取到寄存器上
		   先看当前reg是否已经存储过var，有就直接操作，然后访问次数加1
		   若没有则查看有无空闲的reg，若有，根据变量名从符号表中找到其对应的entry，然后找到其内存位置，将其取到寄存器上，
		   然后进行操作，如果是左值，就改reg的值，并且标记dirty
		   没有要用LRU策略进行替换，查看将要被替换出去的变量的dirty，若标记则需要先把reg内的值存到内存中对应位置
		
		*/
		void writeBack(varEntry *cur){
			if(cur->type==TypeInt){
				ofs << "	sw " + variable_reg[cur->regid] + ", " + to_string(-(cur->offset)) + "(sp)"<< endl;				
			}else if(cur->type==TypeFloat){
				ofs << "    sw " + variable_float_reg[cur->regid] + ", " + to_string(-(cur->offset)) + "(sp)"<< endl;				
			}
			cur->dirty = false;
			cur->inReg = false;
			cur->regid = 0;
		}

		void asm_load_imm(string dst, string imm){
			string regDst = reg_allcocate(dst, true);
			ofs << "    li " + regDst + ", " + imm << endl;
			cout << "	 li " + regDst + ", " + imm << endl;

		}

		string reg_allcocate(string value, bool islv){
			// 两个参数，一个是参数名，一个是判断是否为左值
			if(value=="zero")return value;
			if(value=="sp") return value;
			int index, newidx;
			if(value.find("temp_") != -1 || value.find("imm_") != -1){
			// 只针对中间代码生成的临时变量或立即数做简单分配
				int curTimes = 0x7FFFFFFF;
				if(nameTotype[value] == "int"){
					for(index = 0, newidx = -1; index < 7; index++) {
					// 在变量表中查找当前变量
						if(current_reg_temp[index] == value)
							break;
						if(current_reg_temp_times[index]<curTimes ){
							curTimes = current_reg_temp_times[index];
							newidx = index;
						}
					}
					if (index == 7) index = newidx;
					current_reg_temp[index] = value;
					current_reg_temp_times[index] = ++nowtimes;
					return temp_reg[index]; // 为当前变量分配的寄存器
				}else if(nameTotype[value] == "float"){
					for(index = 0, newidx = -1; index < 12; index++) {
					// 在变量表中查找当前变量
						if(current_float_reg_temp[index] == value)
							break;
						if(current_float_reg_temp_times[index] < curTimes){
							curTimes = current_reg_temp_times[index];
							newidx = index;
						}
					}
					if (index == 12) index = newidx;
					current_float_reg_temp[index] = value;
					current_float_reg_temp_times[index] = ++nowtimes;
					return temp_float_reg[index]; // 为当前变量分配的寄存器
				}
			} else {
				varTableFun F = varTableFun();
				varEntry* curVar = F.lookup(value);
				if(curVar==NULL) printf("ERROR"); // 应该都能找到

				if(curVar->inReg){
					if(curVar->type==Int){
						if(islv){
							curVar->dirty = true;
						}
						current_reg_variable_times[curVar->regid]=++nowtimes;		

						return variable_reg[curVar->regid];
					}else if(curVar->type==Float){
						if(islv){
							curVar->dirty = true;
						}
						current_float_reg_variable_times[curVar->regid]=++nowtimes;		
						return variable_float_reg[curVar->regid];
					}
				}else{
					if(curVar->type==Int){
						int curtime = 0x7FFFFFFF;
						for(index=0, newidx=-1;index<12;++index){
							if(current_reg_variable[index]==""){
								break;
							}
							if(current_reg_variable_times[index]<curtime){
								newidx = index; // 挑选最早使用的
								curtime = current_reg_variable_times[index];
							}
						}
						if(index==12){
							//需要用LRU替换策略, 已经用newidx选出来了
							varEntry* lastVar = F.lookup(current_reg_variable[newidx], newidx);
							if(lastVar->dirty) writeBack(lastVar);
							index = newidx;
						}
						curVar->inReg = true;
						curVar->dirty = islv;
						curVar->regid = index;
						current_reg_variable[index] = value;
						current_reg_variable_times[index] = ++nowtimes;
						
						if(islv==false){
							ofs << "	lw " + variable_reg[cur->regid] + ", " + to_string(-(cur->offset)) + "(sp)"<< endl;				
						}
						return variable_reg[index];

					}else if(curVar->type==Float){
						int curtime = 0x7FFFFFFF;
						for(index=0, newidx=-1;index<12;++index){
							if(current_float_reg_variable[index]==""){
								break;
							}
							if(current_float_reg_variable_times[index]<curtime){
								newidx = index; // 挑选最早使用的
								curtime = current_float_reg_variable_times[index];
							}
						}
						if(index==12){
							//需要用LRU替换策略, 已经用newidx选出来了
							varEntry* lastVar = F.lookup(current_float_reg_variable[newidx], newidx);
							if(lastVar->dirty) writeBack(lastVar);			
							index = newidx;
						}
						curVar->inReg = true;
						curVar->dirty = islv;
						curVar->regid = index;
						current_float_reg_variable[index] = value;
						current_float_reg_variable_times[index] = ++nowtimes;
						
						if(islv==false){
							ofs << "	lw " + variable_float_reg[cur->regid] + ", " + to_string(-(cur->offset)) + "(sp)"<< endl;				
						}
						return variable_float_reg[index];
					}


				}

			}
		}

		// 暂未完成
		string get_offset(string ope)
		{
			return "OFFSET(" + ope + ")";
		}
		
		void asm_head()
		{
			function_return_table["input"] = true;
			function_return_table["output"] = false;
			ofs << ".data" << endl;
			ofs << "_prompt: \n.asciiz \"Enter an integer: \"" << endl;
			ofs << "_ret: \n.asciiz \"\\n\"\n" << endl;
			ofs << ".globl main\n" << endl;
			ofs << ".text" << endl;
		}

		void asm_input()
		{
			ofs << "	li a7, 4" << endl;
			ofs << "	la a0, _prompt" << endl;
			ofs << "	ecall" << endl;
			ofs << "	li a7, 5" << endl;
			ofs << "	ecall" << endl;
		}

		void asm_output()
		{
			ofs << "	li $v0, 1" << endl;
			ofs << "	syscall" << endl;
			ofs << "	li $v0, 4" << endl;
			ofs << "	la $a0, _ret" << endl;
			ofs << "	syscall" << endl;
			ofs << "	move $v0, $0" << endl;
		}

		void asm_declaration_variable(string type,string declaration)
		{
			varEntry *variable = new varEntry();
		    ValueType temp;
			if(type == "INT"){
			  ValueType temp = ValueType::TypeInt;
              type = "int";
			}
			if(type == "FLOAT"){
			  ValueType temp = ValueType::TypeFloat;
              type = "float";
			}
			if(type == "CHAR"){
			  ValueType temp = ValueType::TypeChar;
              type = "char";
			}
			variable->type = temp;
			variable->id = declaration;
			varTableFun t = varTableFun();
			t.insert_Entry(variable);
			variable->offset = current_global_offset;
			current_global_offset += 4;
		}

        void asm_declaration_variable_initialize(string type,string declaration,string init)
		{
			varEntry *variable = new varEntry();
			ValueType temp;
			if(type == "INT"){
			  ValueType temp = ValueType::TypeInt;
              type = "int";
			}
			if(type == "FLOAT"){
			  ValueType temp = ValueType::TypeFloat;
              type = "float";
			}
			if(type == "CHAR"){
			  ValueType temp = ValueType::TypeChar;
              type = "char";
			}
			variable->type = temp;
			variable->id = declaration;
			varTableFun t = varTableFun();
			t.insert_Entry(variable);
			variable->offset = current_global_offset;
			current_global_offset += 4;
			string s = "imm_";
			s += to_string(++immediate_count);
			nameTotype[s] = type;
            asm_load_imm(s, init);
			string regi = reg_allcocate(s, false);
			   ofs<< "    sw " + regi + ",-" + to_string(current_global_offset) + "(sp)"<<endl;
			   cout<< "    sw " + regi + ",-" + to_string(current_global_offset) + "(sp)"<<endl;
		}

		void asm_function(string name){
			varTableFun t = varTableFun();
			t.create_table();
			ofs<<name + ":"<<endl;
			cout<<name + ":"<<endl;
 		}

		void asm_functionend(string name){
			varTableFun t = varTableFun();
			t.delete_current_table();
		}
		
		void printTarget()
		{ 
			string code;
			while(getline(ifs, code))
			{
				cout << code << endl;
			}
		}
};