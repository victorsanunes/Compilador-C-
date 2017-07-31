/****************************************************/
/* File: cmenos.l                                   */
/* Lex specification for Cmenos                     */
/****************************************************/

%{

#include "globals.h"
#include "scan.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmenos.tab.h"


char tokenString[MAXTOKENLEN+1];
extern YYSTYPE yylval;
void erroLexico(char *);
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z0-9]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}   
"="             {return EQ;}
","             {return VIRG;}
"=="            {return EQEQ;}
"<="            {return MINUSEQ;}
">="            {return PLUSEQ;}
"!="            {return NOTEQ;}
"<"             {return LT;}
">"             {return RT;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return OPENCOL;}
"]"             {return CLOSECOL;}
"{"             {return OPENKEY;}
"}"             {return CLOSEKEY;}
";"             {return SEMI;}

{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"             { char c;
		   int verifica = 0;
                  do
                  { c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
		    if (c == '*'){
			c = input ();
			if (c == '/'){
			   verifica = 1;
				}
    		   }	
                  } while (!verifica);
		  
                }
"/"             {return OVER;}
.               {return ERROR;}

%%


TokenType getToken(void){
  static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime){
    firstTime = FALSE;
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
