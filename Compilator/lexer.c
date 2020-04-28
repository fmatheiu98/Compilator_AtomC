/*Compilator LFTC Matheiu Flavius-Dan, grupa 4.2.(luni 14-16)*/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>
#include<stdarg.h>
#include"main.h"


extern Token* tokens;
Token* lastToken;
int line = 1;
char* pch;

Token* addTk(int code)
{
	Token* tk;
	SAFEALLOC(tk, Token)
		tk->code = code;
	tk->line = line;
	tk->next = NULL;
	tk->text = NULL;
	if (lastToken) {
		lastToken->next = tk;
	}
	else {
		tokens = lastToken = tk;
	}
	lastToken = tk;
	return tk;
}

char* createString(char* start, char* end)
{
	char* t = (char*)malloc((end - start) * sizeof(char));
	if (t == NULL)
	{
		perror("Eroare createString");
		exit(11);
	}
	int tt = 0;
	for (int i = 0; i < end - start; i++)
	{
		if (start[i] == '\\' && start[i + 1] != '\0')
		{
			if (start[i + 1] == 't')
				t[tt++] = '\t';
			else if (start[i + 1] == '\n')
				t[tt++] = '\n';
			else if (start[i + 1] == '\r')
				t[tt++] = '\r';
			else t[tt++] = start[i + 1];
			i++;
		}
		else
		{

			t[tt++] = start[i];
		}
	}
	t[end - start] = '\0';
	printf("%s", t);
	printf("\n\n\n\n\n\n");
	return t;
}

