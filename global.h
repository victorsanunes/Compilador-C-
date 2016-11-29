#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "cminus.tab.h"

#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* Número de palavras reservadas */
#define MAXRESERVED 6

typedef int TokenType; 

extern FILE* source;	/* Código fonte do programa */
extern FILE* listing;	/* Saída para arquivo de texto */
extern FILE* code;	/* Saída para arquivo de texto */

extern int lineno;	/* Número da linha */

/**************************************************/
/**************   Árvore sintática ****************/
/**************************************************/


typedef enum {Stmt, Exp, Decl, /*ListK, Error*/} NodeKind;
typedef enum {If, Iter, Attr, /*Cmpd,*/ Return, Ativacao} StmtKind;
typedef enum {/*Factor, */Const, /*Additive, Simple, */Id, Term, Op} ExpKind;
typedef enum {Var, Fun, Param, Type} DeclKind;
//typedef enum {Params, Local, StmtList, AdditiveList, TermList, Args} ListKind;

/* Usado para checagem de tipo */
typedef enum {Void,Int} ExpType;

/* Número máximo de filhos que um nó pode ter */
#define MAXCHILDREN 3

typedef struct treeNode{

    struct treeNode * child[MAXCHILDREN];
    struct treeNode * sibling;
	
    int lineno;
    
    union { StmtKind stmt; ExpKind exp; DeclKind decl; /*ListKind list;*/ } kind;
    union {
		TokenType op;
            	int val;
            	char * name;	
	} attr;
	
    ExpType type;
    NodeKind nodekind;
	
} TreeNode;
#endif
