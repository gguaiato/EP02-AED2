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

void gravaArq(char *arquivoTemporario, int *vetor, int tamanhoVetor, int alteraUltimaLinha){
	int i;
	FILE *f = fopen(arquivoTemporario, "a");

	for(i = 0; i < tamanhoVetor - 1; i++){
		fprintf(f, "%d\n", vetor[i]);
	}
	if(alteraUltimaLinha == 0){
		fprintf(f, "%d", vetor[tamanhoVetor-1]);
	} else {
		fprintf(f, "%d\n", vetor[tamanhoVetor-1]);
	}
	fclose(f);
}

int geradorArquivos(const char *arqInput){

	int qtdLida = 0;
	int qtdArquivosGerados = 0;

	char vetorTemp[20];
	FILE *f = fopen(arqInput, "r");

	if (f == NULL){
		printf("Erro ao procurar o arquivo");
		system("pause");
		return 0;
	}

	int flag = 0;

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

		if(qtdLida == tamanhoLista){
			qtdArquivosGerados++;
			sprintf(vetorTemp,"Temporario%d.txt", qtdArquivosGerados);
			qsort(vetorParaOrdenar, qtdLida, sizeof(int), funcaoOrdenacao);
			gravaArq(vetorTemp, vetorParaOrdenar, qtdLida, 0);
			qtdLida = 0;
		}
	}
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
		if(!feof(valores->file)){
			fscanf(valores->file, "%d", &valores->vetor[valores->maxLeitura]);
			valores->maxLeitura++;
		} else {
			fclose(valores->file);
			valores->file = NULL;
			break;
		}
	}
}

int recuperaMinValor(Valores *arq, int qtdArquivos, int comprimentoSeries, int* menor){
	int i;

	int achouMenor = -1;

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

void ordenaMergesort(char *arquivoTemporario, int qtdArquivos, int comprimentoSeries){
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
			gravaArq(arquivoTemporario, vetor, comprimentoSeries, 1);
			qtdVetor = 0;
		}
	}

	if(qtdVetor != 0){
		gravaArq(arquivoTemporario, vetor, qtdVetor, 1);
	}

	for(i = 0; i < qtdArquivos; i++){
		free(valores[i].vetor);
	}
	free(valores);
	free(vetor);
}

void mergesortImpl(const char *entrada, char *saida){
	char temporario[20];

	geradorArquivos(entrada);
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
