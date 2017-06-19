#include "machineCode.h"

//OPCODES
#define ADD 	"000000"
#define ADDI 	"000001"
#define SUB 	"000010"
#define SUBI 	"000011"
#define AND		"000100"
#define ANDI	"000101"
#define OR 		"000110"
#define ORI 	"000111"
#define NOT 	"001000"
#define XOR		"001001"
#define JUMPR	"001011"
#define MOVE	"001101"
#define NOP 	"001111"
#define HALT	"010000"
#define SLT		"010001"
#define SLTI 	"010010"
#define BEQ		"010011"
#define BNE		"010100"
#define IN 		"010101"
#define OUT 	"010110"
#define LOAD 	"010111"
#define LOADI	"011000"
#define STORE	"011001"
#define SLE		"011010"
#define SLEI 	"011011"
#define SHE		"011100"
#define SHEI	"011101"
#define SHT		"011110"
#define SHTI	"011111"
#define SETI	"100000"
#define SET		"100001"
#define IM16_01 0000000000000001
#define BR_SIZE 32
#define TEMPORARIOS_QTD 300

int bancoDeRegistradores[BR_SIZE];
int registradoresTemporarios[TEMPORARIOS_QTD];
int posicaoMemoriaDeInstrucoes = 3;
int posicaoDeFuncoes[SIZE];
int pilhaParaRetornoDeFuncao[SIZE];
int topoPilhaParaRetornoDeFuncao = 0;
int fimDoBancoDeRegistradores = BR_SIZE - 1;

void resetRegistradores(int *v, int size){
	int i;
	for(i = 0; i < size; i++){
		v[i] = -1;
	}
}

void quadraReset(quadra *q){
	q->campo1 = NULL;
	// q->campo2 = -1;
	// q->campo3 = -1;
	// q->campo4 = -1;
	q->flagCampo2 = -1;
	q->flagCampo3 = -1;
	q->flagCampo4 = -1;
	q->prox = NULL;
}

int buscaRegistradorVazio(int *v, int size){
	int i = 0;
	while (v[i] != -1 && i < size){
		i++;
	}
	if(i >= size)
		{return -1;}
	else
		{return i; }
}

void setRegistradorUsado(int *registrador, int posicao){
	registrador[posicao] = 1;
}
void setRegistradorLivre(int *registrador, int posicao){
	registrador[posicao] = -1;
}

