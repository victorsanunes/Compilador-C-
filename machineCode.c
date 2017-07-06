#include "machineCode.h"

//OPCODES
#define ADD 	/*000000*/ "6'd0"
#define ADDI 	/*000001*/ "6'd1"
#define SUB 	/*000010*/ "6'd2"
#define SUBI 	/*000011*/ "6'd3"
#define AND		/*000100*/ "6'd4"
#define ANDI	/*000101*/ "6'd5"
#define OR 		/*000110*/ "6'd6"
#define ORI 	/*000111*/ "6'd7"
#define NOT 	/*001000*/ "6'd8"
#define XOR		/*001001*/ "6'd9"
#define JUMPR	/*001011*/ "6'd11"
#define MOVE	/*001101*/ "6'd13"
#define NOP 	/*001111*/ "6'd15"
#define HALT	/*010000*/ "6'd16"
#define SLT		/*010001*/ "6'd17"
#define SLTI 	/*010010*/ "6'd18"
#define BEQ		/*010011*/ "6'd19"
#define BNE		/*010100*/ "6'd20"
#define IN 		/*010101*/ "6'd21"
#define OUT 	/*010110*/ "6'd22"
#define LOAD 	/*010111*/ "6'd23"
#define LOADI	/*011000*/ "6'd24"
#define STORE	/*011001*/ "6'd25"
#define SLE		/*011010*/ "6'd26"
#define SLEI 	/*011011*/ "6'd27"
#define SHE		/*011100*/ "6'd28"
#define SHEI	/*011101*/ "6'd29"
#define SHT		/*011110*/ "6'd30"
#define SHTI	/*011111*/ "6'd31"
#define SETI	/*100000*/ "6'd32"
#define SET		/*100001*/ "6'd33"
#define LOADR 	"6'd34"
#define STORER	"6'd35"
#define IM16_01 0000000000000001
#define BR_SIZE 32
#define TEMPORARIOS_QTD 300

int bancoDeRegistradores[BR_SIZE];
int registradoresTemporarios[TEMPORARIOS_QTD];
int posicaoMemoriaDeInstrucoes = 9; //Quantidade de instrucoes iniciais na memoria de instrucoes
int posicaoDeFuncoes[SIZE];
int pilhaParaRetornoDeFuncao[SIZE];
int topoPilhaParaRetornoDeFuncao = 0;
int fimDoBancoDeRegistradores = BR_SIZE - 1;
int enderecoLabel[SIZE];

void binaryConverter(lista *l){
	quadra *p = l->inicio;
	int i = 0;
	while(p != NULL){
		printf("memoriaInstrucoes[%d] <= { ", i);
		if(!strcmp(p->campo1, "ADD") || !strcmp(p->campo1, "SUB")){
			if(!strcmp(p->campo1, "ADD"))
				printf("%s, ", ADD);
			else{
				printf("%s, ", SUB);
			}
			printf("5'd%d, ", p->campo2);
			printf("5'd%d, ", p->campo3);
			printf("5'd%d, ", p->campo4);
			printf("11'd0 ");
		}
		else if(!strcmp(p->campo1, "NOP")){
			printf("%s, ", NOP);
			printf("26'd0 ");
		}
		else if(!strcmp(p->campo1, "IN")){
			printf("%s, ", IN);
			printf("5'd%d, ", p->campo2);
			printf("21'd0 ");
		}
		else if(!strcmp(p->campo1, "OUT")){
			printf("%s, ", OUT);
			printf("5'd%d, ", p->campo2 );
			printf("21'd0 ");
		}
		else if(!strcmp(p->campo1, "LOAD")){
			printf("%s, ", LOAD);
			printf("5'd%d, ", p->campo2);
			printf("21'd%d ", p->campo4);
		}
		else if(!strcmp(p->campo1, "LOADI")){
			printf("%s, ", LOADI);
			printf("5'd%d, ",  p->campo2);
			printf("21'd%d ", p->campo4);
		}
		else if(!strcmp(p->campo1, "JUMPR")){
			printf("%s, ", JUMPR);
			printf("5'd%d, ", p->campo2);
			printf("21'd0 " );
		}
		else if(!strcmp(p->campo1, "STORE")){
			printf("%s, ", STORE);
			printf("5'd%d, ", p->campo2);
			printf("21'd%d ", p->campo4);
		}
		else if(!strcmp(p->campo1, "HLT")){
			printf("%s, ", HALT);
			printf("26'd0 ");
		}

		printf("};\n");
		p = p->prox;
		i++;
	}
}

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

