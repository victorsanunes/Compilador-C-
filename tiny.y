/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);


%}

%token IF THEN ELSE END WHILE VOID RETURN INT ID NUM 
%token ASSIGN LT MT EQ DIFERENT MENOROUIGUAL MAIOROUIGUAL LPAREN RPAREN LCOLCHETE RCOLCHETE LCHAVE RCHAVE SEMI PLUS MINUS TIMES OVER COMA ENDFILE
%token ERROR 

%% /* Grammar for C- */

programa     : declaracao_lista
                 { savedTree = $1;} 
            ;
declaracao_lista    : declaracao_lista declaracao
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | declaracao  { $$ = $1; }
            ;
declaracao	: var_declaracao	{ $$ = $1; }
		| fun_declaracao	{ $$ = $1; } 		
            	| error  		{ $$ = NULL; }
            	;
var_declaracao	: tipo_especificador ID SEMI 				{ }
		| tipo_especificador ID LCOLCHETE NUM RCOLCHETE SEMI 	{ }
		;
tipo_especificador 	: INT 	{$$ = }	
			| VOID	{$$ = }
			;
fun_declaracao	: tipo_especificador ID LPAREN params RPAREN composto_decl { }
		;
params	: param_lista 	{ $$ = }
	| VOID		{ $$ = }
	;
param_lista	: param_lista COMA param 	{$$ = }
		| param				{$$ = }
		;
param	: tipo_especificador ID 			{ $$ = }
	| tipo_especificador ID LCOLCHETE RCOLCHETE 	{ $$ = }
	;
composto_decl	: LCHAVE local_declaracoes statement_lista RCHAVE	{ $$ = }
		| LCHAVE local_declaracoes statement_lista  RCHAVE { $$ = 222222222222}				
;
local_declaracoes	: local_declaracaoes var_declaracao
			;
statement_lista 	: statement_lista statement
			;
statement		: expressao_decl
			| composto_decl
			| selecao_decl
			| iteracao_decl
			| retorno_decl
			;
expressao_decl		: expressao SEMI
			| SEMI
			;

selecao_decl     : IF LPAREN expressao RPAREN stmt_seq
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            	| IF LPAREN expressao RPAREN stmt_seq ELSE stmt_seq
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                   $$->child[2] = $7;
                 }
            	;
iteracao_decl : WHILE LPAREN expressao RPAREN statement
                 { $$ = newStmtNode(WhileK);
                   $$->child[0] = $2;
                   $$->child[1] = $4;
                 }
            ;
retorno_decl	: RETURN SEMI
		| RETURN expressao SEMI
		;
expressao	: var ASSIGN expressao
		| simples_expressao
		;
var	: ID
	| ID LCOLCHETE expressao RCOLCHETE
	;
simples_expressao	: soma_expressao relacional soma_expressao
			| soma_expressao
			;
relacional	: MENOROUIGUAL
		| LT
		| MT
		| MAIOROUIGUAL
		| ASSIGN
		| DIFERENT
		;
soma_expressao	: soma_expressao soma termo
		| termo
		;
soma	: PLUS
	| MINUS
	;
termo	: termo mult fator
	| fator
	;
mult	: TIMES
	| OVER
	;
fator	: LPAREN expressao RPAREN
	| var
	| ativacao
	| NUM
	;
ativacao	: ID LPAREN arg_lista RPAREN
		| ID LPAREN RPAREN
		;
arg_lista	: arg_lista COMA expressao
		| expressao
		;

assign_stmt : ID { savedName = copyString(tokenString);
                   savedLineNo = lineno; }
              ASSIGN expressao
                 { $$ = newStmtNode(AssignK);
                   $$->child[0] = $4;
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                 }
            ;
read_stmt   : READ ID
                 { $$ = newStmtNode(ReadK);
                   $$->attr.name =
                     copyString(tokenString);
                 }
            ;
write_stmt  : WRITE expressao
                 { $$ = newStmtNode(WriteK);
                   $$->child[0] = $2;
                 }
            ;
expressao         : simple_expressao LT simple_expressao 
                 { $$ = newexpressaoNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = LT;
                 }
            | simple_expressao EQ simple_expressao
                 { $$ = newexpressaoNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = EQ;
                 }
            | simple_expressao { $$ = $1; }
            ;
simple_expressao  : simple_expressao PLUS term 
                 { $$ = newexpressaoNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = PLUS;
                 }
            | simple_expressao MINUS term
                 { $$ = newexpressaoNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MINUS;
                 } 
            | term { $$ = $1; }
            ;
term        : term TIMES factor 
                 { $$ = newexpressaoNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = TIMES;
                 }
            | term OVER factor
                 { $$ = newexpressaoNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = OVER;
                 }
            | factor { $$ = $1; }
            ;
factor      : LPAREN expressao RPAREN
                 { $$ = $2; }
            | NUM
                 { $$ = newexpressaoNode(ConstK);
                   $$->attr.val = atoi(tokenString);
                 }
            | ID { $$ = newexpressaoNode(IdK);
                   $$->attr.name =
                         copyString(tokenString);
                 }
            | error { $$ = NULL; }
            ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