void machineCodeGen(){

	codigo_de_maquina = malloc(sizeof(lista));
	inicializaLista(codigo_de_maquina);
	elemento2 = malloc(sizeof(quadra));
	quadraReset(elemento2);
	resetRegistradores(bancoDeRegistradores, BR_SIZE);
	resetRegistradores(registradoresTemporarios, TEMPORARIOS_QTD);

	BucketList b = (BucketList) malloc(sizeof(struct BucketListRec));
	quadra *p = l->inicio;
	quadra *aux;
	int hashIndex;
	int i, reg1, reg2, reg3, reg4;
	int chamadaDeFuncao;
	int funcaoComRetorno = -1;
	int primeiraFuncaoDeclarada = 0;
	int registradoresParaParametros = 3;
	int flagJump = -1; //-1: invalido | 0: zero | 1: negative | 2:ambos |3:diferente
	int qtd_instrucoes = 0;
	int qtd_instrucoes_aux;
	printf("\n\nCÓDIGO OBJETO\n");
	while(p != NULL){
		if(!strcmp(p->campo1, "lab")){
			primeiraFuncaoDeclarada++;
			// if(funcaoComRetorno == -1 ){
			// 	funcaoComRetorno = 2;
			// }
			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					//Checa se a fucao anterior teve retorno
					// if( funcaoComRetorno == 1){
					// 	elemento2->campo1 = "MOVE";
					// 	reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					// 	if(reg1 != -1){
					// 		elemento2->campo2 = reg1;
					// 		elemento2->campo3 = 0;
					// 		elemento2->campo4 = fimDoBancoDeRegistradores - 2;
					// 		elemento2->flagCampo2 = 7;
					// 		elemento2->flagCampo3 = 0;
					// 		elemento2->flagCampo4 = 7;
					// 		insereFinal(codigo_de_maquina, *elemento2);
					// 		posicaoMemoriaDeInstrucoes++;
					// 		quadraReset(elemento2);
					// 	}
					//
					// 	elemento2->campo1 = "JUMPR";
					// 	elemento2->campo2 = reg1;
					// 	elemento2->campo3 = reg1;
					// 	elemento2->campo4 = 0;
					// 	elemento2->flagCampo2 = 7;
					// 	elemento2->flagCampo3 = 7;
					// 	elemento2->flagCampo4 = 0;
					// 	insereFinal(codigo_de_maquina, *elemento2);
					// 	posicaoMemoriaDeInstrucoes++;
					// 	quadraReset(elemento2);
					// 	setRegistradorLivre(bancoDeRegistradores, reg1);
					// 	funcaoComRetorno = 0;
					// }

					//Checa par imprimir jump de volta a posicao de chamada
					if(primeiraFuncaoDeclarada > 1){
						elemento2->campo1 = "JUMPR";
						elemento2->campo2 = fimDoBancoDeRegistradores - 1;
						elemento2->campo3 = fimDoBancoDeRegistradores - 1;
						elemento2->campo4 = 0;
						elemento2->flagCampo2 = 7;
						elemento2->flagCampo3 = 7;
						elemento2->flagCampo4 = 0;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
						quadraReset(elemento2);
						setRegistradorLivre(bancoDeRegistradores, reg1);
						funcaoComRetorno = 0;

					}

					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					posicaoDeFuncoes[b->memloc] = posicaoMemoriaDeInstrucoes + 1;
					elemento2->campo1 = "LAB";
					elemento2->campo2 = p->campo2;
					elemento2->campo3 = 0;
					elemento2->campo4 = 0;
					elemento2->flagCampo2 = 3;
					elemento2->flagCampo3 = 0;
					elemento2->flagCampo4 = 0;
					fprintf(stdout, "%d\n", posicaoMemoriaDeInstrucoes);
					insereFinal(codigo_de_maquina, *elemento2);
					quadraReset(elemento2);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					elemento2->campo1 = "LAB";
					elemento2->campo2 = p->campo2;
					// qtd_instrucoes = p->campo2;
					elemento2->campo3 = -1;
					elemento2->campo4 = -1;
					elemento2->flagCampo2 = 5;
					elemento2->flagCampo3 = 0;
					elemento2->flagCampo4 = 0;
					insereFinal(codigo_de_maquina, *elemento2);
					quadraReset(elemento2);
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "asg")){
			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					if(chamadaDeFuncao){
						elemento2->campo2 = reg1;
						elemento2->flagCampo2 = 7;
						chamadaDeFuncao = 0;
					}

					break;

				//Constante
				case 2:
					elemento2->campo1 = "LOADI";
					elemento2->campo4 = p->campo2;
					elemento2->flagCampo4 = 2;
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
						//quadraReset(elemento2);
					}
					break;

				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo2 = b->memloc;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->flagCampo3 = 7;
						elemento2->campo3 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg3 = registradoresTemporarios[p->campo3];
					break;

				//Constante
				case 2:
					elemento2->campo1 = "LOADI";
					elemento2->campo4 = p->campo3;
					elemento2->flagCampo4 = 2;
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg2 != -1){
						elemento2->campo2 = reg2;
						setRegistradorUsado(bancoDeRegistradores, reg2);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;

				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo2 = b->memloc;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					elemento2->flagCampo3 = 0;
					if(reg2 != -1){
						elemento2->campo4 = reg2;
						setRegistradorUsado(bancoDeRegistradores, reg2);
						elemento2->flagCampo4 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						qtd_instrucoes++;
					}

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					elemento2->campo4 = hashTable[p->campo4]->memloc;
					elemento2->flagCampo4 = 8;
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			elemento2->campo1 = "STORE";
			elemento2->campo3 = 0;
			elemento2->flagCampo3 = 0;

			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			setRegistradorLivre(bancoDeRegistradores, reg1);
			quadraReset(elemento2);
		}

		else if(!strcmp(p->campo1, "igual")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg1 = registradoresTemporarios[p->campo2];
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->flagCampo2 = 7;
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					elemento2->campo1 = "LOADI";
					elemento2->campo4 = p->campo3;
					elemento2->flagCampo4 = 2;
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg2 != -1){
						elemento2->campo2 = reg2;
						setRegistradorUsado(bancoDeRegistradores, reg2);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
						quadraReset(elemento2);
					}
					break;

				//Hash
				case 3:

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					registradoresTemporarios[p->campo4] = reg3;
					setRegistradorUsado(bancoDeRegistradores, reg3);
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			elemento2->campo1 = "SET";
			elemento2->campo2 = reg3;
			elemento2->campo3 = reg2;
			elemento2->campo4 = reg1;
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 7;
			elemento2->flagCampo4 = 7;
			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			setRegistradorLivre(bancoDeRegistradores, reg1);
			setRegistradorLivre(bancoDeRegistradores, reg2);
		}

		else if( (!strcmp(p->campo1, "menorI")) || (!strcmp(p->campo1, "maiorI")) ){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->flagCampo2 = 7;
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->flagCampo2 = 7;
						elemento2->campo2 = reg2;
						setRegistradorUsado(bancoDeRegistradores, reg2);
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					registradoresTemporarios[p->campo4] = reg3;
					setRegistradorUsado(bancoDeRegistradores, reg3);
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			if(!strcmp(p->campo1, "menorI")){
				elemento2->campo1 = "SLE";
			}
			else{
				elemento2->campo1 = "SHE";
			}

			elemento2->campo2 = reg3;
			elemento2->campo3 = reg2;
			elemento2->campo4 = reg1;
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 7;
			elemento2->flagCampo4 = 7;
			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			quadraReset(elemento2);
			setRegistradorLivre(bancoDeRegistradores, reg1);
			setRegistradorLivre(bancoDeRegistradores, reg2);
		}

		else if(!strcmp(p->campo1, "dif")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo3;
					b = hashTable[hashIndex];

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "menor")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];

					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo3;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\na--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "maior")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo3;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\na--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "sum") || !strcmp(p->campo1, "sub")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg1 = registradoresTemporarios[p->campo2];
					break;

				//Constante
				case 2:
					elemento2->campo1 = "LOADI";
					elemento2->campo4 = p->campo2;
					elemento2->flagCampo4 = 2;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo1 = "LOAD";
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg2 = registradoresTemporarios[p->campo3];
					break;

				//Constante
				case 2:
					elemento2->campo1 = "LOADI";
					elemento2->campo4 = p->campo3;
					elemento2->flagCampo4 = 2;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg2 != -1){
						elemento2->campo2 = reg2;
						setRegistradorUsado(bancoDeRegistradores, reg2);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;
				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					if(reg2 != -1){
						elemento2->campo2 = reg2;
						setRegistradorUsado(bancoDeRegistradores, reg2);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					registradoresTemporarios[p->campo4] = reg3;
					setRegistradorUsado(bancoDeRegistradores, reg3);
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			if(!strcmp(p->campo1, "sum")){
				elemento2->campo1 = "ADD";
			}
			else{
				elemento2->campo1 = "SUB";
			}
				elemento2->campo2 = reg3;
				elemento2->campo3 = reg2;
				elemento2->campo4 = reg1;
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 7;
				elemento2->flagCampo4 = 7;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				setRegistradorLivre(bancoDeRegistradores, reg1);
				setRegistradorLivre(bancoDeRegistradores, reg2);

		}

		else if(!strcmp(p->campo1, "mul")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo3;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\na--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "div")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo3;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\na--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "if_f")){
			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					// reg2 = registradoresTemporarios[p->campo2];
					elemento2->campo1 = "LOADI";
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					//Guarda o valor constante 1
					elemento2->campo4 = IM16_01; //Constante 1
					elemento2->flagCampo4 = 2;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						elemento2->flagCampo2 = 7;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
						quadraReset(elemento2);
					}
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {

				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					reg2 = registradoresTemporarios[p->campo3];
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					qtd_instrucoes = p->campo3;
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\na--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			//Conta as instrucoes dentro do if
			// aux = p;
			// qtd_instrucoes_aux = 2;
			// while(strcmp(aux->campo1, "lab") != 0){
			// 	qtd_instrucoes_aux++;
			// 	aux = aux->prox;
			// }

			//Gera a instrucao com o jump
			elemento2->campo1 = "BNE";
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 7;
			elemento2->flagCampo4 = 5;
			elemento2->campo2 = reg1;
			elemento2->campo3 = reg3;
			reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
			elemento2->campo4 = qtd_instrucoes;
			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			quadraReset(elemento2);
			setRegistradorLivre(bancoDeRegistradores, reg1);
			setRegistradorLivre(bancoDeRegistradores, reg2);
			setRegistradorLivre(bancoDeRegistradores, reg3);
		}

		else if(!strcmp(p->campo1, "got")){

			switch (p->flagCampo2) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo3) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo3;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\n--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}

			switch (p->flagCampo4) {
				//Vazio
				case 0:
					break;
				//Temporario
				case 1:
					break;

				//Constante
				case 2:
					break;

				//Hash
				case 3:
					hashIndex = p->campo4;
					b = hashTable[hashIndex];
					if(b != NULL)
						printf("tem gente\n" );
					printf("\na--%d--\n", b->memloc);
					break;
				//Char
				case 4:
					break;
				//Label
				case 5:
					break;
				//Quantidade de parametros
				case 6:
					break;
				//Inativo
				case -1:
					break;
			}
		}

		else if(!strcmp(p->campo1, "param")){
			switch (p->flagCampo2) {

				case 2:
					break;

				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo1 = "LOAD";
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
					}
					break;
			}
			registradoresParaParametros++;
		}

		else if(!strcmp(p->campo1, "call")){

			chamadaDeFuncao = 1;

			//Carrega a posicao de jump em um registrador
			elemento2->campo1 = "LOADI";
			reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
			if(reg2 != -1){
				elemento2->campo2 = reg2;
				elemento2->campo3 = 0;

				hashIndex = p->campo2;
				b = hashTable[hashIndex];

				elemento2->campo4 = posicaoDeFuncoes[b->memloc] - 1;
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 0;
				elemento2->flagCampo4 = 9;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				quadraReset(elemento2);
			}

			//Carrega a posicao de volta para o registrador 30
			elemento2->campo1 = "LOADI";
			elemento2->campo2 = fimDoBancoDeRegistradores - 1;
			elemento2->campo3 = 0;
			elemento2->campo4 = posicaoMemoriaDeInstrucoes + 1;
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 0;
			elemento2->flagCampo4 = 9;
			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			quadraReset(elemento2);

			//Gera a instrucao de jump para a posicao na memoria de instrucoes
			elemento2->campo1 = "JUMPR";
			elemento2->campo2 = reg2;
			elemento2->campo3 = reg2;
			elemento2->campo4 = 0;
			hashIndex = p->campo2;
			b = hashTable[hashIndex];
			elemento2->campo4 = 0;
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 7;
			elemento2->flagCampo4 = 0;
			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			setRegistradorLivre(bancoDeRegistradores, reg2);
			quadraReset(elemento2);

			switch (p->flagCampo2) {
				//Indica a posicao da memoria da funcao chamada
				case 3:


					break;

			}

			//Indica a quantidade de parametros
			switch (p->flagCampo3) {
				case 0:
					break;
			}

			//Indica o temporario para armezenar o retorno
			switch (p->flagCampo4) {
				case 1:
					///funcaoComRetorno = 1;
					// reg1 = registradoresTemporarios[p->campo2];
					elemento2->campo1 = "MOVE";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo4 = fimDoBancoDeRegistradores;
					elemento2->flagCampo4 = 7;
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						setRegistradorUsado(bancoDeRegistradores, reg1);
						elemento2->flagCampo2 = 7;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
						quadraReset(elemento2);
					}
					break;
			}
		}

		else if(!strcmp(p->campo1, "ret")){
			funcaoComRetorno = 1;
			switch (p->flagCampo2) {
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					// reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);

					elemento2->flagCampo2 = 7;
					elemento2->campo2 = fimDoBancoDeRegistradores;
					setRegistradorUsado(bancoDeRegistradores, fimDoBancoDeRegistradores);
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;
					break;
			}
		}
		p = p->prox;
	}
	imprimeLista(codigo_de_maquina);
}
