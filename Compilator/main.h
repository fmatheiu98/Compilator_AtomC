/*Compilator LFTC Matheiu Flavius-Dan, grupa 4.2.(luni 14-16)*/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>
#include<stdarg.h>
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");
#define STACK_SIZE (32*1024)
#define GLOBAL_SIZE (32*1024)


#pragma once
#ifndef PROGRAM

char* pch;
enum {
	ID, BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE, CT_INT, CT_REAL, CT_CHAR, CT_STRING, COMMA, SEMICOLON,
	LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ, END
};
typedef struct _Token {
	int code; // codul (numele)
	union {
		char* text; // folosit pentru ID, CT_STRING (alocat dinamic)
		long int i; // folosit pentru CT_INT, CT_CHAR
		double r; // folosit pentru CT_REAL
	};
	int are_exp; //pentru modul de afisare, am adaugat 3 campuri de test pentru cele 3 feluri de constante
	int are_hexa;
	int are_octal;
	int line; // linia din fisierul de intrare
	struct _Token* next; // inlantuire la urmatorul AL
}Token;

void tkerr(const Token* tk, const char* fmt, ...);
void err(const char* fmt, ...);

Token* addTk(int code);

char* createString(char* start, char* end);
int getNextTk();
void lexer(char *buf);
void afisare_atomi();

int consume(int code);
char* codeName(int code);//ret. numele lui tk ID->>ID
void parser();



enum { TB_INT, TB_DOUBLE, TB_CHAR, TB_STRUCT, TB_VOID };


struct _Symbol;
typedef struct _Symbol Symbol;
typedef struct {
	Symbol** begin; // the beginning of the symbols, or NULL
	Symbol** end; // the position after the last symbol
	Symbol** after; // the position after the allocated space
}Symbols;

typedef struct {
	int typeBase; // TB_*
	Symbol* s; // struct definition for TB_STRUCT
	int nElements; // >0 array of given size, 0=array without size, <0 non array
}Type;

enum { CLS_VAR, CLS_FUNC, CLS_EXTFUNC, CLS_STRUCT };
enum { MEM_GLOBAL, MEM_ARG, MEM_LOCAL };
typedef struct _Symbol {
	const char* name; // a reference to the name stored in a token
	int cls; // CLS_*
	int mem; // MEM_*
	Type type;
	int depth; // 0-global, 1-in function, 2... - nested blocks in function
	union {
		Symbols args; // used only of functions
		Symbols members; // used only for structs
	};
	union {
		void* addr; // vm: the memory address for global symbols
		int offset; // vm: the stack offset for local symbols
	};
}Symbol;

typedef union {
	int i; // int, char
	double d; // double
	const char* str; // char[]
}CtVal;

typedef struct {
	Type type; // type of the result
	int isLVal; // if it is a LVal
	int isCtVal; // if it is a constant value (int, real, char, char[])
	CtVal ctVal; // the constat value
}RetVal;


char stack[STACK_SIZE];
char* SP; // Stack Pointer
char* stackAfter; // first byte after stack; used for stack limit tests

enum { O_PUSHCT_A, O_PUSHCT_I, O_STORE, O_LOAD, O_CALLEXT, O_SUB_I, O_JT_I, O_HALT, O_CALL, O_CAST_I_D,
	O_DROP, O_ENTER, O_EQ_D , O_INSERT , O_OFFSET, O_PUSHFPADDR, O_RET, O_SUB_D}; // all opcodes; each one starts with O_
typedef struct _Instr {
	int opcode; // O_*
	union {
		int i; // int, char
		double d;
		void* addr;
	}args[2];
	struct _Instr* last, * next; // links to last, next instructions
}Instr;
Instr* instructions, * lastInstruction; // double linked list

Instr* addInstrAfter(Instr* after, int opcode);
Instr* addInstr(int opcode);
Instr* addInstrA(int opcode, void* addr);
Instr* addInstrI(int opcode, int val);
Instr* addInstrII(int opcode, int val1, int val2);
void deleteInstructionsAfter(Instr* start);

char globals[GLOBAL_SIZE];
int nGlobals;

void* allocGlobal(int size);

void pushd(double d);
double popd();
void pusha(void *a);
void* popa();
void pushi(int i);
int popi();
void pushc(char c);
char popc();

void run(Instr* IP);

Symbol* requireSymbol(Symbols* symbols, const char* name);

#endif

