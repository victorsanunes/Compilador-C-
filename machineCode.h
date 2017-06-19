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
lista *codigo_de_maquina;
quadra *elemento2; 

void machineCodeGen();
