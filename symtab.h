#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

 /* the list of line numbers of the source
  * code in which a variable is referenced
  */
 typedef struct LineListRec
 {
     int lineno;
     struct LineListRec * next;
 } * LineList;

 /* The record in the bucket lists for
  * each variable, including name,
  * assigned memory location, and
  * the list of line numbers in which
  * it appears in the source code
  */
 typedef struct BucketListRec
 {
    char * name;
    char * name2;
    int posicaoDoParametro;
    LineList lines;
    int memloc ;
    char * escopo;
    TipType tipoId;
    ExpType tipo;
    struct BucketListRec * next;
 } * BucketList;

void st_insert_first( char * name, char * name2, int lineno, int loc, char * escopo , ExpType tipo, TipType tipoId, int posicaoDoParametro);
void st_insert( char * name, int lineno);
char* st_lookupEscopo ( char * name );
ExpType st_lookupTipo ( char * name );
TipType st_lookupTipoId ( char * name );
void setPosicaoDoParametro(char * escopo, int posicao);

/* the hash table */
BucketList hashTable[SIZE];

/* Function st_lookup returns the memory
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE * listing);


#endif
