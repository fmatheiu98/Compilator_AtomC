/*Compilator LFTC Matheiu Flavius-Dan, grupa 4.2.(luni 14-16)*/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>
#include<stdarg.h>
#include"main.h"

//Analizorul sintactic
extern Token* tokens;
Token* crtTk;
Token* consumedTk;
char ret[200];

extern Symbols symbols;
extern int crtDepth;// ("adancimea" contextului curent, initial 0)
extern Symbol* crtFunc;// (pointer la simbolul functiei daca in functie, altfel NULL)
extern Symbol* crtStruct;

void initSymbols(Symbols* symbols)
{
	symbols->begin = NULL;
	symbols->end = NULL;
	symbols->after = NULL;
}

Symbol* addSymbol(Symbols* symbols, const char* name, int cls)
{
	Symbol* s;
	if (symbols->end == symbols->after) { // create more room
		int count = symbols->after - symbols->begin;
		int n = count * 2; // double the room
		if (n == 0)n = 1; // needed for the initial case
			symbols->begin = (Symbol**)realloc(symbols->begin, n * sizeof(Symbol*));
		if (symbols->begin == NULL)err("not enough memory");
		symbols->end = symbols->begin + count;
		symbols->after = symbols->begin + n;
	}
	SAFEALLOC(s, Symbol)
		* symbols->end++ = s;
	s->name = name;
	s->cls = cls;
	s->depth = crtDepth;
	return s;
}

Symbol* findSymbol(Symbols* symbols, const char* name)
{
	if(symbols->begin==NULL)
		return NULL;
	
	Symbol *it = *symbols->end;
	it--;
	
	printf("%s", it->name);
	int contor = symbols->end - symbols->begin;
	
	for (int i=0;i<contor;i++)
	{
		if (it != NULL)
		{
			if (it->name != NULL && strcmp(it->name, name) == 0)
				return it;
		}
		else break;

		it--;
	}
	return NULL;
}

void deleteSymbolsAfter(Symbols *symbols, Symbol *start)
{
	
}


void addVar(Token* tkName, Type* t)
{
	Symbol* s;
	if (crtStruct) {
		if (findSymbol(&crtStruct->members, tkName->text))
			tkerr(crtTk, "symbol redefinition: %s", tkName->text);
		s = addSymbol(&crtStruct->members, tkName->text, CLS_VAR);
	}
	else if (crtFunc) {
		s = findSymbol(&symbols, tkName->text);
		if (s && s->depth == crtDepth)
			tkerr(crtTk, "symbol redefinition: %s", tkName->text);
		s = addSymbol(&symbols, tkName->text, CLS_VAR);
		s->mem = MEM_LOCAL;
	}
	else {
		if (findSymbol(&symbols, tkName->text))
			tkerr(crtTk, "symbol redefinition: %s", tkName->text);
		s = addSymbol(&symbols, tkName->text, CLS_VAR);
		s->mem = MEM_GLOBAL;
	}
	s->type = *t;
}

int unit();
int declStruct();
int declVar();
int typeBase(Type *ret);
int arrayDecl(Type* ret);
int typeName(Type* ret);
int declFunc();
int funcArg();
int stm();
int stmCompound();
int expr();
int exprAssign();
int exprOr();
int exprAnd();
int exprEq();
int exprRel();
int exprAdd();
int exprMul();
int exprCast();
int exprUnary();
int exprPostfix();
int exprPrimary();

