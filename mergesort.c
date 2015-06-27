#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct auxChave {
    int valor;
    struct auxChave *proxChave;
} Chave;

typedef struct {
    int qtdArqs;
    int comprimentoSeries;
    int tamanhoLista;
    Chave *primeiraChave;
} MergeFile;

MergeFile* leArq(char *fileName) {
    FILE *file = fopen( fileName, "r" );
    if ( file == NULL ) {
            printf( "Nao foi possivel ler o arquivo %s\n", fileName);
    } else {
        MergeFile *mergeFile = (MergeFile*) malloc(sizeof(MergeFile));
        int qtdArqs, comprimentoSeries, tamanhoLista;
        fscanf(file, "%d %d %d", &qtdArqs, &comprimentoSeries, &tamanhoLista);
        mergeFile -> qtdArqs = qtdArqs;
        mergeFile -> comprimentoSeries =  comprimentoSeries;
        mergeFile -> tamanhoLista = tamanhoLista;
        mergeFile -> primeiraChave = NULL;

        Chave *chaveAtual = NULL;
        for(int i = 0; i < tamanhoLista; i++)
        {
            int valor;
            fscanf(file, "%d", &valor);
            Chave *chave = (Chave*)malloc(sizeof(Chave));
            chave -> valor = valor;
            chave -> proxChave = NULL;
            if (mergeFile -> primeiraChave == NULL) {
                mergeFile -> primeiraChave = chave;
            } 
            if (chaveAtual != NULL) {
                chaveAtual -> proxChave = chave;
            }
            chaveAtual = chave;
        }

        fclose( file );
        return mergeFile;
    }
    return NULL;
}

void salvaArqMergeFile(const char * filename, MergeFile *mergeFile) {
    FILE *out = fopen(filename,"w");
    if(out==NULL)
    {
        printf("Nao foi possivel escrever para '%s'\n", filename);
        return;
    }
    Chave *chaveAtual = mergeFile -> primeiraChave;
    for(int i = 0; i < mergeFile -> tamanhoLista; i++)
    {
        fprintf(out,"%d ",chaveAtual -> valor);
        chaveAtual = chaveAtual -> proxChave;
    }
    fclose(out);
}

int main(int argc, char *argv[]) {
    if ( argc != 3 ) {
        printf("Voce deve fornecer um arquivo de entrada e um de saida\n");
        return 0;
    }
    MergeFile *mergeFile = leArq(argv[1]);
    // processa mergeSorte
    salvaArqMergeFile(argv[2], mergeFile);
    printf("Fim de execucao\n");
    return 0;
}



