%{
#include "cminus.tab.h"
#include "string.h"
//#include "tabsimbolos.h"
#include "global.h"
#include "scan.h"

char tokenString[MAXTOKENLEN+1];

%}

digito      [0-9]
NUM         {digito}+
letra       [a-zA-Z]
ID          {letra}+
newline     \n
whitespace  [ \t]+

%%

"else"          { return ELSE; }
"if"            { return IF; }
"int"           { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return OVER; }
"<"             { return LT; }
"<="            { return MENOROUIGUAL; }
">"             { return MT; }
">="            { return MAIOROUIGUAL; }
"=="            { return EQ; }
"!="            { return DIFERENT; }
"="             { return ASSIGN; }
";"             { return SEMI; }
","             { return COMA; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LCOLCHETE; }
"]"             { return RCOLCHETE; }
"{"             { return LCHAVE; }
"}"             { return RCHAVE; }

{NUM}           { return NUM; }
{ID}            { return ID; }
{newline}       { lineno ++; }
{whitespace}    {/* ignora espacos em branco */}

"/*"             { int e=0,c;
		  do{
			c = input();
			if ((c == '/') && (e == 1)){
				e=2;
			}else if (c == EOF)
				 break;

			else if (c == '\n')
				 lineno++;
			
			else if (c == '*') 
				e=1; 
			else 
				e=0;			
		 } while (e < 2);
                }
.               { return ERROR; }

%%

TokenType getToken(void){

	static int firstTime = TRUE;
	TokenType currentToken;

	if (firstTime){
		firstTime = FALSE;
		lineno++;
		yyin = source;
	}

	currentToken = yylex();
	strncpy(tokenString,yytext,MAXTOKENLEN);
	return currentToken;
}
