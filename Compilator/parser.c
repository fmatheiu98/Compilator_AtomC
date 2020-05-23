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
	if (!symbols)
		return NULL;
	int contor = symbols->end - symbols->begin;
	
	for (int i=contor-1;i>=0;i--)
	{
		if (symbols->begin[i] != NULL)
		{
			if (symbols->begin[i]->name != NULL && name!=NULL && strcmp(symbols->begin[i]->name, name) == 0)
				return symbols->begin[i];
		}
	}
	return NULL;
}

Symbol* requireSymbol(Symbols* symbols, const char* name)
{
	if (!symbols)
		err("Symbol not found in TS.");
	int contor = symbols->end - symbols->begin;

	for (int i = contor - 1; i >= 0; i--)
	{
		if (symbols->begin[i] != NULL)
		{
			if (symbols->begin[i]->name != NULL && name != NULL && strcmp(symbols->begin[i]->name, name) == 0)
				return symbols->begin[i];
		}
	}
	err("Symbol not found in TS.");
}

void deleteSymbolsAfter(Symbols *symbols, Symbol *start)
{
	int contor = symbols->end - symbols->begin;

	for (int i = 0; i < contor; i++)
	{
		if (symbols->begin[i] != NULL)
		{
			if (symbols->begin[i] == start)
			{
				i++;
				for (int j = i; j < contor; j++)
				{
					symbols->begin[j] = NULL;
				}
				return;
			}
		}
	}
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

Type createType(int typeBase, int nElements)
{
	Type t;
	t.typeBase = typeBase;
	t.nElements = nElements;
	return t;
}

void cast(Type* dst, Type* src)
{
	if (src->nElements > -1) {
		if (dst->nElements > -1) {
			if (src->typeBase != dst->typeBase)
				tkerr(crtTk, "an array cannot be converted to an array of another type");
		}
		else {
			tkerr(crtTk, "an array cannot be converted to a non-array");
		}
	}
	else {
		if (dst->nElements > -1) {
			tkerr(crtTk, "a non-array cannot be converted to an array");
		}
	}
	switch (src->typeBase) {
	case TB_CHAR:
	case TB_INT:
	case TB_DOUBLE:
			switch (dst->typeBase) {
			case TB_CHAR:
			case TB_INT:
			case TB_DOUBLE:
				return;
			}
	case TB_STRUCT:
		if (dst->typeBase == TB_STRUCT) {
			if (src->s != dst->s)
				tkerr(crtTk, "a structure cannot be converted to another one");
			return;
		}
	}
	tkerr(crtTk, "incompatible types");
}

Type getArithType(Type* s1, Type* s2)
{
	switch (s1->typeBase)
	{
		case TB_CHAR:
			switch (s2->typeBase)
			{
			case TB_CHAR:
				return *s2;
				break;
			case TB_INT:
				return *s2;
				break;
			case TB_DOUBLE:
				return *s2;
				break;
			}
			break;
		case TB_DOUBLE:
			switch (s2->typeBase)
			{
			case TB_CHAR:
				return *s1;
				break;
			case TB_INT:
				return *s1;
				break;
			case TB_DOUBLE:
				return *s1;
				break;
			}
			break;
		case TB_INT:
			switch (s2->typeBase)
			{
			case TB_CHAR:
				return *s1;
				break;
			case TB_INT:
				return *s1;
				break;
			case TB_DOUBLE:
				return *s2;
				break;
			}
			break;
	}
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
int expr(RetVal *rv);
int exprAssign(RetVal* rv);
int exprOr(RetVal* rv);
int exprAnd(RetVal* rv);
int exprEq(RetVal* rv);
int exprRel(RetVal* rv);
int exprAdd(RetVal* rv);
int exprMul(RetVal* rv);
int exprCast(RetVal* rv);
int exprUnary(RetVal* rv);
int exprPostfix(RetVal* rv);
int exprPrimary(RetVal* rv);

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
	Token* tkName;
	if (typeBase(&t)) {
		if (consume(ID)) {
			tkName = consumedTk;
			isDV = arrayDecl(&t);//optional
			if (!isDV) {
				t.nElements = -1;
			}
			
			addVar(tkName, &t);
			for (;;) {
				if (consume(COMMA)) {
					isDV = 1;
					if (consume(ID)) {
						tkName = consumedTk;
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
	RetVal rv;
	if (consume(LBRACKET))
	{
		if (expr(&rv)) {
			if (!rv.isCtVal)tkerr(crtTk, "the array size is not a constant");
			if (rv.type.typeBase != TB_INT)tkerr(crtTk, "the array size is not an integer");
			ret->nElements = rv.ctVal.i;
		}
		ret->nElements = 0;       // for now do not compute the real size
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
	RetVal rv,rv1,rv2,rv3;
	if (stmCompound()) {
		return 1;
	}
	else
		if (consume(IF)) {
			if (consume(LPAR)) {
				if (expr(&rv)) {
					if (rv.type.typeBase == TB_STRUCT)
						tkerr(crtTk, "a structure cannot be logically tested");
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
					if (expr(&rv)) {
						if (rv.type.typeBase == TB_STRUCT)
							tkerr(crtTk, "a structure cannot be logically tested");
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
						expr(&rv1);
						if (consume(SEMICOLON)) {
							if (expr(&rv2))
							{
								if (rv2.type.typeBase == TB_STRUCT)
									tkerr(crtTk, "a structure cannot be logically tested");
							}
							if (consume(SEMICOLON)) {
								expr(&rv3);
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
							if (expr(&rv))
							{
								if (crtFunc->type.typeBase == TB_VOID)
									tkerr(crtTk, "a void function cannot return a value");
								cast(&crtFunc->type, &rv.type);
							}
							if (consume(SEMICOLON)) {
								return 1;
							}
							else tkerr(crtTk, "lipseste ; sau expresie invalida\n");
						}
						else
						{
							expr(&rv);
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

int expr(RetVal *rv)
{
	printf("@expr %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprAssign(rv))
	{
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprAssign(RetVal *rv)////////////////////////////////////////////////
{
	printf("@exprAssign %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	RetVal rve;
 	if (exprUnary(rv))
	{
		if (consume(ASSIGN)) {
			if (exprAssign(&rve)) {
				if (!rv->isLVal)tkerr(crtTk, "cannot assign to a non-lval");
				if (rv->type.nElements > -1 || rve.type.nElements > -1)
					tkerr(crtTk, "the arrays cannot be assigned");
				cast(&rv->type, &rve.type);
				rv->isCtVal = rv->isLVal = 0;
				return 1;
			}
			else tkerr(crtTk, "expresie invalida dupa =");
		}
		crtTk = start;
	}
	if (exprOr(rv)) {
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprOrPrim(RetVal* rv)
{
	printf("@exprOrPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	RetVal rve;
	if (consume(OR))
	{
		if (exprAnd(&rve))
		{
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(crtTk, "a structure cannot be logically tested");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprOrPrim(rv))
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprOr(RetVal *rv)
{
	printf("@exprOr %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprAnd(rv))
	{
		if (exprOrPrim(rv))
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprAndPrim(RetVal *rv)
{
	printf("@exprAndPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	RetVal rve;
	if (consume(AND))
	{
		if (exprEq(&rve))
		{
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(crtTk, "a structure cannot be logically tested");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprAndPrim(rv))
				return 1;
		}
	}
	crtTk = start;
	return 1;
}
int exprAnd(RetVal* rv)
{
	printf("@exprAnd %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprEq(rv))
	{
		if (exprAndPrim(rv))
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprEqPrim(RetVal *rv)
{
	printf("@exprEqPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Token *tkop;
	RetVal rve;
	if (consume(EQUAL) || consume(NOTEQ))
	{
		tkop = consumedTk;
		if (exprRel(&rve))
		{
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(crtTk, "a structure cannot be compared");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprEqPrim(rv))
				return 1;
		}
	}
	
	crtTk = start;
	return 1;
}
int exprEq(RetVal *rv)
{
	printf("@exprEq %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprRel(rv))
	{
		if (exprEqPrim(rv))
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprRelPrim(RetVal *rv)
{
	printf("@exprRelPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Token* tkop;
	RetVal rve;
	if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
	{
		tkop = consumedTk;
		if (exprAdd(&rve)) {
			if (rv->type.nElements > -1 || rve.type.nElements > -1)
				tkerr(crtTk, "an array cannot be compared");
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(crtTk, "a structure cannot be compared");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprRelPrim(rv))
				return 1;
		}
	}
	
	crtTk = start;
	return 1;
}
int exprRel(RetVal* rv)
{
	printf("@exprRel %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprAdd(rv))
	{
		if (exprRelPrim(rv))
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprAddPrim(RetVal* rv)
{
	printf("@exprAddPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Token* tkop;
	RetVal rve;
	if (consume(ADD) || consume(SUB))
	{
		tkop = consumedTk;
		if (exprMul(&rve))
		{
			if (rv->type.nElements > -1 || rve.type.nElements > -1)
				tkerr(crtTk, "an array cannot be added or subtracted");
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(crtTk, "a structure cannot be added or subtracted");
			rv->type = getArithType(&rv->type, &rve.type);
			rv->isCtVal = rv->isLVal = 0;
			//rv contine rezultatul operatiei de pana acum
			if (exprAddPrim(rv))
				return 1;
		}
		else tkerr(crtTk, "expresie invalida dupa + sau -\n");
	}

	crtTk = start;
	return 1;
}
int exprAdd(RetVal *rv)
{
	printf("@exprAdd %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprMul(rv))
	{
		if (exprAddPrim(rv))
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprMulPrim(RetVal *rv)
{
	printf("@exprMulPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Token* tkop;
	RetVal rve;
	if (consume(MUL) || consume(DIV))
	{
		tkop = consumedTk;
		if (exprCast(&rve))
		{
			if (rv->type.nElements > -1 || rve.type.nElements > -1)
				tkerr(crtTk, "an array cannot be multiplied or divided");
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(crtTk, "a structure cannot be multiplied or divided");
			rv->type = getArithType(&rv->type, &rve.type);
			rv->isCtVal = rv->isLVal = 0;
			if (exprMulPrim(rv))
				return 1;
		}
	}
	
	crtTk = start;
	return 1;
}
int exprMul(RetVal* rv)
{
	printf("@exprMul %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprCast(rv))
	{
		if (exprMulPrim(rv))
		{
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprCast(RetVal *rv)//////////////////////////////////////////////////
{
	printf("@exprCast %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Type t;
	RetVal rve;
	if (consume(LPAR))
	{
		if (typeName(&t)) {
			if (consume(RPAR)) {
				if (exprCast(&rve)) {
					cast(&t, &rve.type);
					rv->type = t;
					rv->isCtVal = rv->isLVal = 0;
					return 1;
				}
			}
			else tkerr(crtTk, "cast gresit sau lipseste )\n");
		}
	}
	else if (exprUnary(rv)) {
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprUnary(RetVal *rv)//////////////////////////////
{
	printf("@exprUnary %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Token* tkop;
	if (consume(SUB) || consume(NOT)) {
		tkop = consumedTk;
		if (exprUnary(rv)) {
			if (tkop->code == SUB) {
				if (rv->type.nElements >= 0)tkerr(crtTk, "unary '-' cannot be applied to an array");
				if (rv->type.typeBase == TB_STRUCT)
					tkerr(crtTk, "unary '-' cannot be applied to a struct");
			}
			else {  // NOT
				if (rv->type.typeBase == TB_STRUCT)tkerr(crtTk, "'!' cannot be applied to a struct");
				rv->type = createType(TB_INT, -1);
			}
			rv->isCtVal = rv->isLVal = 0;
			return 1;
		}
	}
	else if (exprPostfix(rv)) {
		return 1;
	}
	crtTk = start;
	return 0;
}

int exprPostfixPrim(RetVal* rv)
{
	printf("@exprPostfixPrim %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	RetVal rve;
	Token* tkName;
	if (consume(LBRACKET)) {
		if (expr(&rve)) {
			if (rv->type.nElements < 0)tkerr(crtTk, "only an array can be indexed");
			Type typeInt = createType(TB_INT, -1);
			cast(&typeInt, &rve.type);
			rv->type = rv->type;
			rv->type.nElements = -1;
			rv->isLVal = 1;
			rv->isCtVal = 0;
			if (consume(RBRACKET)) {
				if (exprPostfixPrim(rv)) {
					return 1;
				}
			}
			else tkerr(crtTk, "expresie invalida sau lipseste ]\n");
		}
	}
	else if (consume(DOT)) {
		if (consume(ID)) {
			tkName = consumedTk;
			Symbol* sStruct = rv->type.s;
			Symbol* sMember = findSymbol(&sStruct->members, tkName->text);
			if (!sMember)
				tkerr(crtTk, "struct %s does not have a member %s", sStruct->name, tkName->text);
			rv->type = sMember->type;
			rv->isLVal = 1;
			rv->isCtVal = 0;
			if (exprPostfixPrim(rv)) {
				return 1;
			}
		}
		else tkerr(crtTk, "lipseste numele campului dupa .\n");
	}
	crtTk = start;
	return 1;
}

int exprPostfix(RetVal *rv)
{
	printf("@exprPostfix %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	if (exprPrimary(rv)) {
		if (exprPostfixPrim(rv)) {
			return 1;
		}
	}
	crtTk = start;
	return 0;
}

int exprPrimary(RetVal *rv)////////////////////////
{
	printf("@exprPrimary %s\n", codeName(crtTk->code));
	Token* start = crtTk;
	Token* tkName;
	RetVal arg;
	if (consume(ID)) {
		tkName = consumedTk;
		Symbol* s = findSymbol(&symbols, tkName->text);
		if (!s)tkerr(crtTk, "undefined symbol %s", tkName->text);
		rv->type = s->type;
		rv->isCtVal = 0;
		rv->isLVal = 1;
		tkName = consumedTk;
		if (consume(LPAR)) {
			Symbol** crtDefArg = s->args.begin;
			if (s->cls != CLS_FUNC && s->cls != CLS_EXTFUNC)
				tkerr(crtTk, "call of the non-function %s", tkName->text);
			if (expr(&arg)) {
				if (crtDefArg == s->args.end)tkerr(crtTk, "too many arguments in call");
				cast(&(*crtDefArg)->type, &arg.type);
				crtDefArg++;
				for (;;) {
					if (consume(COMMA)) {
						if (expr(&arg)) {
							if (crtDefArg == s->args.end)tkerr(crtTk, "too many arguments in call");
							cast(&(*crtDefArg)->type, &arg.type);
							crtDefArg++;
						}
						else tkerr(crtTk, "expresie invalida dupa , sau lipseste expresia dupa ,\n");
					}
					else break;
				}
			}
			if (consume(RPAR)) {
				if (crtDefArg != s->args.end)tkerr(crtTk, "too few arguments in call");
				rv->type = s->type;
				rv->isCtVal = rv->isLVal = 0;
			}
			else
				if (s->cls == CLS_FUNC || s->cls == CLS_EXTFUNC)
					tkerr(crtTk, "missing call for function %s", tkName->text);
			//else tkerr(crtTk, "expresie invalida sau lipseste )\n");
		}
		return 1;
	}
	else if (consume(CT_INT)) {
		Token* tki;
		tki = consumedTk;
		rv->type = createType(TB_INT, -1); rv->ctVal.i = tki->i;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(CT_REAL)) {
		Token* tkr;
		tkr = consumedTk;
		rv->type = createType(TB_DOUBLE, -1); rv->ctVal.d = tkr->r;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(CT_CHAR)) {
		Token* tkc;
		tkc = consumedTk;
		rv->type = createType(TB_CHAR, -1); rv->ctVal.i = tkc->i;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(CT_STRING)) {
		Token* tks;
		tks = consumedTk;
		rv->type = createType(TB_CHAR, 0); rv->ctVal.str = tks->text;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(LPAR)) {
		if (expr(rv)) {
			if (consume(RPAR)) {
				return 1;
			}
			else tkerr(crtTk, "expresie invalida sau lipseste )\n");
		}

	}
	crtTk = start;
	return 0;
}

Symbol* addExtFunc(const char* name, Type type,void *addr)
{
	Symbol* s = addSymbol(&symbols, name, CLS_EXTFUNC);
	s->type = type;
	s->addr = addr;
	initSymbols(&s->args);
	return s;
}
Symbol* addFuncArg(Symbol* func, const char* name, Type type)
{
	Symbol* a = addSymbol(&func->args, name, CLS_VAR);
	a->type = type;
	return a;
}

void put_i()
{
	printf("#%d\n", popi());
}void put_d()
{
	printf("#%lf\n", popd());
}void put_c()
{
	printf("#%c\n", popc());
}

void addExtFuncs()
{
	Symbol* s;
	//s = addExtFunc("put_s", createType(TB_VOID, -1));
	//addFuncArg(s, "s", createType(TB_CHAR, 0));

	//s = addExtFunc("get_s", createType(TB_VOID, -1));
	//addFuncArg(s, "s", createType(TB_CHAR, 0));

	s = addExtFunc("put_i", createType(TB_VOID, -1),put_i);
	addFuncArg(s, "i", createType(TB_INT, -1));

	//s = addExtFunc("get_i", createType(TB_INT, -1));

	s = addExtFunc("put_d", createType(TB_VOID, -1),put_d);
	addFuncArg(s, "d", createType(TB_DOUBLE, -1));

	//s = addExtFunc("get_d", createType(TB_DOUBLE, -1));

	s = addExtFunc("put_c", createType(TB_VOID, -1),put_c);
	addFuncArg(s, "c", createType(TB_CHAR, -1));

	//s = addExtFunc("get_c", createType(TB_CHAR, -1));

	//s = addExtFunc("seconds", createType(TB_DOUBLE, -1));
}

void parser()
{
	addExtFuncs();
	//apelare analizor sintactic
	crtTk = tokens;
	if (unit())
	{
		printf("Sintaxa OK.\n\n");
	}
	else
	{
		tkerr(crtTk, "eroare de sintaxa sau lipseste END");//afiseaza si linia unde e eroarea
	}
}