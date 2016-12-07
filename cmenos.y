%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "stdio.h"
#include "scan.h"
#include "parse.h"

#define YYDEBUG 1
#define YYSTYPE TreeNode *
static TreeNode * savedTree;
static char * nomeEscopo="programa";
void yyerror(char *);
void escopo(char *);
static int yylex(void);
static ExpType tipoFun;


%}

%token ID NUM 
%token ELSE IF INT RETURN VOID WHILE
%token MAIS MENOS VEZES BARRA ATTR COMA EQ 
%token MENOROUIGUAL MAIOROUIGUAL DIFERENTE MAIOR MENOR LPAREN RPAREN LCOLCHETE RCOLCHETE LCHAVE RCHAVE SEMI
%token ERROR

%expect 1

%% /* Grammar for Cmenos */
programa    		: declaracao_lista
			{
			savedTree = $1; 
			}
			;

declaracao_lista    	: declaracao_lista declaracao
			{ YYSTYPE t = $1;
		           if (t != NULL)
		           { while (t->sibling != NULL)
		                t = t->sibling;
		             t->sibling = $2;
		             $$ = $1; }
		             else $$ = $2;
		         }
			| declaracao
			{ $$ = $1; }
			;

declaracao	        : var_declaracao
			{ $$ = $1; }
			| fun_declaracao
			{ $$ = $1; }
			|error  {  $$ = NULL; }
			;

var_declaracao		: tipo_especificador id SEMI
			{
				$$ = newDeclNode(VarK);
				$$->escopo=nomeEscopo;
				$$->tipo = $1->tipo;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
			}
			| tipo_especificador id LCOLCHETE fator RCOLCHETE SEMI
			{
				$$ = newDeclNode(VetK);
				$$->escopo=nomeEscopo;
				$$->tipo = $1->tipo;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
				$$->child[0]=$4;
				
			}
			;

tipo_especificador	: INT
			{
				$$ = newDeclNode(VarK);
				$$->tipo = Integer;
			}
			| VOID
			{
				$$ = newDeclNode(VarK);
				$$->tipo = Void;
			}
			
			;

fun_declaracao		: tipo_especificador id {nomeEscopo=copyString($2->attr.name);tipoFun=$1->tipo;}LPAREN params RPAREN composto_decl
			{

				$$ = newDeclNode(FunK);
				$$->tipo = $1->tipo;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
				$$->child[0] = $5;
				$$->child[1] = $7;
			} 
			;

params			: param_lista
			{ $$ = $1; }
			| VOID
			{ $$ = NULL; }
			;

param_lista		: param_lista COMA param
			  { YYSTYPE t = $1;
		           if (t != NULL)
		           { while (t->sibling != NULL)
		                t = t->sibling;
		             t->sibling = $3;
		             $$ = $1; }
		             else $$ = $3;
		         }
			| param
			{ $$ = $1; }
			;

param			: tipo_especificador id
			{
				$$ = newParamNode(DeclK);
				$$->escopo=nomeEscopo;
				$$->tipo = $1->tipo;
				$$->attr.name = $2->attr.name;
			}
			| tipo_especificador id LCOLCHETE RCOLCHETE
			{

				$$ = newParamNode(DeclVK);
				$$->escopo=nomeEscopo;
				$$->tipo = $1->tipo;
				$$->attr.name = $2->attr.name;
				$$->lineno = $2->lineno;
			}
			;

composto_decl		: LCHAVE local_declaracoes statement_lista RCHAVE
			{ YYSTYPE t = $2;
			  if (t != NULL)
			   { while (t->sibling != NULL)
			      t = t->sibling;
			      t->sibling = $3;
			      $$ = $2; }
			    else $$ = $3;
		        }
			;

local_declaracoes	: { $$ = NULL; } //vazio
			| local_declaracoes var_declaracao
			{ YYSTYPE t = $1;
		           if (t != NULL)
		           { while (t->sibling != NULL)
		                t = t->sibling;
		             t->sibling = $2;
		             $$ = $1; }
		             else $$ = $2;
		         }
			;

