#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>

#define SIZE 211
char aux[300];


#define SHIFT 4

typedef struct quadra{
  char * campo1;
  char * campo2;
  char * campo3;
  char * campo4;
  struct quadra * prox;
}quadra;
quadra *no1;
typedef struct lista{
  quadra * inicio;
  quadra * fim;
  int tam;
}lista;

void inicializaLIsta(lista * l){
  l->inicio = NULL;
  l->fim = NULL;
  l->tam=0;
}

quadra* criaNo(char *campo1, char *campo2, char *campo3,char *campo4){
  quadra *novoNo;
  novoNo = malloc(sizeof(quadra));
  novoNo->campo1 =campo1;
  novoNo->campo2 =campo2;
  novoNo->campo3 =campo3;
  novoNo->campo4 =campo4;
  novoNo->prox =  NULL;
  return novoNo;
}

void insereFinal( lista *l, quadra *no){
  if(l->inicio == NULL){//lista vazia
    l->inicio = no;
    l->fim = no;
    l->tam++;
  }
  else{
    l->fim->prox = no;
    l->fim = no;
    l->tam++;
  }
}

void imprimeLista(lista *l){
  quadra *p;
  p = l->inicio;
  while(p!=NULL){
    printf("(%s,%s,%s,%s)\n",p->campo1,p->campo2,p->campo3,p->campo4);
    p= p->prox;
  }
}
int indicadorDeLinhaIF[20];
int contadorDeLinhaIF = -1;
int indicadorDeLinhaWhile1[20];
int contadorDeLinhaWhile1 = -1;
int indicadorDeLinhaWhile2[20];
int contadorDeLinhaWhile2= -1;
int indicadorDeLinhaElse[20];
int contadorDeLinhaElse = -1;
int pilhaQntArgumento[20];
int topoPilha=-1;
int indiceTemporario = 0;
int contadorDeLinha = 1;
int contparam =0;
int auxVetR = 0;

int hash ( char * key ){
   int temp = 0;
   int i = 0;
   while (key[i] != '\0')
   {
       temp = ((temp << SHIFT) + key[i]) % SIZE;
       ++i;
   }
   return temp;
}

static void cGen (TreeNode * tree, FILE *code,lista *codigoIntermediario);
static void cGenA( TreeNode * tree, FILE *code,lista *codigoIntermediario);