int getNextTk()
{
	int s = 0, nCh;//state

	char* pStartCh = NULL;

	Token* tk;
	char* tx = (char*)malloc(1000 * sizeof(char));
	for (;;)
	{
		SAFEALLOC(tk, Token)
			tk->text = NULL;
		char ch = *pch;
		printf("#%d %c(%d)\n", s, ch, ch); //afisez fiecare tranzitie, pentru verificare(stare...car_curent)
		switch (s)
		{
		case 0: if (isalpha(ch) || ch == '_') { pStartCh = pch; s = 1; pch++; }
			  else if (ch == ' ' || ch == '\r' || ch == '\t') { pch++; }
			  else if (ch == '\n') { pch++; line++; }
			  else if (ch == '\'') { pStartCh = pch; s = 3; pch++; }
			  else if (ch == '"') { pStartCh = pch; s = 7; pch++; }
			  else if (ch == '0') { pStartCh = pch; s = 17; pch++; }
			  else if (ch != '0' && isdigit(ch)) { pStartCh = pch; s = 15; pch++; }
			  else if (ch == ',') { s = 28; pch++; }
			  else if (ch == ';') { s = 29; pch++; }
			  else if (ch == '(') { s = 30; pch++; }
			  else if (ch == ')') { s = 31; pch++; }
			  else if (ch == '[') { s = 32; pch++; }
			  else if (ch == ']') { s = 33; pch++; }
			  else if (ch == '{') { s = 34; pch++; }
			  else if (ch == '}') { s = 35; pch++; }
			  else if (ch == '+') { s = 36; pch++; }
			  else if (ch == '-') { s = 37; pch++; }
			  else if (ch == '*') { s = 38; pch++; }
			  else if (ch == '.') { s = 39; pch++; }
			  else if (ch == '&') { s = 40; pch++; }
			  else if (ch == '|') { s = 42; pch++; }
			  else if (ch == '!') { s = 44; pch++; }
			  else if (ch == '=') { s = 47; pch++; }
			  else if (ch == '<') { s = 50; pch++; }
			  else if (ch == '>') { s = 53; pch++; }
			  else if (ch == '/') { s = 11; pch++; }
			  else if (ch == '\0') { s = 57; pch++; }
			  else tkerr(tk, "caracter necunoscut");
			break;
		case 1: if (isalnum(ch) || ch == '_') { pch++; }
			  else { s = 2; }
			  break;
		case 2: nCh = pch - pStartCh;
			if (nCh == 5 && !memcmp(pStartCh, "break", 5)) tk = addTk(BREAK);
			else if (nCh == 4 && !memcmp(pStartCh, "char", 4)) tk = addTk(CHAR);
			else if (nCh == 6 && !memcmp(pStartCh, "double", 6)) tk = addTk(DOUBLE);
			else if (nCh == 4 && !memcmp(pStartCh, "else", 4)) tk = addTk(ELSE);
			else if (nCh == 3 && !memcmp(pStartCh, "for", 3)) tk = addTk(FOR);
			else if (nCh == 2 && !memcmp(pStartCh, "if", 2)) tk = addTk(IF);
			else if (nCh == 3 && !memcmp(pStartCh, "int", 3)) tk = addTk(INT);
			else if (nCh == 6 && !memcmp(pStartCh, "return", 6)) tk = addTk(RETURN);
			else if (nCh == 6 && !memcmp(pStartCh, "struct", 6)) tk = addTk(STRUCT);
			else if (nCh == 4 && !memcmp(pStartCh, "void", 4)) tk = addTk(VOID);
			else if (nCh == 5 && !memcmp(pStartCh, "while", 5)) tk = addTk(WHILE);
			else {
				tk->text = (char*)malloc(nCh * sizeof(char));
				tk = addTk(ID);
				tk->text = createString(pStartCh, pch);
			}
			return tk->code;
		case 3: if (ch == '\\') { s = 4; pch++; }
			  else if (ch != '\'' && ch != '\\') { s = 5; pch++; }
			  else tkerr(tk, "caracter necunoscut");
			break;
		case 4: if (ch == 'a' || ch == 'b' || ch == 'r' || ch == 't' || ch == 'f' || ch == 'n' || ch == 'v' || ch == '?' || ch == '\'' || ch == '"' || ch == '\\' || ch == '0') { s = 5; pch++; }
			  else tkerr(tk, "caracter necunoscut");
			break;
		case 5: if (ch == '\'') { s = 6; pch++; }
			  else tkerr(tk, "caracter necunoscut");
			break;
		case 6: nCh = pch - pStartCh;
			tk = addTk(CT_CHAR);
			char* caracter;
			caracter = createString(pStartCh + 1, pch - 1);
			tk->i = caracter[0];
			return CT_CHAR;
		case 7: if (ch == '\\') { s = 8; pch++; }
			  else if (ch != '"' && ch != '\\') { s = 9; pch++; }
			  else { s = 9; }
			  break;
		case 8: if (ch == 'a' || ch == 'b' || ch == 'r' || ch == 't' || ch == 'f' || ch == 'n' || ch == 'v' || ch == '?' || ch == '\'' || ch == '"' || ch == '\\' || ch == '0') { s = 9; pch++; }
			  else tkerr(tk, "caracter necunoscut");
			break;
		case 9: if (ch == '"') { s = 10; pch++; }
			  else { s = 7; }
			  break;
		case 10: nCh = pch - pStartCh;
			tk->text = (char*)malloc((nCh - 1) * sizeof(char));
			tk = addTk(CT_STRING);
			//strcpy(tx, "");
			//strcpy(tk->text, "");
			tk->text = createString(pStartCh + 1, pch - 1);
			tk->text[strlen(tk->text) - 4] = '\0';
			return tk->code;
		case 11: if (ch == '/') { s = 12; pch++; }
			   else if (ch == '*') { s = 13; pch++; }
			   else { s = 56; }
			   break;
		case 12: if (ch != '\n' && ch != '\r' && ch != '\0') { pch++; }
			   else { s = 0; }
			   break;
		case 13: if (ch == '*') { s = 14; pch++; }
			   else if (ch != '*' && ch == '\n') { pch++; line++; }
			   else if (ch != '*' && ch != '\n') { pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 14: if (ch == '*') { pch++; }
			   else if (ch != '*' && ch != '/') { s = 13; pch++; }
			   else if (ch == '/') { s = 0; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 15: if (isdigit(ch)) { pch++; }
			   else if (ch == '.') { s = 22; pch++; }
			   else if (ch == 'e' || ch == 'E') { s = 24; pch++; }
			   else { s = 16; }
			   break;
		case 16: nCh = pch - pStartCh;
			tk = addTk(CT_INT);
			//tk->i = atoi(createString(pStartCh, pch));
			char* nr;
			nr = (char*)malloc(255 * sizeof(char));
			char* ptr;
			nr = createString(pStartCh, pch);
			if (nr[0] == '0' && nr[1] != 'x')
			{
				tk->are_octal = 1;
				tk->i = strtol(nr, &ptr, 8);
			}
			else
				if (nr[0] == '0' && nr[1] == 'x')
				{
					tk->are_hexa = 1;
					tk->i = strtol(nr, &ptr, 16);
					//printf("\n\n\n%#x\n\n\n", tk->i);
				}
				else
				{
					tk->i = strtol(nr, &ptr, 10);
				}
			return CT_INT;
		case 17: if (ch == 'x') { s = 18; pch++; }
			   else { s = 19; }
			   break;
		case 18: if (isdigit(ch) || (ch >= 'a' && ch <= 'f') || ((ch >= 'A' && ch <= 'F'))) { s = 21; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 19: if (ch == '8' || ch == '9') { s = 20; pch++; }
			   else if (ch >= '0' && ch <= '7') { pch++; }
			   else if (ch == 'e' || ch == 'E') { s = 24; pch++; }
			   else if (ch == '.') { s = 22; pch++; }
			   else { s = 16; }
			   break;
		case 20: if (isdigit(ch)) { pch++; }
			   else if (ch == 'e' || ch == 'E') { s = 24; pch++; }
			   else if (ch == '.') { s = 22; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 21: if (isdigit(ch) || (ch >= 'a' && ch <= 'f') || ((ch >= 'A' && ch <= 'F'))) { pch++; }
			   else { s = 16; }
			   break;
		case 22: if (isdigit(ch)) { s = 23; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 23: if (isdigit(ch)) { pch++; }
			   else if (ch == 'e' || ch == 'E') { s = 24; pch++; }
			   else { s = 27; }
			   break;
		case 24: if (ch == '+' || ch == '-') { s = 25; pch++; }
			   else { s = 25; }
			   break;
		case 25: if (isdigit(ch)) { s = 26; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 26: if (isdigit(ch)) { pch++; }
			   else { s = 27; }
			   break;
		case 27: nCh = pch - pStartCh;
			tk = addTk(CT_REAL);
			tk->r = atof(createString(pStartCh, pch));
			if (strchr(createString(pStartCh, pch), 'e') || strchr(createString(pStartCh, pch), 'E'))
				tk->are_exp = 1;
			return CT_REAL;
		case 28: addTk(COMMA);
			return COMMA;
		case 29: addTk(SEMICOLON);
			return SEMICOLON;
		case 30: addTk(LPAR);
			return LPAR;
		case 31: addTk(RPAR);
			return RPAR;
		case 32: addTk(LBRACKET);
			return LBRACKET;
		case 33: addTk(RBRACKET);
			return RBRACKET;
		case 34: addTk(LACC);
			return LACC;
		case 35: addTk(RACC);
			return RACC;
		case 36: addTk(ADD);
			return ADD;
		case 37: addTk(SUB);
			return SUB;
		case 38: addTk(MUL);
			return MUL;
		case 39: addTk(DOT);
			return DOT;
		case 40: if (ch == '&') { s = 41; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 41: addTk(AND);
			return AND;
		case 42: if (ch == '|') { s = 43; pch++; }
			   else tkerr(tk, "caracter necunoscut");
			break;
		case 43: addTk(OR);
			return OR;
		case 44: if (ch == '=') { s = 45; pch++; }
			   else { s = 46; }
			   break;
		case 45: addTk(NOTEQ);
			return NOTEQ;
		case 46: addTk(NOT);
			return NOT;
		case 47: if (ch == '=') { s = 48; pch++; }
			   else { s = 49; }
			   break;
		case 48: addTk(EQUAL);
			return EQUAL;
		case 49: addTk(ASSIGN);
			return ASSIGN;
		case 50: if (ch == '=') { s = 52; pch++; }
			   else { s = 51; }
			   break;
		case 51: addTk(LESS);
			return LESS;
		case 52: addTk(LESSEQ);
			return LESSEQ;
		case 53: if (ch == '=') { s = 55; pch++; }
			   else { s = 54; }
			   break;
		case 54: addTk(GREATER);
			return GREATER;
		case 55: addTk(GREATEREQ);
			return GREATEREQ;
		case 56: addTk(DIV);
			return DIV;
		case 57: addTk(END);
			return END;
		default: tkerr(tk, "Stare invalida %d", s);
			break;
		}
	}
}

void lexer(char *buf)
{
	SAFEALLOC(tokens, Token)
		SAFEALLOC(lastToken, Token)
		tokens = lastToken = NULL;
	pch = buf;

	while (getNextTk() != END)
	{

	}
}

void afisare_atomi() {

	//afisare atomi -> linie :: atom:<nume_id>/<valoare constante>

	for (Token* t1 = tokens; t1 != NULL; t1 = t1->next)
	{
		printf("%d :: ", t1->line);
		switch (t1->code)
		{
		case ID: printf("ID");
			printf(":%s ", t1->text);
			break;
		case BREAK: printf("BREAK"); break;
		case CHAR: printf("CHAR"); break;
		case DOUBLE: printf("DOUBLE"); break;
		case ELSE: printf("ELSE"); break;
		case FOR: printf("FOR"); break;
		case IF: printf("IF"); break;
		case INT: printf("INT"); break;
		case RETURN: printf("RETURN"); break;
		case STRUCT: printf("STRUCT"); break;
		case VOID: printf("VOID"); break;
		case WHILE: printf("WHILE"); break;
		case CT_INT: printf("CT_INT");
			if (t1->are_hexa == 1)
				//printf(":%#x ", t1->i);//afisare in format hexa
				printf(":%d ", t1->i);
			else if (t1->are_octal == 1)
				//printf(":%#o ", t1->i);//afisare in format octal
				printf(":%d ", t1->i);
			else
				printf(":%d ", t1->i);
			break;
		case CT_REAL: printf("CT_REAL");
			if (t1->are_exp == 1)
				//printf(":%e ", t1->r);//afisare in format stiintific
				printf(":%lf ", t1->r);
			else
				printf(":%lf ", t1->r);
			break;
		case CT_CHAR: printf("CT_CHAR"); printf(":%c ", t1->i); break;
		case CT_STRING: printf("CT_STRING"); printf(":%s ", t1->text); break;
		case COMMA: printf("COMMA"); break;
		case SEMICOLON: printf("SEMICOLON"); break;
		case LPAR: printf("LPAR"); break;
		case RPAR: printf("RPAR"); break;
		case LBRACKET: printf("LBRACKET"); break;
		case RBRACKET: printf("RBRACKET"); break;
		case LACC: printf("LACC"); break;
		case RACC: printf("RACC"); break;
		case ADD: printf("ADD"); break;
		case SUB: printf("SUB"); break;
		case MUL: printf("MUL"); break;
		case DIV: printf("DIV"); break;
		case DOT: printf("DOT"); break;
		case AND: printf("AND"); break;
		case OR: printf("OR"); break;
		case NOT: printf("NOT"); break;
		case ASSIGN: printf("ASSIGN"); break;
		case EQUAL: printf("EQUAL"); break;
		case NOTEQ: printf("NOTEQ"); break;
		case LESS: printf("LESS"); break;
		case LESSEQ: printf("LESSEQ"); break;
		case GREATER: printf("GREATER"); break;
		case GREATEREQ: printf("GREATEREQ"); break;
		case END: printf("END"); break;
		}
		printf(" \n");
	}
}