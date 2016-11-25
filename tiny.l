/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
static int yylex(void);
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"else"          {return ELSE;}
"if"            {return IF;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}
"=="            {return EQ;}
"="            	{return ASSIGN;}
"!="            {return DIFERENT;}
"<"             {return LT;}
"<="           	{return MENOROUIGUAL;}
">"             {return MT;}
">="            {return MAIOROUIGUAL;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LCOLCHETE;}
"]"             {return RCOLCHETE;}
"{"             {return LCHAVE;}
"}"             {return RCHAVE;}
";"             {return SEMI;}
","             {return COMA;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}

{whitespace}    {/* skip whitespace */}
"/*"             { char c;
		int check = 0;

                  do
                  { c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
			if (c == '*')
				{
					c = input();
					if (c == '/')
						check = 1;
				}
			} while (!check);
		
		}
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

