#include "global.h"
#include "util.h"
#include "scan.h"
#include "parse.h"


/* Alocando as variáveis globais */
int lineno = 0;
FILE * source;


main( int argc, char * argv[] )
{
	char sourceName[120];

	if (argc != 2){
		fprintf(stderr,"Uso: %s <arquivo>\n",argv[0]);
		exit(1);
	}

	strcpy(sourceName,argv[1]) ;

	if (strchr (sourceName, '.') == NULL)
		strcat(sourceName,".txt");
	
	source = fopen(sourceName,"r");

	if (source==NULL){
		fprintf(stderr,"Arquivo %s nao encontrado\n",sourceName);
		exit(1);
	}

  extern int yydebug;
  yydebug = 0;

	TreeNode * syntaxTree;
	syntaxTree = parse();

	printf("\n======== Arvore Sintatica ========\n");

	printTree(syntaxTree);

	printf("\n\n\n");
	return 0;
}