char *printSymbol(TreeNode *tree, FILE *code){
    char *aux;
    switch (tree->attr.op) {
        case ATTR:
            //fprintf(stdout, " = ");
            aux="asg";
            return aux;
            break;
        case EQ:
              //fprintf(stdout, " == ");
              aux="igual";
              return aux;
              break;
        case MENOROUIGUAL:
            //fprintf(stdout, " <= ");
            aux="menorI";
            return aux;
            break;
        case MAIOROUIGUAL:
            //fprintf(stdout, " >= ");
            aux="maiorI";
            return aux;
            break;
        case DIFERENTE:
            //fprintf(stdout, " != ");
            aux="dif";
            return aux;
            break;
        case MENOR:
          //  fprintf(stdout, " < ");
            aux="menor";
            return aux;
            break;
        case MAIOR:
            //fprintf(stdout, " > ");
            aux="maior";
            return aux;
            break;
        case MAIS:
            //fprintf(stdout, " + " );
            aux="sum";
            return aux;
            break;
        case MENOS:
            //fprintf(stdout, " - ");
            aux="sub";
            return aux;
            break;
        case VEZES:
            //fprintf(stdout, " * ");
            aux="mul";
            return aux;
            break;
        case BARRA:
            //fprintf(stdout, " / ");
            aux="div";
            return aux;
            break;
        default:
            break;
    }
}
int checaSymbol(TreeNode *tree, FILE *code){
    switch (tree->attr.op) {
        case ATTR:
            return 0;
            break;
        case EQ:
              return 1;
              break;
        case MENOROUIGUAL:
            return 1;
            break;
        case MAIOROUIGUAL:
            return 1;
            break;
        case DIFERENTE:
            return 1;
            break;
        case MENOR:
            return 1;
            break;
        case MAIOR:
            return 1;
            break;
        case MAIS:
            return 0;
            break;
        case MENOS:
            return 0;
            break;
        case VEZES:
            return 0;
            break;
        case BARRA:
            return 0;
            break;
        default:
            break;
    }
}
static void genStmt(TreeNode * tree, FILE *code, lista *codigoIntermediario){
    TreeNode *condicao_do_if, *corpo_do_if, *corpo_do_else,*corpo_do_while,*condicao_do_while, *sentenca_return;



    switch (tree->kind.stmt) {
        case IfK:
            condicao_do_if  = tree->child[0];
            corpo_do_if     = tree->child[1];
            corpo_do_else   = tree->child[2];
            if(condicao_do_if != NULL){
                cGen(condicao_do_if,code,codigoIntermediario);
                fprintf(stdout, "(if_f,t%d,L%d,_)\n",indiceTemporario-1,contadorDeLinha);
                contadorDeLinhaIF++;
                indicadorDeLinhaIF[contadorDeLinhaIF]=contadorDeLinha;
                contadorDeLinha++;
                cGen(corpo_do_if,code,codigoIntermediario);
                if(corpo_do_else!=NULL){
                  contadorDeLinhaElse++;
                  indicadorDeLinhaElse[contadorDeLinhaElse]=contadorDeLinha;
                  contadorDeLinha++;
                  fprintf(stdout, "(got,L%d,_,_)\n", indicadorDeLinhaElse[contadorDeLinhaElse]);
                }
                fprintf(stdout, "(lab,L%d,_,_)\n",indicadorDeLinhaIF[contadorDeLinhaIF]);
                if(corpo_do_else!=NULL){
                  cGen(corpo_do_else,code,codigoIntermediario);
                  fprintf(stdout, "(lab,L%d,_,_)\n",indicadorDeLinhaElse[contadorDeLinhaElse]);
                    contadorDeLinhaElse--;

                }
                contadorDeLinhaIF--;

            }
            break;
        case WhileK:
          condicao_do_while  = tree->child[0];
          corpo_do_while     = tree->child[1];
          if(condicao_do_while != NULL){
            fprintf(stdout, "(lab,L%d,_,_)\n",contadorDeLinha);
            contadorDeLinhaWhile1++;
            indicadorDeLinhaWhile1[contadorDeLinhaWhile1]=contadorDeLinha;
            contadorDeLinha++;
            cGen(condicao_do_while,code,codigoIntermediario);
            fprintf(stdout, "(if_f,t%d,L%d,_)\n",indiceTemporario-1,contadorDeLinha);
            contadorDeLinhaWhile2++;
            indicadorDeLinhaWhile2[contadorDeLinhaWhile2]=contadorDeLinha;
            contadorDeLinha++;
            cGen(corpo_do_while,code,codigoIntermediario);
            fprintf(stdout, "(got,L%d,_,_)\n",indicadorDeLinhaWhile1[contadorDeLinhaWhile1]);
            contadorDeLinhaWhile1--;
            fprintf(stdout, "(lab,L%d,_,_)\n",  indicadorDeLinhaWhile2[contadorDeLinhaWhile2]);
            contadorDeLinhaWhile2--;

          }


            break;
      case ReturnK:
            sentenca_return = tree->child[0];
            if(sentenca_return != NULL){
              if(sentenca_return->kind.expression == VariavelK || sentenca_return->kind.expression == ConstK){
                fprintf(stdout, "(ret");
                cGen(sentenca_return,code,codigoIntermediario);
                fprintf(stdout, ",_,_)\n");
              }else{
                cGen(sentenca_return,code,codigoIntermediario);
                fprintf(stdout, "(ret,t%d,_,_)\n",indiceTemporario-1);
              }
            }


            break;
    }
}
static void genDecli(TreeNode * tree, FILE *code,lista *codigoIntermediario){
    TreeNode *p1, *p2, *p3;
    switch (tree->kind.decl) {
        case VarK:
          break;
        case VetK:
          break;
        case FunK:
          fprintf(stdout,"(lab,%d,_,_)\n",hash(tree->attr.name));
          cGen(tree->child[1],code,codigoIntermediario);
          break;
    }
}