statement_lista		: { $$ = NULL; } //vazio
			| statement_lista statement
		
			{ YYSTYPE t = $1;
		           if (t != NULL)
		           { while (t->sibling != NULL)
		                t = t->sibling;
		             t->sibling = $2;
		             $$ = $1; }
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
			|error  {  $$ = NULL; }
			;

expressao_decl		: expressao SEMI
			{ $$ = $1; }
			| SEMI
			{ $$ = NULL; }
			;

selecao_decl		: IF LPAREN expressao RPAREN statement
			{
				$$ = newStmtNode(IfK);
				$$->child[0] = $3;
				$$->child[1] = $5;
			}
			| IF LPAREN expressao RPAREN statement ELSE statement
			{
				$$ = newStmtNode(IfK);
				$$->child[0] = $3;
				$$->child[1] = $5;
				$$->child[2] = $7;
			}
			;

iteracao_decl		: WHILE LPAREN expressao RPAREN statement
			{
				$$ = newStmtNode(WhileK);
				$$->child[0] = $3;
				$$->child[1] = $5;
			}
			;

retorno_decl		: RETURN SEMI
			{
				$$ = newStmtNode(ReturnK);
				$$->child[0] = NULL;
				$$->tipo=tipoFun;
			}
			| RETURN expressao SEMI
			{
				$$ = newStmtNode(ReturnK);
				$$->child[0] = $2;
				$$->tipo=tipoFun;
			}
			;

expressao		: var ATTR expressao
			{
				$$ = newExpressionNode(EqualK);
				$$->child[0] = $1;
				$$->attr.op = ATTR;
				$$->child[1] = $3;
			}
			| simples_expressao
			{ $$ = $1; }
			;

var			: id
			{
				$$ = newExpressionNode(VariavelK);
				$$->escopo=nomeEscopo;
				$$->lineno = $1->lineno;
				$$->attr.name = $1->attr.name;
			}
			|  id LCOLCHETE expressao RCOLCHETE
			{
				$$ =  newExpressionNode(VetorK);
				$$->escopo=nomeEscopo;
				$$->attr.name = $1->attr.name;
				$$->lineno = $1->lineno;
				$$->child[0] = $3;
			}
			;

simples_expressao	: soma_expressao relacional soma_expressao
			{
				$$ = newExpressionNode(OpK);
				$$->child[0] = $1;
				$$->child[2] = $3;
				$$->attr.op = $2->attr.op;
			}
			| soma_expressao
			{ $$ = $1; }
			;

relacional		:  MENOROUIGUAL { $$ = newExpressionNode (OpK);
			$$->attr.op = MENOROUIGUAL;}
			    | MENOR {$$ = newExpressionNode (OpK);
					$$->attr.op = MENOR;}
			    | MAIOR {$$ = newExpressionNode (OpK);
					$$->attr.op = MAIOR;}
			    | MAIOROUIGUAL {$$ = newExpressionNode (OpK);
					$$->attr.op = MAIOROUIGUAL;}
			    | EQ { $$ = newExpressionNode (OpK);
					$$->attr.op = EQ;}
			    | DIFERENTE { $$ = newExpressionNode (OpK);
					$$->attr.op = DIFERENTE;}
			    ;


soma_expressao		: soma_expressao soma termo
			{
				$$ = newExpressionNode(OpK);
				$$->child[0] = $1;
				$$->child[1] = $3;
				$$->attr.op = $2->attr.op;
			}
			| termo
			{ $$ = $1; }
			;

soma			: MAIS
			{
				$$ = newExpressionNode(OpK);
				$$->attr.op = MAIS;
			}
			| MENOS
			{
				$$ = newExpressionNode(OpK);
				$$->attr.op = MENOS;
			}
			;


termo			: termo mult fator
			{
				$$ = newExpressionNode(OpK);
				$$->child[0] = $1;
				$$->child[1] = $3;
				$$->attr.op = $2->attr.op;
			}
			| fator
			{ $$ = $1; }
			;

mult			: VEZES
			{
				$$ = newExpressionNode(OpK);
				$$->attr.op = VEZES;
			}
			| BARRA
			{
				$$ = newExpressionNode(OpK);
				$$->attr.op = BARRA;
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
				$$ = newExpressionNode(ConstK);
      				$$->attr.val=atoi(tokenString);
      				$$->tipo=Integer;
			}
			;


ativacao		: id LPAREN args RPAREN
			{
				$$ = newExpressionNode(CallK);
				$$->attr.name = $1->attr.name;
				$$->child[0] = $3;
				$$->lineno = $1->lineno;
				$$->escopo=nomeEscopo;		
			}
			;

args			: { $$ = NULL; } //vazio
			| arg_lista
			{ $$ = $1; }
			;

arg_lista		: arg_lista COMA expressao
			{ YYSTYPE t = $1;
		           if (t != NULL)
		           { while (t->sibling != NULL)
		                t = t->sibling;
		             t->sibling = $3;
		             $$ = $1; }
		             else $$ = $3;
		         }
			| expressao
			{ $$ = $1; }
			;

id			: ID
			{
				$$ =  newExpressionNode(IdNovoK);
				$$->attr.name = copyString(tokenString);
				$$->lineno = lineno;
			}
			;

%%

void yyerror(char * msg)
{
	fprintf(listing,"Syntax error at line %d: %s\n",lineno,msg);
  	fprintf(listing,"Current token: ");
  	printToken(yychar,tokenString);
  	Error = TRUE;
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

void esvazia(char *c)
{
    int i, tam=strlen(c);
    if(tam>0){
	    for(i=0; i<=tam-1; i++)
	    {
		c[i]='\0';
	    }
    }
}

void escopo(char *c)
{
    int i, tamanho=0;
    tamanho=strlen(c);
    for(i=0; i<=tamanho-2; i++)
    {
        nomeEscopo[i]=c[i];
    }
}

