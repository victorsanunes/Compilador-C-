/****************************************************/
/* Arquivo: analyze.c                               */
/* Implementação do analisador semântico            */
/* para o compilador Cminus                         */
/* Compiler Construction:                           */
/* Eliseu Júnio Araújo                              */
/****************************************************/

#include "global.h"
#include "symtab.h"
#include "analyze.h"
#include <string.h>

/* contador para a localização na memória da variável */
static int location = 0;

/*
 * Procedure traverse is a generic recursive
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
                      void (* preProc) (TreeNode *),
                      void (* postProc) (TreeNode *) )
{
    if (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i=0; i < MAXCHILDREN; i++)
                traverse(t->child[i],preProc,postProc);
        }
        postProc(t);
        traverse(t->sibling,preProc,postProc);
    }
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{
    if (t==NULL) return;
    else return;
}

static void typeError(TreeNode * t, char * message)
{
    fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
    Error = TRUE;
}

static void declError(TreeNode * t, char * message)
{
    fprintf(listing,"Declaration error at line %d: %s\n",t->lineno,message);
    Error = TRUE;
}

char * cat(char * s, char * v)
{
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    n = n+strlen(v);
    t = malloc(n);
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else
    {
        strcpy(t,s);
        strcat(t,v);
    }
    return t;
}

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
static void insertNode( TreeNode * t)
{
    switch (t->nodekind)
    {
    case DeclKi:
        switch (t->kind.decl)
        {
        case Fun:
            if (st_lookup(cat(t->attr.name,t->attr.name)) == -1)
                st_insert_first(cat(t->attr.name,t->attr.name), t->attr.name, t->lineno,location++,t->escopo, t->type, Funcao);
            else
                declError(t, "erro de declaracao de funcao: funcao ja declarada");
            break;
        case Var:
            if (t->type==Void)
            {
                declError(t, "erro de declaracao de variavel: tipo void nao e permitido a variaveis");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1 )
            {
                declError(t, "erro de declaracao de variavel: mesmo nome usado em funcao");
            }
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1)
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->type, Variavel);
            else
                declError(t, "erro de declaracao de variavel: variavel ja declarada");
            break;
        case Vet:
            if (t->type==Void)
            {
                declError(t, "erro de declaracao de vetores: tipo void nao e permitido a vetores");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "erro de declaracao de vetores: mesmo nome usado em funcao");
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1)
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->type, Vetor);
            else
                declError(t, "erro de declaracao de vetores: vetor ja declarado");
            break;
        default:
            break;
        }
        break;
    case Param:
        switch (t->kind.param)
        {
        case Decl:
            if (t->type==Void)
            {
                declError(t, "erro de parametro: tipo void nao e permitido a variaveis");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "erro de parametro: mesmo nome usado em funcao");
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1)
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->type, Variavel);

            else
                declError(t, "Ha mesmo nome em outra declaração de funcao ou veriavel");
            break;
        case DeclV:
            if (t->type==Void)
            {
                declError(t, "erro de parametro: tipo void nao e permitido a vetores");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "erro de parametro: mesmo nome usado em funcao");
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1)
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->type, Vetor);
            else
                declError(t, "Há mesmo nome em outra declaração de função ou vetor");
            break;
        default:
            break;
        }
        break;
    case Exp:
        switch (t->kind.expression)
        {
        case Variavel:
            if (st_lookup(cat(t->attr.name,t->escopo)) == -1 && st_lookup(cat(t->attr.name,"programa")) == -1)
                declError(t, "erro de chamada de variavel: variavel nao declarada");
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "erro de chamada de variavel: mesmo nome usado em funcao");
            else
            {
                if (st_lookup(cat(t->attr.name,t->escopo)) != -1)
                {
                    t->type=st_lookupTipo(cat(t->attr.name, t->escopo));
                    st_insert(cat(t->attr.name,t->escopo), t->lineno);
                }
                else if(st_lookup(cat(t->attr.name,"programa")) != -1)
                {
                    t->type=st_lookupTipo(cat(t->attr.name, "programa"));
                    st_insert(cat(t->attr.name,"programa"), t->lineno);
                }
            }
            break;
        case Vetor:
            if (st_lookup(cat(t->attr.name,t->escopo)) == -1 && st_lookup(cat(t->attr.name,"programa")) == -1)
                declError(t, "erro de chamada de vetor: vetor nao declarado");
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "erro de chamada de vetor: mesmo nome usado em funcao");
            else
            {

                if (st_lookup(cat(t->attr.name,t->escopo)) != -1)
                {
                    t->type=st_lookupTipo(cat(t->attr.name, t->escopo));
                    st_insert(cat(t->attr.name,t->escopo), t->lineno);
                }
                else if(st_lookup(cat(t->attr.name,"programa")) != -1)
                {
                    t->type=st_lookupTipo(cat(t->attr.name, "programa"));
                    st_insert(cat(t->attr.name,"programa"), t->lineno);
                }
            }
            break;
        case Call:
            if (st_lookup(cat(t->attr.name,t->attr.name)) == -1)
                declError(t, "erro de chamada de funcao: funcao nao declarada");
            else
            {
                t->type=st_lookupTipo(cat(t->attr.name, t->attr.name));
                st_insert(cat(t->attr.name,t->attr.name), t->lineno);
            }
            break;
        case Op:
            if ((t->attr.op == MEIG) || (t->attr.op == LT) || (t->attr.op == RT) || (t->attr.op == MAIG) || (t->attr.op == IGIG) || (t->attr.op == EXIG) || (t->attr.op == OR) || (t->attr.op == EE) || (t->attr.op == NOT))
                t->type = Boolean;
            else
                t->type = Integer;
            break;
        case Unary:
        case Equal:
        case Id:
        default:
            break;
        }
        break;
    case Stmt:
    default:
        break;
    }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
    st_insert_first(cat("input","input"), "input", 0,location++,"input", Integer, Funcao);
    st_insert_first(cat("output","output"), "output", 0,location++,"output", Void, Funcao);
    traverse(syntaxTree,insertNode,nullProc);
    if(st_lookup(cat("main","main")) == -1)
    {
        declError(syntaxTree, "função main nao declarada");
    }
    if (TraceAnalyze)
    {
        fprintf(listing,"\nSymbol table:\n\n");
        printSymTab(listing);
    }
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{
    switch (t->nodekind)
    {
    case Exp:
        switch (t->kind.expression)
        {
        case Op:
            if(t->child[0] != NULL && t->child[1] != NULL)
            {
                if (t->child[0]->type != t->child[1]->type)
                    typeError(t,"Operacao aplicada para tipos diferentes");
            }
            if(t->child[0] != NULL && t->child[1] != NULL)
            {
                if(t->child[0]->type!=t->type || t->child[0]->type!=t->type)
                {
                    typeError(t,"Operacao aplicada para tipos diferentes");
                }
            }
            break;
        case Unary:
            if(t->attr.op == NOT)
            {
                t->type = Boolean;
            }
            else if(t->attr.op == MINUS)
            {
                t->type = Integer;
            }
            if(t->type!=t->child[0]->type)
            {
                typeError(t,"Operacao aplicada para tipos diferentes");
            }
        case Equal:
            if(t->child[0] != NULL && t->child[1] != NULL)
            {
                if (t->child[0]->type != t->child[1]->type)
                    typeError(t,"Operacao aplicada para tipos diferentes");
            }
            break;
        case Vetor:
            if(t->child[0] != NULL)
            {
                if(t->child[0]->type!=Integer)
                {
                    typeError(t,"it have expected Integer");
                }
            }
            break;
        case Const:
        case Variavel:
        case Call:
        default:
            break;
        }
        break;
    case Stmt:
        switch (t->kind.stmt)
        {
        case If:
            if(t->child[0] != NULL)
            {
                if (t->child[0]->type != Boolean)
                    typeError(t->child[0],"if test is not Boolean");
            }
            break;
        case While:
            if(t->child[0] != NULL)
            {
                if (t->child[0]->type != Boolean)
                    typeError(t->child[0],"While test is not Boolean");
            }
            break;
        case Return:
            if (t->child[0]!=NULL)
            {
                if (t->child[0]->type != t->type)
                    typeError(t->child[0],"Tipo de retorno não corresponde ao da função");
            }
            else
            {
                if(t->type!=Void)
                {
                    typeError(t,"Tipo de retorno não corresponde ao da função");
                }
            }
        case Coun:
        default:
            break;
        }
        break;
    case DeclKi:
        switch (t->kind.decl)
        {
        case Var:
            break;
        case Vet:
            break;
        case Fun:
            break;
        default:
            break;
        }
        break;
    case Param:
        switch (t->kind.decl)
        {
        case Decl:
            break;
        case DeclV:
            break;
        default:
            break;
        }
        break;
    default:
        break;

    }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
    traverse(syntaxTree,nullProc,checkNode);
}
