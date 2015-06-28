/*
 ============================================================================
 Nome       : d_8921878_9050204.c
 Alunos     : Caio de Lima Granero & Lucas Sant'Ana
 Nº USP     : 8921878 / 9050204
 Professor  : Dr. Marcelo de Souza Lauretto
 Descricao  : Desenvolver um programa tal que, dados:
				- Um arquivo de entrada contendo uma lista de inteiros, f,
				- o numero de arquivos para o multiway merge sort, m,
				- o tamanho inicial das series, k,
			  realize a ordenacao multiway de f utilizando m arquivos de leitura e m arquivos de escrita.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int m, n, k;

struct arquivo{
	FILE *f;
	 int posicaoVetor, tamanhoMaxVetor, *vetor;
};

//Função principal
void mergeSortExterno(const char *arquivoEntrada, char *arquivoSaida){
	char apagaTemp[20];

	criaArquivosOrdenados(arquivoEntrada);
	int i;

	//Apaga arquivo original.
	remove(arquivoEntrada);

	//Merge Sort(Ordena os m arquivos e insere no arquivo original).
	mergeSort(arquivoSaida, m, k);

	//Apaga arquivos temporarios.
	for(i = 0; i < m; i++){
		sprintf(apagaTemp, "Temp%d.txt", i+1);
		remove(apagaTemp);
	}
}

//Abre o arquivo principal e gera os m arquivos ordenados.
int criaArquivosOrdenados(const char *arquivoEntrada){

	int qtdLida = 0;
	int qtdArquivosGerados = 0;

	char vetorTemp[20];
	FILE *f = fopen(arquivoEntrada, "r");

	if (f == NULL){
		printf("ERRO: Arquivo nao encontrado");
		system("pause");
		return 0;
	}

	int flag = 0;

	//Armazena os dados das variaveis m, k, n.
	while(flag < 3){
		if (flag == 0){
			fscanf(f, "%i", &m);
		}
		if (flag == 1){
			fscanf(f, "%i", &k);
		}
		if(flag == 2){
			fscanf(f, "%i", &n);
		}
		flag++;
	}

	int vetorParaOrdenar[n];

	while(qtdArquivosGerados != m){
		fscanf(f, "%d", &vetorParaOrdenar[qtdLida]);
		qtdLida++;

		//Caso tenha lido a qtd máxima de dados permitido por vetor.
		if(qtdLida == n){
			qtdArquivosGerados++;
			sprintf(vetorTemp,"Temp%d.txt", qtdArquivosGerados);
			qsort(vetorParaOrdenar, qtdLida, sizeof(int), fSort);
			salvaArquivo(vetorTemp, vetorParaOrdenar, qtdLida, 0);
			qtdLida = 0;
		}
	}
	//Caso total de elementos no arquivo não seja multiplo de n (Tamanho máximo permitido no vetor).
	if(qtdLida > 0){
		qtdArquivosGerados++;
		sprintf(vetorTemp,"Temp%d.txt", qtdArquivosGerados);
		qsort(vetorParaOrdenar, qtdLida, sizeof(int), fSort);
		salvaArquivo(vetorTemp, vetorParaOrdenar, qtdLida, 0);
	}
	fclose(f);
}

//Salva os dados nos arquivos temporarios.
void salvaArquivo(char *arquivoTemp, int *vetor, int tamanhoVetor, int mudaLinhaFinal){
	int i;
	FILE *f = fopen(arquivoTemp, "a");

	for(i = 0; i < tamanhoVetor - 1; i++){
		fprintf(f, "%d\n", vetor[i]);
	}
	if(mudaLinhaFinal == 0){
		fprintf(f, "%d", vetor[tamanhoVetor-1]);
	} else {
		fprintf(f, "%d\n", vetor[tamanhoVetor-1]);
	}
	fclose(f);
}

//Ordena os dados pelo metodo mergeSort nos arquivos temporarios
void mergeSort(char *arquivoTemp, int m, int k){
	char vetorTemp[20];
	int i, menor;
	int *vetor = (int*)malloc(k*sizeof(int));

	struct arquivo* arq;
	arq = (struct arquivo*)malloc(m*sizeof(struct arquivo));

	for(i = 0; i < m; i++){
		sprintf(vetorTemp,"Temp%d.txt", i+1);
		arq[i].f = fopen(vetorTemp, "r");
		arq[i].tamanhoMaxVetor = 0;
		arq[i].posicaoVetor = 0;
		arq[i].vetor = (int*)malloc(k*sizeof(int));
		preencheVetor(&arq[i], k);
	}

	int qtdVetor = 0;
	while(procuraMenorElemento(arq, m, k, &menor) == 1){
		vetor[qtdVetor] = menor;
		qtdVetor++;
		if(qtdVetor == k){
			salvaArquivo(arquivoTemp, vetor, k, 1);
			qtdVetor = 0;
		}
	}

	if(qtdVetor != 0){
		salvaArquivo(arquivoTemp, vetor, qtdVetor, 1);
	}

	for(i = 0; i < m; i++){
		free(arq[i].vetor);
	}
	free(arq);
	free(vetor);
}

//Procura entre todos os arquivos temporarios o menor elemento.
int procuraMenorElemento(struct arquivo* arq, int m, int k, int* menor){
	int i;
	int achouMenor = -1; // -1 = Nao achou
						 // +1 = Achou

	//Compara as primeiras posicoes de cada arquivo temporario.
	for(i = 0; i < m; i++){
		if(arq[i].posicaoVetor < arq[i].tamanhoMaxVetor){
			if(achouMenor == -1){
				achouMenor = i;
			} else {
				if(arq[i].vetor[arq[i].posicaoVetor] < arq[achouMenor].vetor[arq[achouMenor].posicaoVetor]){
					achouMenor = i;
				}
			}
		}
	}

	//Caso tenha achado o menor elemento, atualiza a posicao.
	if(achouMenor != -1){
		*menor = arq[achouMenor].vetor[arq[achouMenor].posicaoVetor];
		arq[achouMenor].posicaoVetor++;
		if(arq[achouMenor].posicaoVetor == arq[achouMenor].tamanhoMaxVetor){
			preencheVetor(&arq[achouMenor], k);
		}
		return 1;
	} else {
		return 0;
	}
}

//Prenche a struct arquivo.
void preencheVetor(struct arquivo* arq, int k){
	int i;
	if(arq->f == NULL){
		return;
	}

	arq->posicaoVetor = 0;
	arq->tamanhoMaxVetor = 0;
	for(i = 0; i < k; i++){
		//Le os dados do arquivo e insere no vetor.
		if(!feof(arq->f)){
			fscanf(arq->f, "%d", &arq->vetor[arq->tamanhoMaxVetor]);
			arq->tamanhoMaxVetor++;
		} else {
			//Fecha o arquivo caso tenha acabado os dados.
			fclose(arq->f);
			arq->f = NULL;
			break;
		}
	}
}

//Função de ordenação pro metodo qsort.
int fSort(const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[]){
	mergeSortExterno(argv[1], argv[2]);
}
