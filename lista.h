#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct quadra{
    char *campo1;
    int campo2;
    int campo3;
    int campo4;
    int flagCampo2; //0: Vazio | 1: Temporario | 2: Constante |3: Hash |4: Char | 5:label | 6:qntParametros |-1: Inativa
    int flagCampo3; //0: Vazio | 1: Temporario | 2: Constante |3: Hash |4: Char | 5:label | 6:qntParametros |-1: Inativa
    int flagCampo4; //0: Vazio | 1: Temporario | 2: Constante |3: Hash |4: Char | 5:label | 6:qntParametros |-1: Inativa
    int flagVetor; //0: Nao tem vetor | 1: Vetor recebe | 2: Vetor como operando
    int vetorPorReferencia;
    struct quadra * prox;
}quadra;


//quadra *no1;

typedef struct lista{
    quadra * inicio;
    quadra * fim;
    int tam;
}lista;

lista *l;

void inicializaLista(lista * l);
void insereFinal( lista *l, quadra novo);
void imprimeLista(lista *l);
void imprimeListaFormatada(lista *l);