void imprimeQuadra(quadra *p){
	printf("====(%s,%d,%d,%d) (flag1:%d,flag2:%d,flag3:%d, flagVetor:%d)====\n",p->campo1,p->campo2,p->campo3,p->campo4,p->flagCampo2,p->flagCampo3,p->flagCampo4, p->flagVetor);
}

void machineCodeGen(){

	codigo_de_maquina = malloc(sizeof(lista));
	codigo_de_maquina2 = malloc(sizeof(lista));

	inicializaLista(codigo_de_maquina);
	inicializaLista(codigo_de_maquina2);
	elemento2 = malloc(sizeof(quadra));
	quadraReset(elemento2);
	resetRegistradores(bancoDeRegistradores, BR_SIZE);
	resetRegistradores(registradoresTemporarios, TEMPORARIOS_QTD);
	resetRegistradores(enderecoLabel, SIZE);

	BucketList b = (BucketList) malloc(sizeof(struct BucketListRec));

	//l eh o codigo intermediario
	quadra *p = l->inicio;
	quadra *aux;
	int hashIndex;
	int i, reg1, reg2, reg3, reg4;
	int chamadaDeFuncao;
	int funcaoComRetorno = -1;
	int primeiraFuncaoDeclarada = 0;
	int memlocFuncaoAnterior = -1;
	int memlocFuncaoAtual = -1;
	int flagJump = -1; //-1: invalido | 0: zero | 1: negative | 2:ambos |3:diferente
	int flagVetor = -1; //0: recebendo | 1: operando
	int qtd_instrucoes = 0;
	int qtd_instrucoes_aux;
	int flagDiferente = -1; //flag para instrucao de igual e diferente

	//Gera o primeiro codigo objeto
	printf("\n\nCÓDIGO OBJETO 1\n");
	while(p != NULL){
		if(!strcmp(p->campo1, "lab")){


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

					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					memlocFuncaoAnterior = memlocFuncaoAtual;
					memlocFuncaoAtual = b->memloc;
					primeiraFuncaoDeclarada++;

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
						hashIndex = p->campo2;
						b = hashTable[hashIndex];

						//Guarda o valor do retorno da funcao
						elemento2->campo1 = "LOAD";
						// elemento2->campo2 = fimDoBancoDeRegistradores;
						reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
						elemento2->campo2 = reg1;
						elemento2->campo4 = memlocFuncaoAnterior;
						elemento2->campo3 = 0;
						elemento2->flagCampo2 = 7;
						elemento2->flagCampo3 = 0;
						elemento2->flagCampo4 = 8;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;

						hashIndex = p->campo2;
						b = hashTable[hashIndex];

						elemento2->campo1 = "JUMPR";
						elemento2->campo2 = reg1;
						elemento2->campo3 = reg1;
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
					// posicaoDeFuncoes[p->campo2] = posicaoMemoriaDeInstrucoes - 3;
					posicaoDeFuncoes[p->campo2] = posicaoMemoriaDeInstrucoes;

					elemento2->campo1 = "LAB";
					elemento2->campo2 = p->campo2;
					elemento2->campo3 = 0;
					elemento2->campo4 = 0;
					elemento2->flagCampo2 = 3;
					elemento2->flagCampo3 = 0;
					elemento2->flagCampo4 = 0;
					// fprintf(stdout, "%d\n", posicaoMemoriaDeInstrucoes);
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
					enderecoLabel[p->campo2] = posicaoMemoriaDeInstrucoes;
					// qtd_instrucoes = p->campo2;
					elemento2->campo3 = 0;
					elemento2->campo4 = 0;
					elemento2->flagCampo2 = 5;
					elemento2->flagCampo3 = 0;
					elemento2->flagCampo4 = 0;
					insereFinal(codigo_de_maquina, *elemento2);
					// imprimeQuadra(elemento2);
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
					reg1 = registradoresTemporarios[p->campo2];
					break;

				//Constante
				case 2:
					//Carrega a constante em um reg
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
						// quadraReset(elemento2);
					}
					break;

				//Hash
				case 3:
					if(p->flagVetor == 0) {// flagVetor = 1;
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
					}

					else if(p->flagVetor == 2){

						//Carrega o valor do memloc
						elemento2->campo1 = "LOADI";
						reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
						if(reg1 != -1){
							elemento2->campo2 = reg1;
							elemento2->flagCampo2 = 7;
							elemento2->campo3 = 0;
							elemento2->flagCampo3 = 0;
							elemento2->campo4 = hashTable[p->campo2]->memloc;
							elemento2->flagCampo4 = 8;
							setRegistradorUsado(bancoDeRegistradores, reg1);
							insereFinal(codigo_de_maquina, *elemento2);
							posicaoMemoriaDeInstrucoes++;
						}
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
					//Carrega a constante em um reg
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
						// quadraReset(elemento2);
					}
					break;

				//Hash
				case 3:
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo3;
					b = hashTable[hashIndex];
					elemento2->campo4 = b->memloc;
					elemento2->flagCampo4 = 8;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
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
					// hashIndex = p->campo4;
					// b = hashTable[hashIndex];
					//
					//
					// elemento2->campo4 = b->memloc;
					// elemento2->flagCampo4 = 8;

					// elemento2->campo1 = "++ADDI";
					// elemento2->campo3 = reg2;
					// elemento2->flagCampo3 = 7;
					// reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					// if(reg3 != -1){
					// 	elemento2->campo2 = reg3;
					// 	elemento2->flagCampo2 = 7;
					// 	setRegistradorUsado(bancoDeRegistradores, reg3);
					// 	insereFinal(codigo_de_maquina, *elemento2);
					// 	posicaoMemoriaDeInstrucoes++;
					// }

					/*elemento2->campo1 = "STORER";
					elemento2->campo2 = reg3;
					elemento2->campo3 = reg1;
					elemento2->campo4 = 0;
					elemento2->flagCampo2 = 7;
					elemento2->flagCampo3 = 7;
					elemento2->flagCampo4 = 0;
					*/


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

			if(p->flagVetor == 0){
				elemento2->campo1 = "STORE";
				elemento2->campo2 = reg1;
				elemento2->campo3 = 0;
				elemento2->campo4 = hashTable[p->campo4]->memloc;
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 0;
				elemento2->flagCampo4 = 8;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				setRegistradorLivre(bancoDeRegistradores, reg3);
				// imprimeQuadra(elemento2);

			}
			else if(p->flagVetor == 1){
				//Calcula o descolamento
				elemento2->campo1 = "ADDI";
				elemento2->campo4 = hashTable[p->campo4]->memloc;
				elemento2->flagCampo4 = 8;
				elemento2->campo3 = reg2;
				elemento2->flagCampo3 = 7;
				reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
				if(reg3 != -1){
					elemento2->campo2 = reg3;
					setRegistradorUsado(bancoDeRegistradores, reg3);
					elemento2->flagCampo2 = 7;
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;
					// quadraReset(elemento2);
				}

				elemento2->campo1 = "STORER";
				elemento2->campo2 = reg3;
				elemento2->campo3 = 0;
				elemento2->campo4 = reg1;
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 0;
				elemento2->flagCampo4 = 7;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				setRegistradorLivre(bancoDeRegistradores, reg1);
				setRegistradorLivre(bancoDeRegistradores, reg2);
				setRegistradorLivre(bancoDeRegistradores, reg3);
			}

			else if(p->flagVetor == 2){
				//Calcula o descolamento
				elemento2->campo1 = "ADD";
				// reg3 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
				if(reg3 != -1){
					elemento2->campo2 = reg3;
					setRegistradorUsado(bancoDeRegistradores, reg3);
					elemento2->flagCampo2 = 7;
					elemento2->campo3 = reg2;
					elemento2->campo4 = reg1;
					elemento2->flagCampo3 = 7;
					elemento2->flagCampo4 = 7;
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;
				}

				setRegistradorLivre(bancoDeRegistradores, reg1);
				setRegistradorLivre(bancoDeRegistradores, reg2);

				reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
				setRegistradorUsado(bancoDeRegistradores, reg1);

				elemento2->campo1 = "LOADR";
				elemento2->campo2 = reg1;
				elemento2->campo4 = reg3;
				elemento2->campo3 = 0;
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 0;
				elemento2->flagCampo4 = 7;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				//reg1 = reg3;
				registradoresTemporarios[p->campo4] = reg1;
				setRegistradorLivre(bancoDeRegistradores, reg2);
				setRegistradorLivre(bancoDeRegistradores, reg3);
			}
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

			flagDiferente = 1;
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
					//Carrega a constante em um reg
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
						// quadraReset(elemento2);
					}
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
					//Carrega a constante em um reg
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
						// quadraReset(elemento2);
					}
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
			flagDiferente = 1;
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
			flagDiferente = 0;
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
					//Carrega a constante em um reg
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
						// quadraReset(elemento2);
					}
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

		else if((!strcmp(p->campo1, "menor")) ||  (!strcmp(p->campo1, "maior")) ){

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
					//Carrega a constante em um reg
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
						// quadraReset(elemento2);
					}
					break;
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

			if(!strcmp(p->campo1, "menor")){
				elemento2->campo1 = "SLT";
			}
			else if(!strcmp(p->campo1, "maior")){
				elemento2->campo1 = "SHT";
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

		// else if(!strcmp(p->campo1, "maior")){
		//
		// 	switch (p->flagCampo2) {
		// 		//Vazio
		// 		case 0:
		// 			break;
		// 		//Temporario
		// 		case 1:
		// 			break;
		//
		// 		//Constante
		// 		case 2:
		// 			break;
		//
		// 		//Hash
		// 		case 3:
		// 			hashIndex = p->campo2;
		// 			b = hashTable[hashIndex];
		// 			if(b != NULL)
		// 				printf("tem gente\n" );
		// 			printf("\n--%d--\n", b->memloc);
		// 			break;
		// 		//Char
		// 		case 4:
		// 			break;
		// 		//Label
		// 		case 5:
		// 			break;
		// 		//Quantidade de parametros
		// 		case 6:
		// 			break;
		// 		//Inativo
		// 		case -1:
		// 			break;
		// 	}
		//
		// 	switch (p->flagCampo3) {
		// 		//Vazio
		// 		case 0:
		// 			break;
		// 		//Temporario
		// 		case 1:
		// 			break;
		//
		// 		//Constante
		// 		case 2:
		// 			break;
		//
		// 		//Hash
		// 		case 3:
		// 			hashIndex = p->campo3;
		// 			b = hashTable[hashIndex];
		// 			if(b != NULL)
		// 				printf("tem gente\n" );
		// 			printf("\n--%d--\n", b->memloc);
		// 			break;
		// 		//Char
		// 		case 4:
		// 			break;
		// 		//Label
		// 		case 5:
		// 			break;
		// 		//Quantidade de parametros
		// 		case 6:
		// 			break;
		// 		//Inativo
		// 		case -1:
		// 			break;
		// 	}
		//
		// 	switch (p->flagCampo4) {
		// 		//Vazio
		// 		case 0:
		// 			break;
		// 		//Temporario
		// 		case 1:
		// 			break;
		//
		// 		//Constante
		// 		case 2:
		// 			break;
		//
		// 		//Hash
		// 		case 3:
		// 			hashIndex = p->campo4;
		// 			b = hashTable[hashIndex];
		// 			if(b != NULL)
		// 				printf("tem gente\n" );
		// 			printf("\na--%d--\n", b->memloc);
		// 			break;
		// 		//Char
		// 		case 4:
		// 			break;
		// 		//Label
		// 		case 5:
		// 			break;
		// 		//Quantidade de parametros
		// 		case 6:
		// 			break;
		// 		//Inativo
		// 		case -1:
		// 			break;
		// 	}
		// }

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
					hashIndex = p->campo3;
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
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
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
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg2 != -1){
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

			elemento2->campo1 = "MULT";
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
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
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
					elemento2->flagCampo3 = 0;
					elemento2->campo3 = 0;
					reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg2 != -1){
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

			elemento2->campo1 = "DIV";
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

		/*
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
		*/

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
					//Guarda o valor constante 1 ou 0
					if(flagDiferente == 1){
						elemento2->campo4 = IM16_01; //Constante 1
					}
					if(flagDiferente == 0){
						elemento2->campo4 = 0;
					}
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
			elemento2->campo4 = p->campo3;
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
					elemento2->campo1 = "LOADI";
					reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					if(reg1 != -1){
						elemento2->campo2 = reg1;
						elemento2->flagCampo2 = 7;
						elemento2->campo3 = 0;
						elemento2->flagCampo3 = 0;
						elemento2->campo4 = p->campo2;
						elemento2->flagCampo4 = 5;
						insereFinal(codigo_de_maquina, *elemento2);
						posicaoMemoriaDeInstrucoes++;
						quadraReset(elemento2);
					}

					elemento2->campo1 = "JUMPR";
					elemento2->campo2 = reg1;
					elemento2->flagCampo2 = 7;
					elemento2->campo3 = reg1;
					elemento2->flagCampo3 = 7;
					elemento2->campo4 = 0;
					elemento2->flagCampo4 = 0;
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;

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
				case 3:
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					if(b == NULL){
						return;
					}
					elemento2->campo1 = "LOAD";
					elemento2->campo4 = b->memloc;
					// elemento2->campo4 = 5000;
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
					// imprimeQuadra(elemento2);
					quadraReset(elemento2);
					break;
			}

			switch (p->flagCampo4) {
				case 8:

					break;
			}

			elemento2->campo1 = "STORE";
			elemento2->campo2 = reg1;
			elemento2->campo3 = 0;
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 0;
			elemento2->campo4 = p->campo4;
			elemento2->flagCampo4 = 8;
			insereFinal(codigo_de_maquina, *elemento2);
			posicaoMemoriaDeInstrucoes++;
			setRegistradorLivre(bancoDeRegistradores, reg1);
			// imprimeQuadra(elemento2);
			quadraReset(elemento2);
		}

		else if(!strcmp(p->campo1, "call")){

			chamadaDeFuncao = 1;

			//Carrega o valor a ser gravado na memoria em um registrador
			// elemento2->campo1 = "LOADI end de ret";
			elemento2->campo1 = "LOADI";
			reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
			if(reg2 != -1){
				elemento2->campo2 = reg2;
				elemento2->campo3 = 0;

				hashIndex = p->campo2;
				b = hashTable[hashIndex];

				elemento2->campo4 = posicaoMemoriaDeInstrucoes + 4;
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 0;
				elemento2->flagCampo4 = 9;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				quadraReset(elemento2);
			}

			//Carrega a posicao de volta na posicao do memloc da funcao chamada
			// elemento2->campo1 = "STORE end de ret";
			elemento2->campo1 = "STORE";
			elemento2->campo2 = reg2;
			elemento2->campo3 = 0;
			// elemento2->campo4 = posicaoMemoriaDeInstrucoes;
			elemento2->campo4 = hashTable[p->campo2]->memloc;
			elemento2->flagCampo2 = 7;
			elemento2->flagCampo3 = 0;
			elemento2->flagCampo4 = 8;
			insereFinal(codigo_de_maquina, *elemento2);
			quadraReset(elemento2);
			setRegistradorLivre(bancoDeRegistradores, reg2);
			posicaoMemoriaDeInstrucoes++;

			//Carrega a posicao de jump em um registrador
			// elemento2->campo1 = "LOADI end de j";
			elemento2->campo1 = "LOADI";
			reg2 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
			if(reg2 != -1){
				elemento2->campo2 = reg2;
				elemento2->campo3 = 0;

				hashIndex = p->campo2;
				b = hashTable[hashIndex];

				// elemento2->campo4 = posicaoDeFuncoes[p->campo2] - 1;
				elemento2->campo4 = posicaoDeFuncoes[p->campo2];
				elemento2->flagCampo2 = 7;
				elemento2->flagCampo3 = 0;
				elemento2->flagCampo4 = 9;
				insereFinal(codigo_de_maquina, *elemento2);
				posicaoMemoriaDeInstrucoes++;
				quadraReset(elemento2);
			}

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

					registradoresTemporarios[p->campo4] = reg1;
					break;
			}
		}

		else if(!strcmp(p->campo1, "ret")){
			funcaoComRetorno = 1;
			switch (p->flagCampo2) {
				case 3:

					// //Guarda o retorno
					// elemento2->campo1 = "STORE";
					// reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
					// if(reg1 != -1){
					// 	elemento2->campo2 = reg1;
					// 	elemento2->campo3 = 0;
					// 	elemento2->campo4 = hashTable[p->campo2]->memloc;
					// 	elemento2->flagCampo2 = 7;
					// 	elemento2->flagCampo3 = 0;
					// 	elemento2->flagCampo4 = 8;
					// 	insereFinal(codigo_de_maquina, *elemento2);
					// 	posicaoMemoriaDeInstrucoes++;
					// 	// imprimeQuadra(elemento2);
					// }

					//Carrega o retorno numa variavel fixa
					elemento2->campo1 = "LOAD";
					hashIndex = p->campo2;
					b = hashTable[hashIndex];
					// elemento2->campo4 = b->memloc;
					// elemento2->campo4 = memlocFuncaoAnterior;
					elemento2->campo4 = hashTable[p->campo2]->memloc;
					elemento2->flagCampo4 = 8;
					elemento2->campo3 = 0;
					elemento2->flagCampo3 = 0;
					// reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);

					elemento2->flagCampo2 = 7;
					// elemento2->campo2 = fimDoBancoDeRegistradores - 1;
					elemento2->campo2 = fimDoBancoDeRegistradores;
					setRegistradorUsado(bancoDeRegistradores, fimDoBancoDeRegistradores);
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;
					// imprimeQuadra(elemento2);

					//Pula de volta para a posicao de chamada
					elemento2->campo1 = "LOAD";
					elemento2->campo2 = reg1;
					elemento2->campo3 = 0;
					// elemento2->campo4 = memlocFuncaoAnterior;
					elemento2->campo4 = memlocFuncaoAtual;
					elemento2->flagCampo2 = 7;
					elemento2->flagCampo3 = 0;
					elemento2->flagCampo4 = 8;
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;

					elemento2->campo1 = "JUMPR";
					elemento2->campo2 = reg1;
					elemento2->campo3 = reg1;
					elemento2->campo4 = 0;
					elemento2->flagCampo2 = 7;
					elemento2->flagCampo3 = 7;
					elemento2->flagCampo4 = 0;
					insereFinal(codigo_de_maquina, *elemento2);
					posicaoMemoriaDeInstrucoes++;
					break;
			}
		}
		p = p->prox;
	}
	// imprimeLista(codigo_de_maquina);
	imprimeListaFormatada(codigo_de_maquina);

	p = codigo_de_maquina->inicio;
	quadraReset(elemento2);

	elemento2->campo1 = "NOP";
	elemento2->campo2 = 0;
	elemento2->campo3 = 0;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 0;
	elemento2->flagCampo3 = 0;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);

	elemento2->campo1 = "IN";
	elemento2->campo2 = fimDoBancoDeRegistradores - 2;
	elemento2->campo3 = 0;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 7;
	elemento2->flagCampo3 = 0;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);

	elemento2->campo1 = "LOAD";
	reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
	if(reg1 != -1){
		elemento2->campo2 = reg1;
		elemento2->flagCampo2 = 7;
		elemento2->campo3 = 0;
		elemento2->flagCampo3 = 0;
		elemento2->campo4 = 0; //memloc do input
		elemento2->flagCampo4 = 9;
		insereFinal(codigo_de_maquina2, *elemento2);
	}

	elemento2->campo1 = "JUMPR";
	elemento2->campo2 = reg1;
	elemento2->campo3 = reg1;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 7;
	elemento2->flagCampo3 = 7;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);

	//Insere funcao de output na memoria de instrucoes
	elemento2->campo1 = "OUT";
	elemento2->campo2 = fimDoBancoDeRegistradores - 2;
	elemento2->campo3 = 0;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 7;
	elemento2->flagCampo3 = 0;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);

	elemento2->campo1 = "LOAD";
	reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
	if(reg1 != -1){
		elemento2->campo2 = reg1;
		elemento2->flagCampo2 = 7;
		elemento2->campo3 = 0;
		elemento2->flagCampo3 = 0;
		elemento2->campo4 = 1; //memloc do output
		elemento2->flagCampo4 = 9;
		insereFinal(codigo_de_maquina2, *elemento2);
	}

	elemento2->campo1 = "JUMPR";
	elemento2->campo2 = reg1;
	elemento2->campo3 = reg1;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 7;
	elemento2->flagCampo3 = 7;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);

	//Insere o JUMP para a main
	elemento2->campo1 = "LOADI";
	reg1 = buscaRegistradorVazio(bancoDeRegistradores, BR_SIZE);
	if(reg1 != -1){
		elemento2->campo2 = reg1;
		elemento2->flagCampo2 = 7;
		elemento2->campo3 = 0;
		elemento2->flagCampo3 = 0;
		elemento2->campo4 = posicaoDeFuncoes[94];
		elemento2->flagCampo4 = 9;
		insereFinal(codigo_de_maquina2, *elemento2);
	}

	elemento2->campo1 = "JUMPR";
	elemento2->campo2 = reg1;
	elemento2->campo3 = reg1;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 7;
	elemento2->flagCampo3 = 7;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);


	while(p != NULL){

		//Exclui a instrucao LAB e atualiza a posicao das labels
		if(strcmp(p->campo1, "LAB")){
			elemento2->campo1 = p->campo1;
			elemento2->campo2 = p->campo2;
			elemento2->campo3 = p->campo3;
			if(p->flagCampo4 == 5){
				elemento2->campo4 = enderecoLabel[p->campo4] + 2;
			}
			else{
				elemento2->campo4 = p->campo4;
			}

			elemento2->flagCampo2 = p->flagCampo2;
			elemento2->flagCampo3 = p->flagCampo3;
			elemento2->flagCampo4 = p->flagCampo4;

			insereFinal(codigo_de_maquina2, *elemento2);
		}
		p = p->prox;
	}

	elemento2->campo1 = "HLT";
	elemento2->campo2 = 0;
	elemento2->campo3 = 0;
	elemento2->campo4 = 0;
	elemento2->flagCampo2 = 0;
	elemento2->flagCampo3 = 0;
	elemento2->flagCampo4 = 0;
	insereFinal(codigo_de_maquina2, *elemento2);

	printf("\n\nCÓDIGO OBJETO 2\n");
	imprimeListaFormatada(codigo_de_maquina2);

	printf("\n\nCODIGO BINARIO\n");
	binaryConverter(codigo_de_maquina2);
}