char* codeName(int code)//ret. numele lui tk ID->>ID
{
	switch (code)
	{
	case ID: strcpy(ret, "ID"); return ret;
	case BREAK: strcpy(ret, "BREAK"); return ret;
	case CHAR: strcpy(ret, "CHAR"); return ret;
	case DOUBLE: strcpy(ret, "DOUBLE"); return ret;
	case ELSE: strcpy(ret, "ELSE"); return ret;
	case FOR: strcpy(ret, "FOR"); return ret;
	case IF: strcpy(ret, "IF"); return ret;
	case INT: strcpy(ret, "INT"); return ret;
	case RETURN: strcpy(ret, "RETURN"); return ret;
	case STRUCT: strcpy(ret, "STRUCT"); return ret;
	case VOID: strcpy(ret, "VOID"); return ret;
	case WHILE: strcpy(ret, "WHILE"); return ret;
	case CT_INT: strcpy(ret, "CT_INT"); return ret;
	case CT_REAL: strcpy(ret, "CT_REAL"); return ret;
	case CT_CHAR: strcpy(ret, "CT_CHAR"); return ret;
	case CT_STRING: strcpy(ret, "CT_STRING"); return ret;
	case COMMA: strcpy(ret, "COMMA"); return ret;
	case SEMICOLON: strcpy(ret, "SEMICOLON"); return ret;
	case LPAR: strcpy(ret, "LPAR"); return ret;
	case RPAR: strcpy(ret, "RPAR"); return ret;
	case LBRACKET: strcpy(ret, "LBRACKET"); return ret;
	case RBRACKET: strcpy(ret, "RBRACKET"); return ret;
	case LACC: strcpy(ret, "LACC"); return ret;
	case RACC: strcpy(ret, "RACC"); return ret;
	case ADD: strcpy(ret, "ADD"); return ret;
	case SUB: strcpy(ret, "SUB"); return ret;
	case MUL: strcpy(ret, "MUL"); return ret;
	case DIV: strcpy(ret, "DIV"); return ret;
	case DOT: strcpy(ret, "DOT"); return ret;
	case AND: strcpy(ret, "AND"); return ret;
	case OR: strcpy(ret, "OR"); return ret;
	case NOT: strcpy(ret, "NOT"); return ret;
	case ASSIGN: strcpy(ret, "ASSIGN"); return ret;
	case EQUAL: strcpy(ret, "EQUAL"); return ret;
	case NOTEQ: strcpy(ret, "NOTEQ"); return ret;
	case LESS: strcpy(ret, "LESS"); return ret;
	case LESSEQ: strcpy(ret, "LESSEQ"); return ret;
	case GREATER: strcpy(ret, "GREATER"); return ret;
	case GREATEREQ: strcpy(ret, "GREATEREQ"); return ret;
	case END: strcpy(ret, "END"); return ret;
	}
}

int consume(int code)
{
	printf("consume(%s)", codeName(code));
	if (crtTk->code == code) {
		printf("=>consumat\n");
		consumedTk = crtTk;
		crtTk = crtTk->next;
		return 1;
	}
	else {
		printf("=>altceva (%s)\n", codeName(crtTk->code));
		return 0;
	}
}

