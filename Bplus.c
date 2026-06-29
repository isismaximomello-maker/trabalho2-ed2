#include "Bplus.h"
#include <stdio.h>
#include <stdlib.h>

// funcões para a página
Pagina *criaPagina();
void inicializarPagina(Pagina *pagina, int indice, int tipo);
void destroiPagina(Pagina *p);
void inserirElemento(Pagina *p, const void* chave);

void removerElemento(Pagina *p, const void *chave, int (*comparar)(const void*, const void*)){
    int pos = -1;

    // procura a chave na página
    for(int i = 0; i < p->qtElementos; i++){
        if(comparar(chave, p->chave[i]) == 0){
            pos = i;
            break;
        }
    }

    // chave não encontrada, retorna
    if(pos == -1){
        return;
    }

    //Se encontrada, desloca elementos para esquerda
    for(int i = pos; i < p->qtElementos - 1; i++){
        memcpy(p->chave[i],
               p->chave[i+1],
               sizeof(p->chave[i]));

        p->filho[i] = p->filho[i+1];
    }

    // limpa a última posição do vetor
    memset(&p->chave[p->qtElementos-1],
           0,
           sizeof(p->chave[p->qtElementos-1]));

    p->qtElementos--;
}

void verificarOverflow(Pagina *p);

void verificarUnderflow(FILE *arquivo, Pagina *pagina){
    Cabecalho header;

    // lê cabeçalho
    fseek(arquivo, 0, SEEK_SET);
    fread(&header, sizeof(Cabecalho), 1, arquivo);

    int minimo = ORDEM / 2;

    // não existe underflow
    if (pagina->qtElementos >= minimo)
        return;

    // caso a página seja raiz
    if (pagina->indice == header.raiz){
        // se a raiz ficou vazia, árvore fica vazia
        if (pagina->qtElementos == 0){
            header.raiz = -1;

            fseek(arquivo, 0, SEEK_SET);
            fwrite(&header, sizeof(Cabecalho), 1, arquivo);
        }
        return;
    }

    // carrega pai
    Pagina pai;

    fseek(arquivo, sizeof(Cabecalho) + pagina->pai * sizeof(Pagina), SEEK_SET);
    fread(&pai, sizeof(Pagina), 1, arquivo);

    // descobre posição da página no pai
    int pos = 0;

    while (pai.filho[pos] != pagina->indice) pos++;

    Pagina irmaAdjacente;

    // tenta pegar da irmã esquerda
    if (pos > 0){
        fseek(arquivo, sizeof(Cabecalho) + pai.filho[pos-1] * sizeof(Pagina), SEEK_SET);
        fread(&irmaAdjacente, sizeof(Pagina), 1, arquivo);

        if (irmaAdjacente.qtElementos > minimo){

            // move elementos da página para abrir espaço
            for (int i = pagina->qtElementos; i > 0; i--){
                memcpy(pagina->chave[i],
                       pagina->chave[i-1],
                       sizeof(pagina->chave[i]));

                pagina->filho[i] = pagina->filho[i-1];
            }
            // pega maior elemento do irmão esquerdo

            memcpy(pagina->chave[0],
                   irmaAdjacente.chave[irmaAdjacente.qtElementos-1],
                   sizeof(pagina->chave[0]));

            pagina->filho[0] = irmaAdjacente.filho[irmaAdjacente.qtElementos-1];
            pagina->qtElementos++;
            irmaAdjacente.qtElementos--;

            // salva a irmã

            fseek(arquivo, sizeof(Cabecalho) + irmaAdjacente.indice*sizeof(Pagina), SEEK_SET);
            fwrite(&irmaAdjacente,sizeof(Pagina),1,arquivo);

            return;
        }
    }

    //tenta pegar da irmã direita
    if (pos < pai.qtElementos){
        fseek(arquivo, sizeof(Cabecalho) + pai.filho[pos+1]*sizeof(Pagina), SEEK_SET);
        fread(&irmaAdjacente,sizeof(Pagina),1,arquivo);

        if (irmaAdjacente.qtElementos > minimo){

            memcpy(pagina->chave[pagina->qtElementos],
                   irmaAdjacente.chave[0],
                   sizeof(pagina->chave[0]));

            pagina->filho[pagina->qtElementos] = irmaAdjacente.filho[0];
            pagina->qtElementos++;

            // remove primeiro elemento da irmã
            for(int i=0;i<irmaAdjacente.qtElementos-1;i++){

                memcpy(irmaAdjacente.chave[i],
                       irmaAdjacente.chave[i+1],
                       sizeof(irmaAdjacente.chave[i]));

                irmaAdjacente.filho[i] = irmaAdjacente.filho[i+1];
            }

            irmaAdjacente.qtElementos--;

            fseek(arquivo, sizeof(Cabecalho) + irmaAdjacente.indice*sizeof(Pagina), SEEK_SET);
            fwrite(&irmaAdjacente,sizeof(Pagina),1,arquivo);

            return;
        }
    }

    //se chegou aqui, não conseguiu redistribuir
    //faz concatenação !!!

    if (pos > 0){

        fseek(arquivo, sizeof(Cabecalho) + pai.filho[pos-1]*sizeof(Pagina), SEEK_SET);
        fread(&irmaAdjacente,sizeof(Pagina),1,arquivo);

        for(int i=0;i<pagina->qtElementos;i++){

            memcpy(irmaAdjacente.chave[irmaAdjacente.qtElementos],
                   pagina->chave[i],
                   sizeof(pagina->chave[i]));

            irmaAdjacente.filho[irmaAdjacente.qtElementos] = pagina->filho[i];
            irmaAdjacente.qtElementos++;
        }

        // marca página como removida
        pagina->foiDeletada = 1;

        fseek(arquivo, sizeof(Cabecalho) + irmaAdjacente.indice*sizeof(Pagina), SEEK_SET);
        fwrite(&irmaAdjacente,sizeof(Pagina),1,arquivo);

        // remove referência no pai
        for(int i=pos;i<pai.qtElementos;i++){
            pai.filho[i] = pai.filho[i+1];
        }
        pai.qtElementos--;

        fseek(arquivo, sizeof(Cabecalho) + pai.indice*sizeof(Pagina), SEEK_SET);
        fwrite(&pai,sizeof(Pagina),1,arquivo);
    }
}

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

