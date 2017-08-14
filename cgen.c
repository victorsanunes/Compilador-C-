#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>
#include "lista.h"

#define SIZE 211
char aux[300];
#define SHIFT 4


quadra no;
quadra noAUX;


int campo;
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
int contparam = 0;
int contparam_aux;
int auxVetR = 0;
int memlocParametro = -1;
int vetorPorReferencia = -1;

int hash ( char * key ){
    int temp = 0;
    int i = 0;
    while (key[i] != '\0'){
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

//concatena s e v
char * concatena(char * s, char * v){
    //printf("concatena %s com %s\n", s, v);
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    n = n+strlen(v);
    t = malloc(n);
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else{
        strcpy(t,s);
        strcat(t,v);
    }
    return t;
}

static void cGen (TreeNode * tree, FILE *code);
static void cGenA( TreeNode * tree, FILE *code);

int buscaMemlocDeParametro(char * escopo, int posicaoDoParametro){
	int h;
	BucketList l;
	for(h = 0; h < SIZE; h++){
		l = hashTable[h];
        if(l != NULL){
            // printf("h: %d\n", h);
            // printf("escopo: %s \t posicaoDoParametro: %d\n", l->escopo, l->posicaoDoParametro);
            if(l->escopo != NULL){
                if( !strcmp(escopo,l->escopo) ){
                    // printf("%s\n", l->escopo);
                    if(l->posicaoDoParametro == posicaoDoParametro){
                        //printf("h: %d\n", l->memloc);
                        return l->memloc;
                    }
                }
            }
        }
    }
}

char *printSymbol(TreeNode *tree, FILE *code){
    char *aux;
    switch (tree->attr.op) {
        case ATTR:
            //fprintf(code, " = ");
            aux="asg";
            return aux;
            break;
        case EQ:
              //fprintf(code, " == ");
              aux="igual";
              return aux;
              break;
        case MENOROUIGUAL:
            //fprintf(code, " <= ");
            aux="menorI";
            return aux;
            break;
        case MAIOROUIGUAL:
            //fprintf(code, " >= ");
            aux="maiorI";
            return aux;
            break;
        case DIFERENTE:
            //fprintf(code, " != ");
            aux="dif";
            return aux;
            break;
        case MENOR:
          //  fprintf(code, " < ");
            aux="menor";
            return aux;
            break;
        case MAIOR:
            //fprintf(code, " > ");
            aux="maior";
            return aux;
            break;
        case MAIS:
            //fprintf(code, " + " );
            aux="sum";
            return aux;
            break;
        case MENOS:
            //fprintf(code, " - ");
            aux="sub";
            return aux;
            break;
        case VEZES:
            //fprintf(code, " * ");
            aux="mul";
            return aux;
            break;
        case BARRA:
            //fprintf(code, " / ");
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
static void genStmt(TreeNode * tree, FILE *code){
    TreeNode *condicao_do_if, *corpo_do_if, *corpo_do_else,*corpo_do_while,*condicao_do_while, *sentenca_return;

    switch (tree->kind.stmt) {
        case IfK:
            condicao_do_if  = tree->child[0];
            corpo_do_if     = tree->child[1];
            corpo_do_else   = tree->child[2];
            if(condicao_do_if != NULL){
                cGen(condicao_do_if,code);
                no.campo1="if_f";
                no.campo2=indiceTemporario-1;
                no.flagCampo2=1;//Temporario
                no.campo3=contadorDeLinha;
                no.flagCampo3=5;//label
                no.campo4=0;
                no.flagCampo4=0;//vazio
                no.prox=NULL;
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, "(if_f,t%d,L%d,_)\n",indiceTemporario-1,contadorDeLinha);
                contadorDeLinhaIF++;
                indicadorDeLinhaIF[contadorDeLinhaIF]=contadorDeLinha;
                contadorDeLinha++;
                cGen(corpo_do_if,code);
                if(corpo_do_else!=NULL){
                  contadorDeLinhaElse++;
                  indicadorDeLinhaElse[contadorDeLinhaElse]=contadorDeLinha;
                  contadorDeLinha++;
                  no.campo1="got";
                  no.campo2=indicadorDeLinhaElse[contadorDeLinhaElse];
                  no.flagCampo2=5;//label
                  no.campo3=0;
                  no.flagCampo3=0;//vazio
                  no.campo4=0;
                  no.flagCampo4=0;//vazio
                  no.prox=NULL;
                  no.flagVetor = 0;
                  no.vetorPorReferencia = -1;
                  insereFinal(l,no);
                  fprintf(code, "(got,L%d,_,_)\n", indicadorDeLinhaElse[contadorDeLinhaElse]);
                }
                no.campo1="lab";
                no.campo2=indicadorDeLinhaIF[contadorDeLinhaIF];
                no.flagCampo2=5;//label
                no.campo3=0;
                no.flagCampo3=0;//vazio
                no.campo4=0;
                no.flagCampo4=0;//vazio
                no.prox=NULL;
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, "(lab,L%d,_,_)\n",indicadorDeLinhaIF[contadorDeLinhaIF]);
                if(corpo_do_else!=NULL){
                  cGen(corpo_do_else,code);
                  no.campo1="lab";
                  no.campo2=indicadorDeLinhaElse[contadorDeLinhaElse];
                  no.flagCampo2=5;//label
                  no.campo3=0;
                  no.flagCampo3=0;//vazio
                  no.campo4=0;
                  no.flagCampo4=0;//vazio
                  no.prox=NULL;
                  no.flagVetor = 0;
                  no.vetorPorReferencia = -1;
                  insereFinal(l,no);
                  fprintf(code, "(lab,L%d,_,_)\n",indicadorDeLinhaElse[contadorDeLinhaElse]);
                    contadorDeLinhaElse--;
                }
                contadorDeLinhaIF--;
            }
            break;
        case WhileK:
          condicao_do_while  = tree->child[0];
          corpo_do_while     = tree->child[1];
          if(condicao_do_while != NULL){
            no.campo1="lab";
            no.campo2=contadorDeLinha;
            no.flagCampo2=5;//label
            no.campo3=0;
            no.flagCampo3=0;//vazio
            no.campo4=0;
            no.flagCampo4=0;//vazio
            no.prox=NULL;
            no.flagVetor = 0;
            no.vetorPorReferencia = -1;
            insereFinal(l,no);
            fprintf(code, "(lab,L%d,_,_)\n",contadorDeLinha);
            contadorDeLinhaWhile1++;
            indicadorDeLinhaWhile1[contadorDeLinhaWhile1]=contadorDeLinha;
            contadorDeLinha++;
            cGen(condicao_do_while,code);
            no.campo1="if_f";
            no.campo2=indiceTemporario-1;
            no.flagCampo2=1;//Temporario
            no.campo3=contadorDeLinha;
            no.flagCampo3=5;//label
            no.campo4=0;
            no.flagCampo4=0;//vazio
            no.prox=NULL;
            no.flagVetor = 0;
            no.vetorPorReferencia = -1;
            insereFinal(l,no);
            fprintf(code, "(if_f,t%d,L%d,_)\n",indiceTemporario-1,contadorDeLinha);
            contadorDeLinhaWhile2++;
            indicadorDeLinhaWhile2[contadorDeLinhaWhile2]=contadorDeLinha;
            contadorDeLinha++;
            cGen(corpo_do_while,code);
            no.campo1="got";
            no.campo2=indicadorDeLinhaWhile1[contadorDeLinhaWhile1];
            no.flagCampo2=5;//label
            no.campo3=0;
            no.flagCampo3=0;//vazio
            no.campo4=0;
            no.flagCampo4=0;//vazio
            no.prox=NULL;
            no.flagVetor = 0;
            no.vetorPorReferencia = -1;
            insereFinal(l,no);
            fprintf(code, "(got,L%d,_,_)\n",indicadorDeLinhaWhile1[contadorDeLinhaWhile1]);
            contadorDeLinhaWhile1--;
            no.campo1="lab";
            no.campo2=indicadorDeLinhaWhile2[contadorDeLinhaWhile2];
            no.flagCampo2=5;//label
            no.campo3=0;
            no.flagCampo3=0;//vazio
            no.campo4=0;
            no.flagCampo4=0;//vazio
            no.prox=NULL;
            no.flagVetor = 0;
            no.vetorPorReferencia = -1;
            insereFinal(l,no);
            fprintf(code, "(lab,L%d,_,_)\n",  indicadorDeLinhaWhile2[contadorDeLinhaWhile2]);
            contadorDeLinhaWhile2--;
          }
            break;
      case ReturnK:
            sentenca_return = tree->child[0];
            if(sentenca_return != NULL){
              if(sentenca_return->kind.expression == VariavelK || sentenca_return->kind.expression == ConstK){
                no.campo1="ret";
                fprintf(code, "(ret");
                campo=2;
                cGen(sentenca_return,code);
                no.campo3=0;
                no.flagCampo3=0;//vazio
                no.campo4=0;
                no.flagCampo4=0;//vazio
                no.prox=NULL;
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, ",_,_)\n");
              }else{
                cGen(sentenca_return,code);
                no.campo1="ret";
                no.campo2=indiceTemporario-1;
                no.flagCampo2=1;//Temporario
                no.campo3=0;
                no.flagCampo3=0;//vazio
                no.campo4=0;
                no.flagCampo4=0;//vazio
                no.prox=NULL;
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, "(ret,t%d,_,_)\n",indiceTemporario-1);
              }
            }
            break;
    }
}
static void genDecli(TreeNode * tree, FILE *code){
    TreeNode *p1, *p2, *p3;
    no.vetorPorReferencia = -1;
    switch (tree->kind.decl) {
        case VarK:
          break;
        case VetK:

            no.campo1 = "vet";
            if(hashTable[hash(concatena(tree->attr.name, tree->escopo))]==NULL){
                if(hashTable[hash(concatena(tree->attr.name,"programa"))] != NULL){
                    no.campo4 = hash(concatena(tree->attr.name, "programa"));
                }
            }
            else{
                no.campo2 = hash(concatena(tree->attr.name, tree->escopo));
            }
            no.campo3 = 0;
            no.campo4 = 0;
            no.flagCampo2 = 3;
            no.flagCampo3 = 0;
            no.flagCampo4 = 0;
            no.flagVetor = 0;
            no.vetorPorReferencia = -1;
            insereFinal(l, no);

          break;
        case FunK:
            no.campo1="lab";

            no.campo2=hash(concatena(tree->attr.name, tree->attr.name));
            no.flagCampo2=3;//hash
            no.campo3=0;
            no.flagCampo3=0;//vazio
            no.campo4=0;
            no.flagCampo4=0;//vazio
            no.prox=NULL;
            no.flagVetor = 0;
            no.vetorPorReferencia = -1;
            insereFinal(l,no);
            fprintf(code,"(lab,%d,_,_)\n",hash(concatena(tree->attr.name, tree->attr.name)));//apagar
            cGen(tree->child[1],code);
            break;
    }
}
static void genExpression(TreeNode * tree, FILE *code){
    TreeNode *p1, *p2, *p3;
    no.vetorPorReferencia = -1;
    switch (tree->kind.expression) {
        case ConstK:
          if(tree != NULL){
            if(campo == 2){
              no.campo2 =tree->attr.val;
              no.flagCampo2=2;//constante
            }else if(campo == 3){
              no.campo3 =tree->attr.val;
              no.flagCampo3=2;//constante
            }else if(campo ==4){
              no.campo4 =tree->attr.val;
              no.flagCampo4=2;//constante
            }
              fprintf(code,",*%d",tree->attr.val);
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
                no.campo1=printSymbol(tree,code);
                fprintf(code, "(%s",printSymbol(tree,code));
                campo=2;
                cGen(p1,code);
                campo=3;
                cGen(p2,code);
                no.campo4 =indiceTemporario;
                no.flagCampo4=1;//temporario
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code,",t%d)\n",indiceTemporario );
                indiceTemporario++;
              } else if ((p1->kind.expression == ConstK || p1->kind.expression == VariavelK) && (p2->kind.expression == VetorK || p2->kind.expression == CallK)){
                    auxVetR=0;
                    cGen(p2,code);
                    no.campo1 = printSymbol(tree,code);
                    fprintf(code, "(%s",printSymbol(tree,code));//code
                    campo=2;
                    cGen(p1,code);
                    no.campo3 =indiceTemporario-1;
                    no.flagCampo3 =1;//temporario
                    no.campo4=indiceTemporario;
                    no.flagCampo4 =1;//temporario
                    no.flagVetor = 0;
                    no.vetorPorReferencia = -1;
                    insereFinal(l,no);
                    fprintf(code, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );//code
                    indiceTemporario++;
              }else if((p2->kind.expression == ConstK || p2->kind.expression == VariavelK) && (p1->kind.expression == VetorK || p1->kind.expression == CallK)){
                    auxVetR=0;
                    cGen(p1,code);
                    no.campo1 =printSymbol(tree,code);
                    fprintf(code, "(%s",printSymbol(tree,code));//code
                    campo=2;
                    cGen(p2,code);
                    no.campo3 =indiceTemporario-1;
                    no.flagCampo3 =1;//temporario
                    no.campo4=indiceTemporario;
                    no.flagCampo4 =1;//temporario
                    no.flagVetor = 0;
                    no.vetorPorReferencia = -1;
                    insereFinal(l,no);
                    fprintf(code, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );//code
                    indiceTemporario++;
              }else if(p2->kind.expression == OpK && (p1->kind.expression == ConstK || p1->kind.expression == VariavelK)){
                cGen(p2,code);
                no.campo1 =printSymbol(tree,code);
                fprintf(code, "(%s",printSymbol(tree,code));
                campo=2;
                cGen(p1,code);
                no.campo3 =indiceTemporario-1;
                no.flagCampo3 =1;//temporario
                no.campo4=indiceTemporario;
                no.flagCampo4 =1;//temporario
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );
                indiceTemporario++;
              }else if(p1->kind.expression == OpK && (p2->kind.expression == ConstK || p2->kind.expression == VariavelK) ){
                cGen(p1,code);
                no.campo1 =printSymbol(tree,code);
                fprintf(code, "(%s",printSymbol(tree,code));
                campo=2;
                cGen(p2,code);
                no.campo3 =indiceTemporario-1;
                no.flagCampo3 =1;//temporario
                no.campo4=indiceTemporario;
                no.flagCampo4 =1;//temporario
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, ",t%d,t%d)\n",indiceTemporario-1,indiceTemporario );
                indiceTemporario++;
              }else if(p2->kind.expression == OpK && (p1->kind.expression == VetorK || p1->kind.expression == CallK)){
                cGen(p2,code);
                cGen(p1,code);
                no.campo1=printSymbol(tree,code);
                no.campo2=indiceTemporario-2;
                no.flagCampo2=1;//temporario
                no.campo3 =indiceTemporario-1;
                no.flagCampo3 =1;//temporario
                no.campo4=indiceTemporario;
                no.flagCampo4 =1;//temporario
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, "(%s,t%d,t%d,t%d)\n",printSymbol(tree,code),indiceTemporario-2,indiceTemporario-1,indiceTemporario);
                indiceTemporario++;
              }else if(p1->kind.expression == OpK && (p2->kind.expression == VetorK || p2->kind.expression == CallK)){
                cGen(p2,code);
                cGen(p1,code);
                no.campo1=printSymbol(tree,code);
                no.campo2=indiceTemporario-2;
                no.flagCampo2=1;//temporario
                no.campo3 =indiceTemporario-1;
                no.flagCampo3 =1;//temporario
                no.campo4=indiceTemporario;
                no.flagCampo4 =1;//temporario
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, "(%s,t%d,t%d,t%d)\n",printSymbol(tree,code),indiceTemporario-2,indiceTemporario-1,indiceTemporario);
                indiceTemporario++;
              }else{
                auxVetR=0;
                cGen(p2,code);
                cGen(p1,code);
                no.campo1=printSymbol(tree,code);
                no.campo2=indiceTemporario-2;
                no.flagCampo2=1;//temporario
                no.campo3 =indiceTemporario-1;
                no.flagCampo3 =1;//temporario
                no.campo4=indiceTemporario;
                no.flagCampo4 =1;//temporario
                no.flagVetor = 0;
                no.vetorPorReferencia = -1;
                insereFinal(l,no);
                fprintf(code, "(%s,t%d,t%d,t%d)\n",printSymbol(tree,code),indiceTemporario-2,indiceTemporario-1,indiceTemporario);
                indiceTemporario++;
              }
            }
            break;
        case VariavelK:
            if(tree != NULL){
              if(campo == 2){
                no.campo2 =hash(concatena(tree->attr.name, tree->escopo));
                no.flagCampo2=3;//hash
              }else if(campo == 3){
                no.campo3 =hash(concatena(tree->attr.name, tree->escopo));
                no.flagCampo3=3;//hash
              }else if(campo ==4){
                no.campo4 =hash(concatena(tree->attr.name, tree->escopo));
                no.flagCampo4=3;//hash
              }
                fprintf(code, ",%s", tree->attr.name);
              //  fprintf(code, "%s ", tree->attr.name);
            }
            break;
        case EqualK:
          p1 = tree->child[0];
          p2 = tree->child[1];
          if(p1!=NULL && p2!=NULL){
            if(p1->kind.expression == VetorK){
              if(p2->kind.expression == ConstK || p2->kind.expression == VariavelK){//?????????????
                no.campo1=printSymbol(tree,code);
                fprintf(code, "(%s",printSymbol(tree,code));//code
                campo=2;
                cGen(p2,code);
                auxVetR=1;
                campo=3;
                cGen(p1,code);
              }
              else if(p2->kind.expression == OpK || p2->kind.expression == VetorK || p2->kind.expression == CallK){//?????????????????
                cGen(p2,code);
                no.campo1=printSymbol(tree,code);
                no.campo2=indiceTemporario-1;
                no.flagCampo2=1;//temporario
                fprintf(code, "(%s,t%d",printSymbol(tree,code),indiceTemporario-1);
                auxVetR=1;
                campo=3;
                cGen(p1,code);
              }
                // else if(p2->kind.expression == CallK){
                //     cGen(p2,code);
                //     no.campo1=printSymbol(tree,code);
                //     no.campo2=indiceTemporario-1;
                //     no.flagCampo2=1;//temporario
                //     auxVetR=1;
                //     campo=3;
                //     cGen(p1,code);
                // }
            }else{// quando p1 é variavelk
              if(p2->kind.expression == ConstK || p2->kind.expression == VariavelK){
                no.campo1=printSymbol(tree,code);
                fprintf(code, "(%s",printSymbol(tree,code));//code
                campo=2;
                cGen(p2,code);
                auxVetR=0;
                no.campo3=0;
                no.flagCampo3=0;
                fprintf(code, ",_");
                campo=4;
                cGen(p1,code);
                no.flagVetor = 0;
                insereFinal(l,no);
                fprintf(code, ")\n");
              }
              else if(p2->kind.expression == OpK || p2->kind.expression == VetorK || p2->kind.expression == CallK){
                cGen(p2,code);
                no.campo1=printSymbol(tree,code);
                no.campo2=indiceTemporario-1;
                no.flagCampo2=1;//temporario
                fprintf(code, "(%s,t%d",printSymbol(tree,code),indiceTemporario-1);
                auxVetR=0;
                no.campo3=0;
                no.flagCampo3=0;//vazio
                fprintf(code, ",_");
                campo=4;
                cGen(p1,code);
                no.flagVetor = 0;
                insereFinal(l,no);
                fprintf(code, ")\n");
              }
            }
          }
          break;
        case VetorK:
          p1 = tree->child[0];
          if(p1 != NULL){
            // Checa se o vetor não está na main() para fazer passagem de
            // parametro por referencia, quando um vetor for parametro
            // de uma funcao

            if(auxVetR == 1){ // caso o vetor recebe
              if(p1->kind.expression == ConstK || p1->kind.expression == VariavelK){
                campo=3;
                cGen(p1,code);

                if(hashTable[hash(concatena(tree->attr.name, tree->escopo))]==NULL){
                  if(hashTable[hash(concatena(tree->attr.name,"programa"))] != NULL){
                    no.campo4=hash(concatena(tree->attr.name, "programa"));
                  }
                }else{
                  no.campo4=hash(concatena(tree->attr.name, tree->escopo));
                }
                no.flagCampo4=3;//hash
                fprintf(code, ",%s)\n",tree->attr.name);
                no.flagVetor = 1;
                if(strcmp(tree->escopo, "main")){
                    vetorPorReferencia = 1;
                    no.vetorPorReferencia = vetorPorReferencia;
                }

                // Escopo do vetor eh a main()
                else{
                    vetorPorReferencia = 0;
                    no.vetorPorReferencia = vetorPorReferencia;
                }
                insereFinal(l,no);
                auxVetR=0;
              }
              else{
                cGen(p1,code);
                no.campo3= indiceTemporario-1;
                no.flagCampo3=1;//temporario
                if(hashTable[hash(concatena(tree->attr.name, tree->escopo))]==NULL){
                  if(hashTable[hash(concatena(tree->attr.name,"programa"))] != NULL){
                    no.campo4=hash(concatena(tree->attr.name, "programa"));
                  }
                }else{
                  no.campo4=hash(concatena(tree->attr.name, tree->escopo));
                }
                no.flagCampo4=3;//hash
                no.flagVetor = 1;
                if(strcmp(tree->escopo, "main")){
                    vetorPorReferencia = 1;
                    no.vetorPorReferencia = vetorPorReferencia;
                }

                // Escopo do vetor eh a main()
                else{
                    vetorPorReferencia = 0;
                    no.vetorPorReferencia = vetorPorReferencia;
                }
                insereFinal(l,no);
              }
            }

            else{ // caso o vetor seja somado
              if(p1->kind.expression == ConstK || p1->kind.expression == VariavelK){
                no.campo1="asg";
                if(hashTable[hash(concatena(tree->attr.name, tree->escopo))]==NULL){
                  if(hashTable[hash(concatena(tree->attr.name,"programa"))] != NULL){
                    no.campo2=hash(concatena(tree->attr.name, "programa"));
                  }
                }else{
                  no.campo2=hash(concatena(tree->attr.name, tree->escopo));
                }
                no.flagCampo2=3;//hash
                campo=3;
                cGen(p1,code);
                no.campo4=indiceTemporario;
                no.flagCampo4=1;//temporario
                no.flagVetor = 2;
                if(strcmp(tree->escopo, "main")){
                    vetorPorReferencia = 1;
                    no.vetorPorReferencia = vetorPorReferencia;
                }

                // Escopo do vetor eh a main()
                else{
                    vetorPorReferencia = 0;
                    no.vetorPorReferencia = vetorPorReferencia;
                }
                insereFinal(l,no);
                indiceTemporario++;
                auxVetR=0;
              }else{
                cGen(p1,code);
                no.campo1="asg";
                if(hashTable[hash(concatena(tree->attr.name, tree->escopo))]==NULL){
                  if(hashTable[hash(concatena(tree->attr.name,"programa"))] != NULL){
                    no.campo2=hash(concatena(tree->attr.name, "programa"));
                  }
                }else{
                  no.campo2=hash(concatena(tree->attr.name, tree->escopo));
                }
                no.flagCampo2=3;//hash
                no.campo3=indiceTemporario-1;
                no.flagCampo3=1;//temporario
                no.campo4=indiceTemporario;
                no.flagCampo4=1;//temporario
                no.flagVetor = 2;
                if(strcmp(tree->escopo, "main")){
                    vetorPorReferencia = 1;
                    no.vetorPorReferencia = vetorPorReferencia;
                }

                // Escopo do vetor eh a main()
                else{
                    vetorPorReferencia = 0;
                    no.vetorPorReferencia = vetorPorReferencia;
                }
                insereFinal(l,no);
                indiceTemporario++;
            }
          }
        }
          break;
        case CallK:
            p1=tree->child[0];
            while(p1!=NULL){
                // setPosicaoDoParametro(p1->escopo, contparam);
                contparam++;
          	    p1=p1->sibling;
            }

            topoPilha++;
            pilhaQntArgumento[topoPilha]=contparam;
            contparam_aux = 0;
            contparam=0;
            p1=tree->child[0];

            while(p1!=NULL){
                if(p1!=NULL){
                    if(p1->kind.expression == ConstK || p1->kind.expression == VariavelK ){
                        no.campo1="param";
                        campo=2;
                        cGenA(p1,code);
                        no.campo3 = contparam_aux;

                        no.flagCampo3 = 6;//Ordem dos parametros
                        no.campo4 = buscaMemlocDeParametro(tree->attr.name, contparam_aux);
                        // no.flagCampo4 = buscaMemlocDeParametro(p1->escopo, contparam_aux);//vazio
                        contparam_aux++;
                        no.flagCampo4 = 8;
                        no.flagVetor = 0;
                        insereFinal(l,no);
                        fprintf(code,"(param,%d,_,_)\n",p1->attr.val);
                        p1=p1->sibling;
                    }
                }
                if(p1!=NULL){
                    if(p1->kind.expression == VetorK || p1->kind.expression == OpK || p1->kind.expression == CallK){
                        auxVetR=0;
                        cGenA(p1,code);
                        no.campo1="param";
                        no.campo2=indiceTemporario-1;
                        no.flagCampo2=1;//temporario
                        no.campo3 = contparam_aux;
                        no.flagCampo3 = 6;//Ordem dos parametros
                        no.campo4 = buscaMemlocDeParametro(tree->attr.name, contparam_aux);
                        contparam_aux++;
                        no.flagCampo4=8;//vazio
                        no.flagVetor = 0;
                        no.vetorPorReferencia = -1;
                        insereFinal(l,no);
                       	fprintf(code,"(param,t%d,_,_)\n",(indiceTemporario-1));
                        p1=p1->sibling;
                    }
                }

                if(p1!=NULL){
                    if (p1->kind.expression == VariavelK){
                        no.campo1="param";
                        no.campo2=hash(concatena(p1->attr.name, tree->escopo));
                        no.flagCampo2=3;//hash
                        no.campo3 = contparam_aux;

                        no.flagCampo3 = 6;//Ordem dos parametros
                        no.campo4 = buscaMemlocDeParametro(tree->attr.name, contparam_aux);
                        contparam_aux++;
                        no.flagCampo4=8;//vazio
                        no.flagVetor = 0;
                        no.vetorPorReferencia = -1;
                        insereFinal(l,no);
                        fprintf(code,"(param,%s,_,_)\n",p1->attr.name);
                      	p1=p1->sibling;
                    }
                }
            }
          if(tree->tipo==Void){
              no.campo1="call";
              no.campo2=hash(concatena(tree->attr.name, tree->attr.name));
              no.flagCampo2=3;//hash
              no.campo3=pilhaQntArgumento[topoPilha];
              no.flagCampo3=6;//qntParametros
              no.campo4=0;
              no.flagCampo4=0;//vazio
              no.flagVetor = 0;
              no.vetorPorReferencia = -1;
              insereFinal(l,no);
		  	      fprintf(code,"(call,%d,%d,_)\n",hash(concatena(tree->attr.name, tree->escopo)),pilhaQntArgumento[topoPilha]);
              pilhaQntArgumento[topoPilha]=0;
              topoPilha--;

		           }
               else{
                 no.campo1="call";
                 no.campo2=hash(concatena(tree->attr.name, tree->attr.name));
                 no.flagCampo2=3;//hash
                 no.campo3=pilhaQntArgumento[topoPilha];
                 no.flagCampo3=6;//qntParametros
                 no.campo4=indiceTemporario;
                 no.flagCampo4=1;//vazio
                 no.flagVetor = 0;
                 no.vetorPorReferencia = -1;
                 insereFinal(l,no);
		  	         fprintf(code,"(call,%d,%d,t%d)\n",hash(concatena(tree->attr.name, tree->escopo)),pilhaQntArgumento[topoPilha],indiceTemporario);
                 pilhaQntArgumento[topoPilha]=0;
                 topoPilha--;
		  	         indiceTemporario++;
		           }
          break;
        default:
        break;
    }
}
static void cGen( TreeNode * tree, FILE *code){
    if (tree != NULL)
    {
        //Verifica o tipo do no

        switch (tree->nodekind)
        {

        //No para declaracoes
        case StmtK:
        	// fprintf(stdout, "\ncase StmtK :\n");
            genStmt(tree, code);
            break;
        case DeclKi:
            //fprintf(code, "\ncase DeclK :\n");
            genDecli(tree,code);
            break;
        case ExpressionK:
		//fprintf(code, "\ncase ExpressionK :\n");

            genExpression(tree, code);

            break;
        case ParamK:
	        // printf("\ncase ParamK :\n");
            break;
        default:
            break;
        }
        cGen(tree->sibling, code);
    }
}
static void cGenA( TreeNode * tree, FILE *code){
    if (tree != NULL)
    {
        //Verifica o tipo do no

        switch (tree->nodekind)
        {

        //No para declaracoes
        /*case StmtK:
        	//fprintf(code, "\ncase StmtK :\n");
            genStmt(tree, code);
            break;*/
        case DeclKi:
            //fprintf(code, "\ncase DeclK :\n");
            genDecli(tree,code);
            break;
        case ExpressionK:
		//fprintf(code, "\ncase ExpressionK :\n");

            genExpression(tree, code);

            break;
        case ParamK:
	        // printf("\ncase A ParamK :\n");
            // genParam(tree);
            break;
        default:
            break;
        }
    }
}

void codeGen(TreeNode * syntaxTree, char * codefile, FILE *code){
    l=malloc(sizeof(lista));
    inicializaLista(l);
    char * s = malloc(strlen(codefile)+7); //Aloca mais 7 espaços para adicionar a string "File: " e o /0
    strcpy(s,"File: ");
    strcat(s,codefile); //concatena com o nome do arquivo de codigo intermediario
    fprintf(stdout,"\nCMINUS Compilation\n\n"); //imprime no terminal
    cGen(syntaxTree, code); //Funcao principal de geracao
    fprintf(listing,"\n\nEnd of execution.\n\n");
    imprimeLista(l);
}
