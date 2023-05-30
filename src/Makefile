all:
	lex token.l
	yacc -d tree.y
	g++ lex.yy.c y.tab.c parse.cpp IRGenerate.cpp code_gen.cpp -o compiler
yacc: 
	lex token.l
	yacc -d tree.y
clean:
	rm lex.yy.c y.tab.c y.tab.h compiler
