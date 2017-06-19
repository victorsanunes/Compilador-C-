#include "lista.h"

void inicializaLista(lista * l){
  l->inicio = NULL;
  l->fim = NULL;
  l->tam=0;
}

void insereFinal( lista *l, quadra novo){
    quadra *no = malloc(sizeof(quadra));
    no->campo1 = novo.campo1;
    no->campo2 = novo.campo2;
    no->campo3 = novo.campo3;
    no->campo4 = novo.campo4;
    no->flagCampo2=novo.flagCampo2;
    no->flagCampo3=novo.flagCampo3;
    no->flagCampo4=novo.flagCampo4;
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
    while(p != NULL){
        printf("(%s,%d,%d,%d) (flag1:%d,flag2:%d,flag3:%d)\n",p->campo1,p->campo2,p->campo3,p->campo4,p->flagCampo2,p->flagCampo3,p->flagCampo4);
        p = p->prox;
    }
}
