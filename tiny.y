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
var_declaracao	: tipo_especificador ID SEMI	{ $$ = $1;
						  $2 = newExpKind(IdK);
						}
		| tipo_especificador ID LCOLCHETE NUM RCOLCHETE SEMI 	{ $$ = $1; }
		;
tipo_especificador 	: INT 	{$$ = newExpNode(IntK)}	
			| VOID	{$$ = newStmtNode(VoidK)}
			;
fun_declaracao	: tipo_especificador ID LPAREN params RPAREN composto_decl 	{ $$ = $1;
										  $$->child[0] = $2;
										  $$->child[1] = $4;
										  $$->child[2] = $6;
										}
		;
params	: param_lista 	{ $$ = $1; }
	| VOID		{ $$ = newExpKind(Voidk); }
	;
param_lista	: param_lista COMA param 	{  
                 				  YYSTYPE t = $1;
						   if (t != NULL)
						   { while (t->sibling != NULL)
							t = t->sibling;
						     t->sibling = $3;
						     $$ = $1; }
						     else $$ = $3;
						 }
		| param				{ $$ = $1;}
		;
param	: tipo_especificador ID 			{ $$ = $1; 
							  $$->child[0] = $2;							
							}
	| tipo_especificador ID LCOLCHETE RCOLCHETE 	{ $$ = $1; 
							  $$->child[0] = $2; 
							}
	;
composto_decl	: LCHAVE local_declaracoes statement_lista RCHAVE	{ YYSTYPE t = $2;
									   if (t != NULL)
									   { while (t->sibling != NULL)
										t = t->sibling;
									     t->sibling = $3;
									     $$ = $1; }
									     else $$ = $3;
									 }			
		;
local_declaracoes	: local_declaracaoes var_declaracao { $$ = }
			;
statement_lista 	: statement_lista statement 	{ $$ = }

compos_decl :   LCHAVOSO RCHAVOSO
                  { $$ = NULL;}
            |  LCHAVOSO local_decl RCHAVOSO
                  { $$ = $2;}
            | LCHAVOSO statem_lista RCHAVOSO
                  { $$ = $2;}
            | LCHAVOSO local_decl statem_lista RCHAVOSO
                 { $$ = $2;
                   $$->child[0] = $3;
                 }
            ;
local_decl : local_decl var_decl
{ YYSTYPE t = $1;
  if (t != NULL)
    { while (t->sibling != NULL)
      t = t->sibling;
      t->sibling = $2;
      $$ = $1; }
      else $$ = $2;
}
            ;
statem_lista : statem_lista statem
{ YYSTYPE t = $1;
  if (t != NULL)
    { while (t->sibling != NULL)
      t = t->sibling;
      t->sibling = $2;
      $$ = $1; }
      else $$ = $2;
}
			;
statement		: expressao_decl	{ $$ = $1; }
			| composto_decl		{ $$ = $1; }
			| selecao_decl		{ $$ = $1; }
			| iteracao_decl		{ $$ = $1; }
			| retorno_decl		{ $$ = $1; }
			;
expressao_decl		: expressao SEMI	{ $$ = $1; }
			| SEMI			{ $$ = newExpNode(OpK);
				  		  $$->attr.op = SEMI;
						}
			;

selecao_decl     : IF LPAREN expressao RPAREN statement
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            	| IF LPAREN expressao RPAREN statement ELSE statement
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                   $$->child[2] = $7;
                 }
            	;
iteracao_decl : WHILE LPAREN expressao RPAREN statement
                 { $$ = newStmtNode(WhileK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            ;
retorno_decl	: RETURN SEMI		{ $$ =  newStmtNode(ReturnK); }
		| RETURN expressao SEMI	{ $$ = $1; 
					  $$->child[0] = $1;
					}
		;
expressao	: var ASSIGN expressao	{ $$ = newExpNode(AssignK); 
					  $$->child[0] = $1;
					  $$->child[1] = $3;					
					}
		| simples_expressao	{ $$ = $1; }
		;
var	: ID					{ $$ = newExpNode(IdK);
                   				  $$->attr.name =
			                          	copyString(tokenString); }
	| ID LCOLCHETE expressao RCOLCHETE	{ $$ = $1; 
						  $$->child[0] = $3;
						}
	;
simples_expressao	: soma_expressao relacional soma_expressao	{ $$ = }
			| soma_expressao				{ $$ = $1; }
			;
relacional	: MENOROUIGUAL	{ $$ = newExpNode(OpK);
				  $$->attr.op = MENOROUIGUAL;
				}
		| LT		{ $$ = newExpNode(OpK);
				  $$->attr.op = LT;
				}
		| MT		{ $$ = newExpNode(OpK);
				  $$->attr.op = MT;
				}
		| MAIOROUIGUAL	{ $$ = newExpNode(OpK);
				  $$->attr.op = MAIOROUIGUAL;
				}	
		| ASSIGN	{ $$ = newExpNode(OpK);
				  $$->attr.op = ASSIGN;
				}
		| DIFERENT	{ $$ = newExpNode(OpK);
				  $$->attr.op = DIFERENT;
				}
		;
soma_expressao	: soma_expressao soma termo	{ $$ = $2; 
						  $$->child[0] = $1;
						  $$->child[1] = $3;
						}
		| termo				{ $$ = $1; }
		;
soma	: PLUS	{ $$ = newExpNode(OpK);
		  $$->attr.op = PLUS;}
	| MINUS	{ $$ = newExpNode(OpK);
		  $$->attr.op = MINUS;}
	;
termo	: termo mult fator	{ $$ = $2; 
				  $$->child[0] = $1;
				  $$->child[1] = $3;
				}
	| fator			{ $$ = $1; }
	;
mult	: TIMES	{ $$ = newExpNode(OpK);
		  $$->attr.op = TIMES;
		}
	| OVER	{ $$ = newExpNode(OpK);
		  $$->attr.op = OVER;
		}
	;
fator	: LPAREN expressao RPAREN	{ $$ = $1; }
	| var				{ $$ = $1; }
	| ativacao			{ $$ = $1; }
	| NUM				{ $$ = $1; }
	;
ativacao	: ID LPAREN arg_lista RPAREN	{ $$ = $1; 
						  $$->child[0] = $3; 
						}
		| ID LPAREN RPAREN		{ $$ = $1;}
		;	
arg_lista	: arg_lista COMA expressao	{ YYSTYPE t = $1;
						   if (t != NULL)
						   { while (t->sibling != NULL)
							t = t->sibling;
						     t->sibling = $3;
						     $$ = $1; }
						     else $$ = $3;
						}	
		| expressao			{ $$ = $1; }
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
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = LT;
                 }
            | simple_expressao EQ simple_expressao
                 { $$ = newExpNode(OpK);
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
            | ID { $$ = newExpNode(IdK);
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

