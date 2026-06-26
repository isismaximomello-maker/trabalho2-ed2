#include "Bplus.h"
#include <stdio.h>
#include <stdlib.h>

// funcões para a página
Pagina *criaPagina();
void inicializarPagina(Pagina *pagina, int indice, int tipo);
void destroiPagina(Pagina *p);
void inserirElemento(Pagina *p, const void* chave);
void removerElemento(Pagina *p, const void* chave);
void verificarOverflow(Pagina *p);
void verificarUnderflow(Pagina *p);
void ordenarPaginaFolha(Pagina *p);
int buscarPaginaLivre(){
    //abre o arquivo pra leitura
    FILE *arquivo = fopen(arquivoArvore, "rb+");
    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    //pula o cabecalho
    fseek(arquivo, sizeof(Cabecalho), SEEK_SET);
    //le pagina a pagina até encontrar uma que foi deletada logicamente
    Pagina p;
    int i = 0;
    while(fread(&p, sizeof(Pagina), 1, arquivo)){
        if (p.foiDeletada)
            break;
        i++;
    }
    //fecha o arquivo
    fclose(arquivo);
    //retorna indice da página já deletada
    return i;
}

// funcões para a árvore
void inicializarArvore();
void imprimirArvore();
int buscarChave(const void* chave, int *indice, int (*comparar)(const void*, const void*)){
    //abrir arquivo
    FILE *arquivo = fopen(arquivoArvore, "rb+");
    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    Cabecalho header;
    if (fread(&header, sizeof(Cabecalho), 1, arquivo) != 1){
        if (header.raiz == -1){
        printf("Arvore vazia!\n");
        fclose(arquivo);
        return;
        }
    }
    //carregar pagina raiz
    fseek(arquivo, sizeof(Cabecalho), SEEK_SET);
    Pagina p;
    fread(&p, sizeof(Pagina), 1, arquivo);

    //loop de descida
    while(p.ehfolha == 0){
        int q = p.qtElementos;
        for (int i = 0; i < q-1; i++){
            comparar(p.chave[i], (const void*)chave);
        }
    }
    //if folha
    //comparacão exata
    //chave não encontrada
}
void inserirChave(const void* chave, int enderecoRegistro);
void deletarChave(const void* chave);
void imprimirChavesIntervalo(const void* inicial, const void* final);