int unit()
{
	printf("@unit %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	for (;;)
	{
		if (declStruct())
		{
		}
		else if (declFunc())
		{
		}
		else if (declVar())
		{
		}
		else break;
	}
	if (consume(END))
	{
		return 1;
	}
	else tkerr(crtTk, "eroare de sintaxa\n");
	crtTk = start;
	return 0;
}

int declStruct()
{
	printf("@declStruct %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	int isDS = 0; //is declVar
	if (consume(STRUCT))
	{
		if (consume(ID))
		{
			Token* tkName = consumedTk;
			if (consume(LACC))
			{
				isDS = 1;
				{
					if (findSymbol(&symbols, tkName->text))
						tkerr(crtTk, "symbol redefinition: %s", tkName->text);
					crtStruct = addSymbol(&symbols, tkName->text, CLS_STRUCT);
					initSymbols(&crtStruct->members);
				}
				//while(declVar()){}
				for (;;)
				{
					if (declVar())
					{
					}
					else
					{
						break;
					}

				}
				if (consume(RACC))
				{
					if (consume(SEMICOLON))
					{
						{crtStruct = NULL; }
						return 1;
					}
					else tkerr(crtTk, "lipseste ; dupa }\n");
				}
				else tkerr(crtTk, "declarare de variabila invalida in struct sau lipseste }\n");
			}
		}
		else tkerr(crtTk, "lipseste numele structurii dupa struct\n");
	}
	crtTk = start;
	return 0;
}

int declVar()
{
	printf("@declVar %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	int isDV = 0; //is declVar
	Type t;
	if (typeBase(&t)) {
		if (consume(ID)) {
			isDV = arrayDecl(&t);//optional
			if (!isDV) {
				t.nElements = -1;
			}
			Token* tkName = consumedTk;
			addVar(tkName, &t);
			for (;;) {
				if (consume(COMMA)) {
					isDV = 1;
					if (consume(ID)) {
						Token* tkName = consumedTk;
						if (arrayDecl(&t)) {

						}
						else { t.nElements = -1; }
						addVar(tkName, &t);
					}
					else tkerr(crtTk, "lipseste numele variabilei dupa ,");
				}
				else break;
			}
			if (consume(SEMICOLON)) {
				return 1;
			}
			else {
				if (isDV) {
					tkerr(crtTk, "lipseste ; dupa declararea de variabila");
				}
			}
		}
	}
	crtTk = start;
	return 0;
}

int typeBase(Type *ret)
{
	printf("@typeBase %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(INT)) {
		{ret->typeBase = TB_INT; }
		return 1;
	}
	else if (consume(DOUBLE)) {
		{ret->typeBase = TB_DOUBLE; }
		return 1;
	}
	else if (consume(CHAR)) {
		{ret->typeBase = TB_CHAR; }
		return 1;
	}
	else if (consume(STRUCT)) {
		if (consume(ID))
		{
			Token* tkName = consumedTk;
			{
				Symbol* s = findSymbol(&symbols, tkName->text);
				if (s == NULL)tkerr(crtTk, "undefined symbol: %s", tkName->text);
				if (s->cls != CLS_STRUCT)tkerr(crtTk, "%s is not a struct", tkName->text);
				ret->typeBase = TB_STRUCT;
				ret->s = s;
			}
			return 1;
		}
		else tkerr(crtTk, "lipseste numele structurii dupa struct\n");
	}
	crtTk = start;
	return 0;
}

int arrayDecl(Type *ret)
{
	printf("@arrayDecl %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(LBRACKET))
	{
		if (expr()) {
			ret->nElements = 0;       // for now do not compute the real size
		}
		if (consume(RBRACKET))
			return 1;
		else tkerr(crtTk, "declaratie eronata de vector sau lipseste ]\n");
	}
	crtTk = start;
	return 0;
}

int typeName(Type *ret)
{
	printf("@typeName %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Type t;
	if (typeBase(ret))
	{
		if (arrayDecl(ret)) {
			return 1;
		}
		else ret->nElements = -1;
	}
	crtTk = start;
	return 0;
}

int declFunc()
{
	printf("@declFunc %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Type t;
	if (typeBase(&t))
	{
		if (consume(MUL)) {
			t.nElements = 0;
		}
		t.nElements = -1;
		if (consume(ID)) {
			Token* tkName = consumedTk;
			if (consume(LPAR)) {
				if (findSymbol(&symbols, tkName->text))
					tkerr(crtTk, "symbol redefinition: %s", tkName->text);
				crtFunc = addSymbol(&symbols, tkName->text, CLS_FUNC);
				initSymbols(&crtFunc->args);
				crtFunc->type = t;
				crtDepth++;
				if (funcArg()) {
					for (;;) {
						if (consume(COMMA)) {
							if (funcArg()) {
							}
							else tkerr(crtTk, "lipseste argumentul de dupa ,\n");
						}
						else break;
					}
				}
				if (consume(RPAR)) {
					crtDepth--;
					if (stmCompound())
					{
						deleteSymbolsAfter(&symbols, crtFunc);
						crtFunc = NULL;
						return 1;
					}
				}
				else tkerr(crtTk, "declaratie eronata de functie sau lipseste )\n");
			}
		}
	}
	else if (consume(VOID)) {
		t.typeBase = TB_VOID;
		if (consume(ID)) {
			Token* tkName = consumedTk;
			if (consume(LPAR)) {
				if (findSymbol(&symbols, tkName->text))
					tkerr(crtTk, "symbol redefinition: %s", tkName->text);
				crtFunc = addSymbol(&symbols, tkName->text, CLS_FUNC);
				initSymbols(&crtFunc->args);
				crtFunc->type = t;
				crtDepth++;
				if (funcArg()) {
					for (;;) {
						if (consume(COMMA)) {
							if (funcArg()) {
							}
							else tkerr(crtTk, "lipseste argumentul de dupa ,\n");
						}
						else break;
					}
				}
				if (consume(RPAR)) {
					crtDepth--;
					if (stmCompound())
					{
						deleteSymbolsAfter(&symbols, crtFunc);
						crtFunc = NULL;
						return 1;
					}
				}
				else tkerr(crtTk, "declaratie eronata de functie sau lipseste )\n");
			}
		}
	}
	crtTk = start;
	return 0;
}

int funcArg()
{
	printf("@funcArg %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Type t;
	if (typeBase(&t)) {
		if (consume(ID)) {
			Token* tkName = consumedTk;
			if (arrayDecl(&t)) {

			}
			t.nElements = -1;

			Symbol* s = addSymbol(&symbols, tkName->text, CLS_VAR);
			s->mem = MEM_ARG;
			s->type = t;
			s = addSymbol(&crtFunc->args, tkName->text, CLS_VAR);
			s->mem = MEM_ARG;
			s->type = t;
			return 1;
		}
		else tkerr(crtTk, "lipseste numele parametrului\n");
	}
	crtTk = start;
	return 0;
}

int stm()
{
	printf("@stm %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (stmCompound()) {
		return 1;
	}
	else
		if (consume(IF)) {
			if (consume(LPAR)) {
				if (expr()) {
					if (consume(RPAR)) {
						if (stm()) {
							consume(ELSE);
							if (stm()) {}
							return 1;
						}
						else tkerr(crtTk, "eroare in corpul lui if sau lipsa acestuia\n");
					}
					else tkerr(crtTk, "conditie invalida in if sau lipseste )\n");
				}
				else tkerr(crtTk, "conditie invalida pentru if\n");
			}
			else tkerr(crtTk, "lipseste ( dupa if\n");
		}
		else
			if (consume(WHILE)) {
				if (consume(LPAR)) {
					if (expr()) {
						if (consume(RPAR)) {
							if (stm()) {
								return 1;
							}
							else tkerr(crtTk, "eroare in corpul lui while sau lipsa acestuia\n");
						}
						else tkerr(crtTk, "conditie invalida in while sau lipseste )\n");
					}
					else tkerr(crtTk, "conditie invalida pentru while\n");
				}
				else tkerr(crtTk, "lipseste ( dupa while\n");
			}
			else
				if (consume(FOR)) {
					if (consume(LPAR)) {
						expr();
						if (consume(SEMICOLON)) {
							expr();
							if (consume(SEMICOLON)) {
								expr();
								if (consume(RPAR)) {
									if (stm()) {
										return 1;
									}
								}
								else tkerr(crtTk, "conditie invalida in for sau lipseste )\n");
							}
							else tkerr(crtTk, "lipseste ; dupa a 2-a expresie din for\n");
						}
						else tkerr(crtTk, "lipseste ; dupa prima expresie din for\n");
					}
					else tkerr(crtTk, "lipseste ( dupa for\n");
				}
				else
					if (consume(BREAK)) {
						if (consume(SEMICOLON)) {
							return 1;
						}
						else tkerr(crtTk, "lipseste ; dupa break\n");
					}
					else
						if (consume(RETURN)) {
							expr();
							if (consume(SEMICOLON)) {
								return 1;
							}
							else tkerr(crtTk, "lipseste ; sau expresie invalida\n");
						}
						else
						{
							expr();
							if (consume(SEMICOLON)) {
								return 1;
							}
						}
	crtTk = start;
	return 0;
}

int stmCompound()
{
	printf("@stmCompound %s\n", codeName(crtTk->code));
	Token* start1 = crtTk;
	Symbol* start = symbols.end[-1];
	if (consume(LACC)) {
		crtDepth++;
		for (;;) {
			if (declVar()) {

			}
			else if (stm()) {

			}
			else break;
		}
		if (consume(RACC)) {
			crtDepth--;
			deleteSymbolsAfter(&symbols, start);
			return 1;
		}
		else tkerr(crtTk, "eroare de sintaxa sau lipseste }\n");
	}
	crtTk = start1;
	return 0;
}

int expr()
{
	printf("@expr %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprAssign())
	{
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprAssign()////////////////////////////////////////////////
{
	printf("@exprAssign %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprUnary())
	{
		if (consume(ASSIGN)) {
			if (exprAssign()) {
				return 1;
			}
			else tkerr(crtTk, "expresie invalida dupa =");
		}
		crtTk = start;
	}
	if (exprOr()) {
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprOrPrim()
{
	printf("@exprOrPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(OR))
	{
		if (exprAnd())
		{
			if (exprOrPrim())
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprOr()
{
	printf("@exprOr %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprAnd())
	{
		if (exprOrPrim())
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprAndPrim()
{
	printf("@exprAndPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(AND))
	{
		if (exprEq())
		{
			if (exprAndPrim())
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprAnd()
{
	printf("@exprAnd %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprEq())
	{
		if (exprAndPrim())
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprEqPrim()
{
	printf("@exprEqPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(EQUAL))
	{
		if (exprRel())
		{
			if (exprEqPrim())
				return 1;
		}
	}
	else if (consume(NOTEQ)) {
		if (exprRel())
		{
			if (exprEqPrim())
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprEq()
{
	printf("@exprEq %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprRel())
	{
		if (exprEqPrim())
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprRelPrim()
{
	printf("@exprRelPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(LESS))
	{
		if (exprAdd()) {
			if (exprRelPrim())
				return 1;
		}
	}
	else if (consume(LESSEQ)) {
		if (exprAdd()) {
			if (exprRelPrim())
				return 1;
		}
	}
	else if (consume(GREATER)) {
		if (exprAdd()) {
			if (exprRelPrim())
				return 1;
		}
	}
	else if (consume(GREATEREQ)) {
		if (exprAdd()) {
			if (exprRelPrim())
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprRel()
{
	printf("@exprRel %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprAdd())
	{
		if (exprRelPrim())
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprAddPrim()
{
	printf("@exprAddPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(ADD))
	{
		if (exprMul())
		{
			if (exprAddPrim())
				return 1;
		}
		else tkerr(crtTk, "expresie invalida dupa +\n");
	}
	else if (consume(SUB)) {
		if (exprMul())
		{
			if (exprAddPrim())
				return 1;
		}
		else tkerr(crtTk, "expresie invalida dupa -\n");
	}
	crtTk = start;
	return 1;
}
int exprAdd()
{
	printf("@exprAdd %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprMul())
	{
		if (exprAddPrim())
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprMulPrim()
{
	printf("@exprMulPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(MUL))
	{
		if (exprCast())
		{
			if (exprMulPrim())
				return 1;
		}
	}
	else if (consume(DIV)) {
		if (exprCast())
		{
			if (exprMulPrim())
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprMul()
{
	printf("@exprMul %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprCast())
	{
		if (exprMulPrim())
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprCast()//////////////////////////////////////////////////
{
	printf("@exprCast %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Type t;
	if (consume(LPAR))
	{
		if (typeName(&t)) {
			if (consume(RPAR)) {
				if (exprCast()) {
					return 1;
				}
			}
			else tkerr(crtTk, "cast gresit sau lipseste )\n");
		}
	}
	else if (exprUnary()) {
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprUnary()//////////////////////////////
{
	printf("@exprUnary %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(SUB)) {
		if (exprUnary()) {
			return 1;
		}
	}
	else if (consume(NOT)) {
		if (exprUnary()) {
			return 1;
		}
	}
	else if (exprPostfix()) {
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprPostfixPrim()
{
	printf("@exprPostfixPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(LBRACKET)) {
		if (expr()) {
			if (consume(RBRACKET)) {
				if (exprPostfixPrim()) {
					return 1;
				}
			}
			else tkerr(crtTk, "expresie invalida sau lipseste ]\n");
		}
	}
	else if (consume(DOT)) {
		if (consume(ID)) {
			if (exprPostfixPrim()) {
				return 1;
			}
		}
		else tkerr(crtTk, "lipseste numele campului dupa .\n");
	}
	crtTk = start;
	return 1;
}

int exprPrimary()////////////////////////
{
	printf("@exprPrimary %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (consume(ID)) {
		if (consume(LPAR)) {
			if (expr()) {
				for (;;) {
					if (consume(COMMA)) {
						if (expr()) {

						}
						else tkerr(crtTk, "expresie invalida dupa , sau lipseste expresia dupa ,\n");
					}
					else break;
				}
			}
			if (consume(RPAR)) {

			}
			else tkerr(crtTk, "expresie invalida sau lipseste )\n");
		}
		return 1;
	}
	else if (consume(CT_INT)) {
		return 1;
	}
	else if (consume(CT_REAL)) {
		return 1;
	}
	else if (consume(CT_CHAR)) {
		return 1;
	}
	else if (consume(CT_STRING)) {
		return 1;
	}
	else if (consume(LPAR)) {
		if (expr()) {
			if (consume(RPAR)) {
				return 1;
			}
			else tkerr(crtTk, "expresie invalida sau lipseste )\n");
		}

	}
	crtTk = start;
	return 0;
}

int exprPostfix()
{
	printf("@exprPostfix %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprPrimary()) {
		if (exprPostfixPrim()) {
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

void parser()
{
	//apelare analizor sintactic
	crtTk = tokens;
	if (unit())
	{
		printf("Sintaxa OK.");
	}
	else
	{
		tkerr(crtTk, "eroare de sintaxa sau lipseste END");//afiseaza si linia unde e eroarea
	}
}