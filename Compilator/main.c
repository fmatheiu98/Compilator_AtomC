/*Compilator LFTC Matheiu Flavius-Dan, grupa 4.2.(luni 14-16)*/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>
#include<stdarg.h>
#include "main.h"

Token* tokens;
Symbols symbols;
int crtDepth;// ("adancimea" contextului curent, initial 0)
Symbol* crtFunc;// (pointer la simbolul functiei daca in functie, altfel NULL)
Symbol* crtStruct;// (pointer la simbolul structurii daca in structura, altfel NULL)

Token* init(Token* tk)
{
	tk->code = -99;
	tk->text = (char*)malloc(sizeof(char));
	strcpy(tk->text, "");
	tk->line = -99;
	tk->next = NULL;
	return tk;
}

void tkerr(const Token* tk, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "error in line %d: ", tk->line);
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}

void err(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}

int main()
{
	char* buf = (char*)malloc(30000 * sizeof(char));
	if (buf == NULL)
	{
		perror("");
		exit(11);
	}
	FILE* fis = fopen("code.txt", "rt");
	if (fis == NULL)
	{
		perror("Eroare deschidere fisier.");
		exit(1);
	}
	int n = fread(buf, 1, 30000, fis);
	if (n < 0)
	{
		printf("Eroare citire.");
		exit(2);
	}
	buf[n] = '\0';
	fclose(fis);

	lexer(buf);
	afisare_atomi();
	parser();
	
	return 0;
}