%{
#define YYPARSER 
#include "stdio.h"
#include "global.h"
#include "scan.h"
#include "util.h"
#include "parse.h"

#define YYDEBUG 1
#define YYSTYPE TreeNode *

static TreeNode * savedTree;

void yyerror(char *);
static int yylex(void);


%}

%token ERROR 
%token ID NUM 
%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES OVER LT MENOROUIGUAL MT MAIOROUIGUAL EQ DIFERENT ASSIGN SEMI COMA LPAREN RPAREN LCOLCHETE RCOLCHETE LCHAVE RCHAVE



%%
programa    		: declaracao_lista
			{ savedTree = $1; }
			;
			
declaracao_lista    	: declaracao_lista declaracao
			{
				TreeNode * t = $1;
				if( t!= NULL){
					while( t->sibling != NULL )
						t = t->sibling;
					t->sibling=$2;
					$$ = $1;
				}
				else $$ = $2;
			}
			| declaracao
			{ $$ = $1; }
			;
			
declaracao	        : var_declaracao
			{ $$ = $1; }
			| fun_declaracao
			{ $$ = $1; }
			;
			
var_declaracao		: tipo_especificador id SEMI
			{
				$$ = newDeclNode(Var); 
				$$->type = $1->type;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
			}			
			| tipo_especificador id LCOLCHETE NUM RCOLCHETE SEMI
			{
				$$ = newDeclNode(Var);
				$$->type = $1->type;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
			}
			;

tipo_especificador	: INT
			{ 
				$$ = newDeclNode(Var);
				$$->type = Int; 
			}
			| VOID
			{ 
				$$ = newDeclNode(Var);
				$$->type = Void; 
			}
			;

fun_declaracao		: tipo_especificador id LPAREN params RPAREN composto_decl
			{
				
				$$ = newDeclNode(Fun);
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
				$$->type = $1->type;
				$$->child[0] = $4;
				$$->child[1] = $6;

				
			}
			;

params			: param_lista
			{ $$ = $1; }
			| VOID
			{ $$ = NULL; }
			;

param_lista		: param_lista COMA param
			{
				
				TreeNode * t = $1;
				if( t!= NULL){
					while( t->sibling != NULL )
						t = t->sibling;
					t->sibling=$3;
					$$ = $1;
				}
				else $$ = $3;
			}
			| param
			{ $$ = $1; }
			;

param			: tipo_especificador id
			{
				$$ = newDeclNode(Param);
				$$->type = $1->type;
				$$->attr.name = $2->attr.name;
			}
			| tipo_especificador id LCOLCHETE RCOLCHETE
			{
				
				$$ = newDeclNode(Param);
				$$->type = $1->type;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
			}
			;

composto_decl		: LCHAVE local_declaracoes statement_lista RCHAVE
			{
			
				TreeNode * t = $2;
				if( t!= NULL){
					while( t->sibling != NULL )
						t = t->sibling;
					t->sibling=$3;
					$$ = $2;
				}
				else $$ = $3;
			}
			;

local_declaracoes	: /* string vazia */
			{ $$ = NULL; }
			| local_declaracoes var_declaracao
			{
				
				TreeNode * t = $1;
				if( t!= NULL){
					while( t->sibling != NULL )
						t = t->sibling;
					t->sibling=$2;
					$$ = $1;
				}
				else $$ = $2;
			}
			;

statement_lista		: /* string vazia */
			{ $$ = NULL; }
			| statement_lista statement
			{
				
				TreeNode * t = $1;
				if( t!= NULL){
					while( t->sibling != NULL )
						t = t->sibling;
					t->sibling=$2;
					$$ = $1;
				}
				else $$ = $2;
			}
			;

statement		: expressao_decl
			{ $$ = $1; }
			| composto_decl
			{ $$ = $1; }
			| selecao_decl
			{ $$ = $1; }
			| iteracao_decl
			{ $$ = $1; }
			| retorno_decl
			{ $$ = $1; }
			;

expressao_decl		: expressao SEMI
			{ $$ = $1; }
			| SEMI
			{ $$ = NULL; }
			;

