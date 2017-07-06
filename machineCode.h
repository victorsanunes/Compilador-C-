#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include "symtab.h"
#include "globals.h"

extern lista *l;
extern FILE *listing;
extern FILE *stdout;
extern FILE *code;
//extern BucketList hashTable[SIZE];

//Primeiro codigo de maquina gerado
lista *codigo_de_maquina;
lista *codigo_de_maquina2;

quadra *elemento2;

void machineCodeGen();

void binaryConverter(lista *l);