static void genExpression(TreeNode * tree, FILE *code,lista *codigoIntermediario){
    TreeNode *p1, *p2, *p3;
    switch (tree->kind.expression) {

        case ConstK:
          if(tree != NULL){
              fprintf(stdout,",%d",tree->attr.val);
          }
          break;
        case OpK:
            if(checaSymbol(tree,code)){
            p1 = tree->child[0];
            p2 = tree->child[2];
          }else{
            p1 = tree->child[0];
            p2 = tree->child[1];
          }
            if(p1 != NULL && p2 != NULL){
              //fprintf(listing, "**p1=%d  p2=%d\n",p1->kind.expression,p2->kind.expression );
              if((p1->kind.expression == ConstK || p1->kind.expression == VariavelK) && (p2->kind.expression == ConstK || p2->kind.expression == VariavelK)){
                fprintf(stdout, "(%s",printSymbol(tree,code));
                cGen(p1,code,codigoIntermediario);
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout,",t%d)\n",indiceTemporario );
                indiceTemporario++;
              } else if ((p1->kind.expression == ConstK || p1->kind.expression == VariavelK) && (p2->kind.expression == VetorK || p2->kind.expression == CallK)){
                auxVetR=0;
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout, "(%s",printSymbol(tree,code));//code
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );//code
                indiceTemporario++;
              }else if((p2->kind.expression == ConstK || p2->kind.expression == VariavelK) && (p1->kind.expression == VetorK || p1->kind.expression == CallK)){
                auxVetR=0;
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, "(%s",printSymbol(tree,code));//code
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );//code
                indiceTemporario++;
              }else if(p2->kind.expression == OpK && (p1->kind.expression == ConstK || p1->kind.expression == VariavelK)){
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout, "(%s",printSymbol(tree,code));
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );
                indiceTemporario++;
              }else if(p1->kind.expression == OpK && (p2->kind.expression == ConstK || p2->kind.expression == VariavelK) ){
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, "(%s",printSymbol(tree,code));
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );
                indiceTemporario++;
              }else if(p2->kind.expression == OpK && (p1->kind.expression == VetorK || p1->kind.expression == CallK)){
                cGen(p2,code,codigoIntermediario);
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, "(%s,t%d,t%d,t%d)\n",printSymbol(tree,code),indiceTemporario-2,indiceTemporario-1,indiceTemporario);
                indiceTemporario++;
              }else if(p1->kind.expression == OpK && (p2->kind.expression == VetorK || p2->kind.expression == CallK)){
                cGen(p2,code,codigoIntermediario);
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, "(%s,t%d,t%d,t%d)\n",printSymbol(tree,code),indiceTemporario-2,indiceTemporario-1,indiceTemporario);
                indiceTemporario++;
              }else{
                auxVetR=0;
                cGen(p2,code,codigoIntermediario);
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, "(%s,t%d,t%d,t%d)\n",printSymbol(tree,code),indiceTemporario-2,indiceTemporario-1,indiceTemporario);
                indiceTemporario++;
              }
              }

            break;
        case VariavelK:
            if(tree != NULL){
                fprintf(stdout, ",%s", tree->attr.name);
              //  fprintf(code, "%s ", tree->attr.name);
            }
            break;
        case EqualK:
          p1 = tree->child[0];
          p2 = tree->child[1];
          if(p1!=NULL && p2!=NULL){
            if(p1->kind.expression == VetorK){
              if(p2->kind.expression == ConstK || p2->kind.expression == VariavelK){
                fprintf(stdout, "(%s",printSymbol(tree,code));//code
                cGen(p2,code,codigoIntermediario);
                auxVetR=1;
                cGen(p1,code,codigoIntermediario);
              }
              else if(p2->kind.expression == OpK || p2->kind.expression == VetorK){
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout, "(%s,t%d",printSymbol(tree,code),indiceTemporario-1);
                auxVetR=1;
                cGen(p1,code,codigoIntermediario);
              }
            }else{// quando p1 é variavelk
              if(p2->kind.expression == ConstK || p2->kind.expression == VariavelK){
                fprintf(stdout, "(%s",printSymbol(tree,code));//code
                cGen(p2,code,codigoIntermediario);
                auxVetR=1;
                fprintf(stdout, ",_");
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, ")\n");
              }
              else if(p2->kind.expression == OpK || p2->kind.expression == VetorK || p2->kind.expression == CallK){
                cGen(p2,code,codigoIntermediario);
                fprintf(stdout, "(%s,t%d",printSymbol(tree,code),indiceTemporario-1);
                auxVetR=1;
                fprintf(stdout, ",_");
                cGen(p1,code,codigoIntermediario);
                fprintf(stdout, ")\n");
              }

            }

          }
          break;
        case VetorK:
          p1 = tree->child[0];
          if(p1 != NULL){
            if(auxVetR == 1){ // caso o vetor recebe
              cGen(p1,code,codigoIntermediario);
              fprintf(stdout, ",%s)\n",tree->attr.name);
              auxVetR=0;
            }
            else{ // caso o vetor seja somado
              fprintf(stdout, "(asg,%s",tree->attr.name);//code
              cGen(p1,code,codigoIntermediario);
              fprintf(stdout, ",t%d)\n",indiceTemporario );
              indiceTemporario++;
            }
          }
          break;
        case CallK:
          p1=tree->child[0];
          while(p1!=NULL){
            contparam++;
          	p1=p1->sibling;
          }
          topoPilha++;
          pilhaQntArgumento[topoPilha]=contparam;
          contparam=0;
          p1=tree->child[0];
          while(p1!=NULL){
            if(p1!=NULL){
              if(p1->kind.expression == ConstK ){
                fprintf(stdout,"(param,%d,_,_)\n",p1->attr.val);
                p1=p1->sibling;
              }
            }
            if(p1!=NULL){
              if(p1->kind.expression == VetorK || p1->kind.expression == OpK || p1->kind.expression == CallK){
                auxVetR=0;
                cGenA(p1,code,codigoIntermediario);
               	fprintf(stdout,"(param,t%d,_,_)\n",(indiceTemporario-1));
                p1=p1->sibling;
              }
            }

            if(p1!=NULL){
              if (p1->kind.expression == VariavelK){
                fprintf(stdout,"(param,%s,_,_)\n",p1->attr.name);
              	 p1=p1->sibling;
              }
            }
          }
          if(tree->tipo==Void){
		  	       fprintf(stdout,"(call,%d,%d,_)\n",hash(tree->attr.name),pilhaQntArgumento[topoPilha]);
               pilhaQntArgumento[topoPilha]=0;
               topoPilha--;

		           }
               else{
		  	            fprintf(stdout,"(call,%d,%d,t%d)\n",hash(tree->attr.name),pilhaQntArgumento[topoPilha],indiceTemporario);
                    pilhaQntArgumento[topoPilha]=0;
                    topoPilha--;
		  	            indiceTemporario++;
		           }

          break;
        default:
        break;



    }
}