Pagina buscarFolha(FILE *arquivo, Cabecalho *header, const void *chave, int (*comparar)(const void *, const void *)){
    
    Pagina pagina;

    //Carrega a raiz
    fseek(arquivo, sizeof(Cabecalho) + header->raiz * sizeof(Pagina), SEEK_SET);
    fread(&pagina, sizeof(Pagina), 1, arquivo);

    // Enquanto não chegar em uma folha
    while (pagina.ehfolha == 0){
        int i = 0;

        // Descobre qual filho seguir
        while (i < pagina.qtElementos && comparar(chave, pagina.chave[i]) > 0) i++;
        
        // Carrega o filho escolhido
        fseek(arquivo, sizeof(Cabecalho) + pagina.filho[i] * sizeof(Pagina), SEEK_SET);
        fread(&pagina, sizeof(Pagina), 1, arquivo);
    }   
    return pagina;
}

int buscarChaveNaArvore(const void* chave, int *enderecoRegistro, int (*comparar)(const void*, const void*)){

    FILE *arquivo = fopen(arquivoArvore, "rb");

    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    Cabecalho header;

    if (fread(&header, sizeof(Cabecalho), 1, arquivo) != 1){
        printf("Erro ao ler o cabeçalho!!\n");
        fclose(arquivo);
        return -1;
    }


    if (header.raiz == -1){
        printf("Árvore vazia!!\n");
        fclose(arquivo);
        return -1;
    }

    // busca a folha
    Pagina p = buscarFolha(arquivo, &header, chave, comparar);

    // procura a chave na folha
    for (int i = 0; i < p.qtElementos; i++){

        if (comparar(chave, p.chave[i]) == 0){

            *enderecoRegistro = p.filho[i];

            fclose(arquivo);
            return 1;
        }
    }


    fclose(arquivo);

    return -1;
}

void inserirChaveNaArvore(const void *chave, int enderecoRegistro, size_t tamanhoChave, int (*comparar)(const void *, const void *)){

    FILE *arquivo = fopen(arquivoArvore, "rb+");

    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    Cabecalho header;
    fread(&header, sizeof(Cabecalho), 1, arquivo);

    // Árvore vazia
    if (header.raiz == -1){

        Pagina *raiz = criaPagina();
        inicializarPagina(raiz, 0, 1);
        inserirElemento(raiz, chave);
        raiz->filho[0] = enderecoRegistro;
        header.raiz = 0;
        header.qtdPaginas = 1;

        fseek(arquivo, 0, SEEK_SET);
        fwrite(&header, sizeof(Cabecalho), 1, arquivo);

        fseek(arquivo, sizeof(Cabecalho), SEEK_SET);
        fwrite(raiz, sizeof(Pagina), 1, arquivo);

        destroiPagina(raiz);

        fclose(arquivo);
        return;
    }

    //encontra a folha
    Pagina pagina = buscarFolha(arquivo, &header, chave, comparar);

    //insere na folha
    inserirElemento(&pagina, chave);
    pagina.filho[pagina.qtElementos - 1] = enderecoRegistro;
    ordenarPaginaFolha(&pagina);
    verificarOverflow(&pagina);

    //salva a página
    fseek(arquivo, sizeof(Cabecalho) + pagina.indice * sizeof(Pagina), SEEK_SET);
    fwrite(&pagina, sizeof(Pagina), 1, arquivo);
    fclose(arquivo);
}

void deletarChaveNaArvore(const void *chave, int (*comparar)(const void *, const void *)){

    FILE *arquivo = fopen(arquivoArvore, "rb+");

    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo!!!\n");
        return;
    }

    Cabecalho header;

    if (fread(&header, sizeof(Cabecalho), 1, arquivo) != 1){
        fclose(arquivo);
        return;
    }

    if (header.raiz == -1){
        printf("Árvore vazia!\n");
        fclose(arquivo);
        return;
    }

    // encontra a folha
    Pagina pagina = buscarFolha(arquivo, &header, chave, comparar);

    int encontrou = 0;

    for (int i = 0; i < pagina.qtElementos; i++){
        if (comparar(chave, pagina.chave[i]) == 0){
            encontrou = 1;
            break;
        }
    }

    if (!encontrou){
        printf("Chave não encontrada.\n");
        fclose(arquivo);
        return;
    }

    removerElemento(&pagina, chave);
    verificarUnderflow(arquivo, &pagina);

    fseek(arquivo, sizeof(Cabecalho) + pagina.indice * sizeof(Pagina), SEEK_SET);

    fwrite(&pagina, sizeof(Pagina), 1, arquivo);
    fclose(arquivo);
}



