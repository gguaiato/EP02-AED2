#include <stdio.h>
#include <stdlib.h>

typedef struct {
	FILE *file;
	int *vetor;
	int atualLeitura;
	int maxLeitura;
} Valores;

int qtdArquivos;
int tamanhoLista;
int comprimentoSeries;

int funcaoOrdenacao(const void * x, const void * y)
{
   return (*(int*) x - *(int*) y);
}

//Salva valores temporarios.
void gravaArq(char *arquivoTemp, int *vetor, int tamanhoVetor, int mudaLinhaFinal){
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

//Carrega o arquivo inicial e processa os proximos arquivos ordenados.
int geraArqs(const char *arquivoEntrada){

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

	//Armazena os dados das variaveis qtdArquivos, comprimentoSeries, tamanhoLista.
	while(flag < 3){
		if (flag == 0){
			fscanf(f, "%i", &qtdArquivos);
		}
		if (flag == 1){
			fscanf(f, "%i", &comprimentoSeries);
		}
		if(flag == 2){
			fscanf(f, "%i", &tamanhoLista);
		}
		flag++;
	}

	int vetorParaOrdenar[ tamanhoLista ];

	while(qtdArquivosGerados != qtdArquivos){
		fscanf(f, "%d", &vetorParaOrdenar[qtdLida]);
		qtdLida++;

		//Caso tenha lido a qtd máxima de dados permitido por vetor.
		if(qtdLida == tamanhoLista){
			qtdArquivosGerados++;
			sprintf(vetorTemp,"Temporario%d.txt", qtdArquivosGerados);
			qsort(vetorParaOrdenar, qtdLida, sizeof(int), funcaoOrdenacao);
			gravaArq(vetorTemp, vetorParaOrdenar, qtdLida, 0);
			qtdLida = 0;
		}
	}
	//Caso total de elementos no arquivo não seja multiplo de n (Tamanho máximo permitido no vetor).
	if(qtdLida > 0){
		qtdArquivosGerados++;
		sprintf(vetorTemp,"Temporario%d.txt", qtdArquivosGerados);
		qsort(vetorParaOrdenar, qtdLida, sizeof(int), funcaoOrdenacao);
		gravaArq(vetorTemp, vetorParaOrdenar, qtdLida, 0);
	}
	fclose(f);
	return 1;
}

void populaValores(Valores *valores, int comprimentoSeries){
	int i;
	if(valores->file == NULL){
		return;
	}

	valores->atualLeitura = 0;
	valores->maxLeitura = 0;
	for(i = 0; i < comprimentoSeries; i++){
		//Le os dados do arquivo e insere no vetor.
		if(!feof(valores->file)){
			fscanf(valores->file, "%d", &valores->vetor[valores->maxLeitura]);
			valores->maxLeitura++;
		} else {
			//Fecha o arquivo caso tenha acabado os dados.
			fclose(valores->file);
			valores->file = NULL;
			break;
		}
	}
}

//Pesquisa o menor valor dentre os arquivos temporarios
int recuperaMinValor(Valores *arq, int qtdArquivos, int comprimentoSeries, int* menor){
	int i;

	// -1 = Valor nao encontrado, +1 = Valor encontrado
	int achouMenor = -1;

	//Compara as primeiras posicoes de cada arquivo temporario.
	for(i = 0; i < qtdArquivos; i++){
		if(arq[i].atualLeitura < arq[i].maxLeitura){
			if(achouMenor == -1){
				achouMenor = i;
			} else {
				if(arq[i].vetor[arq[i].atualLeitura] < arq[achouMenor].vetor[arq[achouMenor].atualLeitura]){
					achouMenor = i;
				}
			}
		}
	}

	//Caso tenha achado o menor elemento, atualiza a posicao.
	if(achouMenor != -1){
		*menor = arq[achouMenor].vetor[arq[achouMenor].atualLeitura];
		arq[achouMenor].atualLeitura++;
		if(arq[achouMenor].atualLeitura == arq[achouMenor].maxLeitura){
			populaValores(&arq[achouMenor], comprimentoSeries);
		}
		return 1;
	} else {
		return 0;
	}
}

//Ordena os dados pelo metodo mergeSort nos arquivos temporarios
void ordenaMergesort(char *arquivoTemp, int qtdArquivos, int comprimentoSeries){
	char vetorTemp[20];
	int i, menor;
	int *vetor = (int*)malloc(comprimentoSeries * sizeof(int));

	Valores *valores = (Valores*)malloc( qtdArquivos * sizeof(Valores) );

	for(i = 0; i < qtdArquivos; i++){
		sprintf(vetorTemp,"Temporario%d.txt", i+1);
		valores[i].file = fopen(vetorTemp, "r");
		valores[i].maxLeitura = 0;
		valores[i].atualLeitura = 0;
		valores[i].vetor = (int*)malloc( comprimentoSeries * sizeof(int));
		populaValores(&valores[i], comprimentoSeries);
	}

	int qtdVetor = 0;
	while(recuperaMinValor(valores, qtdArquivos, comprimentoSeries, &menor) == 1){
		vetor[qtdVetor] = menor;
		qtdVetor++;
		if(qtdVetor == comprimentoSeries){
			gravaArq(arquivoTemp, vetor, comprimentoSeries, 1);
			qtdVetor = 0;
		}
	}

	if(qtdVetor != 0){
		gravaArq(arquivoTemp, vetor, qtdVetor, 1);
	}

	for(i = 0; i < qtdArquivos; i++){
		free(valores[i].vetor);
	}
	free(valores);
	free(vetor);
}

void mergesortImpl(const char *entrada, char *saida){
	char temporario[20];

	geraArqs(entrada);
	int i;

	remove(entrada);

	ordenaMergesort(saida, qtdArquivos, comprimentoSeries);

	for(i = 0; i < qtdArquivos; i++){
		sprintf(temporario, "Temporario%d.txt", i+1);
		remove(temporario);
	}
}

int main(int argc, char *argv[]){
	printf("Iniciando processamento\n");

	mergesortImpl(argv[1], argv[2]);

	printf("Fim de processamento\n");
}