static void cGen( TreeNode * tree, FILE *code,lista *codigoIntermediario){
    if (tree != NULL)
    {
        //Verifica o tipo do no

        switch (tree->nodekind)
        {

        //No para declaracoes
        case StmtK:
        	//fprintf(stdout, "\ncase StmtK :\n");
            genStmt(tree, code,codigoIntermediario);
            break;
        case DeclKi:
            //fprintf(stdout, "\ncase DeclK :\n");
            genDecli(tree,code,codigoIntermediario);
            break;
        case ExpressionK:
		//fprintf(stdout, "\ncase ExpressionK :\n");

            genExpression(tree, code,codigoIntermediario);

            break;
        /*case ParamK:
	        //fprintf(stdout, "\ncase ParamK :\n");
            genParam(tree);
            break;*/
        default:
            break;
        }
        cGen(tree->sibling, code,codigoIntermediario);
    }
}

static void cGenA( TreeNode * tree, FILE *code,lista *codigoIntermediario){
    if (tree != NULL)
    {
        //Verifica o tipo do no

        switch (tree->nodekind)
        {

        //No para declaracoes
        /*case StmtK:
        	//fprintf(stdout, "\ncase StmtK :\n");
            genStmt(tree, code);
            break;*/
        case DeclKi:
            //fprintf(stdout, "\ncase DeclK :\n");
            genDecli(tree,code,codigoIntermediario);
            break;
        case ExpressionK:
		//fprintf(stdout, "\ncase ExpressionK :\n");

            genExpression(tree, code,codigoIntermediario);

            break;
        /*case ParamK:
	        //fprintf(stdout, "\ncase ParamK :\n");
            genParam(tree);
            break;*/
        default:
            break;
        }
    }
}

void codeGen(TreeNode * syntaxTree, char * codefile, FILE *code){   lista codigoIntermediario;
    inicializaLIsta(&codigoIntermediario);
    char * s = malloc(strlen(codefile)+7); //Aloca mais 7 espaços para adicionar a string "File: " e o /0
    strcpy(s,"File: ");
    strcat(s,codefile); //concatena com o nome do arquivo de codigo intermediario
    fprintf(stdout,"\nCMINUS Compilation\n\n"); //imprime no terminal
    //fprintf(listing,"Codigo Intermediario\n\n"); //imprime no arquivo
    //fprintf(listing,"\nStandard prelude:");
    //fprintf(listing,"\nEnd of standard prelude.");
    //fprintf(listing,"\nCodigo Intermediario\n");
    cGen(syntaxTree, code,&codigoIntermediario); //Funcao principal de geracao
    /*fprintf(listing, "\nCodigo em quadra:\n" );
    imprimeLista(&codigoIntermediario);*/
    fprintf(listing,"\n\nEnd of execution.\n\n");
}