selecao_decl		: IF LPAREN expressao RPAREN statement 
			{
				$$ = newStmtNode(If);
				$$->child[0] = $3;
				$$->child[1] = $5;
			}
			| IF LPAREN expressao RPAREN statement ELSE statement
			{
				$$ = newStmtNode(If);
				$$->child[0] = $3;
				$$->child[1] = $5;
				$$->child[2] = $7;
			}
			;

iteracao_decl		: WHILE LPAREN expressao RPAREN statement
			{
				$$ = newStmtNode(Iter);
				$$->child[0] = $3;
				$$->child[1] = $5;
			}
			;

retorno_decl		: RETURN SEMI
			{
				$$ = newStmtNode(Return);
				$$->child[0] = NULL;
			}
			| RETURN expressao SEMI
			{
				$$ = newStmtNode(Return);
				$$->child[0] = $2;
			}
			;

expressao		: var ASSIGN expressao
			{
				$$ = newStmtNode(Attr);
				$$->child[0] = $1;
				$$->attr.op = ASSIGN;
				$$->child[1] = $3;
			}
			| simples_expressao
			{ $$ = $1; }
			;

var			: id
			{
				$$ = newExpNode(Id);
				$$->lineno = $1->lineno;
				$$->attr.name = $1->attr.name;
			}
			| id LCOLCHETE expressao RCOLCHETE
			{
				$$ =  newExpNode(Id);
				$$->attr.name = $1->attr.name;
				$$->lineno = $1->lineno;
				$$->child[1] = $3;
			}
			;

simples_expressao	: soma_expressao relacional soma_expressao
			{
				$$ = newExpNode(Op);
				$$->child[0] = $1;
				$$->child[2] = $3;
				$$->attr.op = $2->attr.op;
			}
			| soma_expressao
			{ $$ = $1; }
			;

relacional		: MENOROUIGUAL
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = MENOROUIGUAL; 
			}
			| LT
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = LT; 
			}
			| MT
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = MT; 
			}
			| MAIOROUIGUAL
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = MAIOROUIGUAL; 
			}
			| EQ
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = EQ; 
			}
			| DIFERENT
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = DIFERENT; 
			}
			;

soma_expressao		: soma_expressao soma termo
			{
				$$ = newExpNode(Op);
				$$->child[0] = $1;
				$$->child[1] = $3;
				$$->attr.op = $2->attr.op;				
			}
			| termo
			{ $$ = $1; }
			;

soma			: PLUS
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = PLUS; 
			}
			| MINUS
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = MINUS; 
			}
			;


termo			: termo mult fator
			{
				$$ = newExpNode(Op);
				$$->child[0] = $1;
				$$->child[1] = $3;
				$$->attr.op = $2->attr.op;
			}
			| fator
			{ $$ = $1; }
			;

mult			: TIMES			
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = TIMES; 
			}
			| OVER
			{ 
				$$ = newExpNode(Op);
				$$->attr.op = OVER; 
			}

			;

fator			: LPAREN expressao RPAREN
			{ $$ = $2; }
			| var
			{ $$ = $1; }
			| ativacao
			{ $$ = $1; }
			| NUM
			{
				$$ = newExpNode(Const);
				$$->attr.val = atoi(tokenString);		
			}
			;


ativacao		: id LPAREN args RPAREN
			{
				$$ = newStmtNode(Ativacao);
				$$->attr.name = $1->attr.name;
				$$->child[0] = $3;
				$$->lineno = $1->lineno;
			}
			;

args			: /* string vazia */
			{ $$ = NULL; }
			| arg_lista
			{ $$ = $1; }
			;

arg_lista		: arg_lista COMA expressao
			{
				TreeNode * t = $1;
				if( t != NULL )
				{
					while( t->sibling != NULL )
						t = t->sibling;
					t->sibling = $3;
					$$ = $1;
				}

				else $$ = $3;
			}
			| expressao
			{ $$ = $1; }
			;

id			: ID
			{
				$$ =  newExpNode(Id);
				$$->attr.name = copyString(tokenString);
				$$->lineno = lineno;				
			}
			;


%%

void yyerror(char * msg)
{
	printf("Syntax error at line %d: %s\n",lineno,msg);
}

static int yylex(void)
{
	return getToken();
}

TreeNode * parse(void)
{
	yyparse();
	return savedTree;
}
