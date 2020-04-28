/*Compilator LFTC Matheiu Flavius-Dan, grupa 4.2.(luni 14-16)*/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>
#include<stdarg.h>
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");
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
}Symbol;




#endif

