#include "Bplus.h"
#include <stdio.h>
#include <stdlib.h>
static const char *arquivoArvore = "arvore.dat";

// funcões para a página
Pagina *criaPagina(){
    Pagina *pagina = (Pagina*)malloc(sizeof(Pagina));
    if (pagina == NULL){
        printf("Erro ao alocar memória para a página!\n");
        return NULL;
    }
    return pagina;
}

void inicializarPagina(Pagina *pagina, int indice, int tipo){
    pagina->indice = indice;
    pagina->pai = -1;
    pagina->proximaFolha = -1;
    pagina->qtElementos = 0;
    pagina->ehfolha = tipo;
    pagina->foiDeletada = 0;
}

void destroiPagina(Pagina *p){ 
    p->foiDeletada = 1;
    p->qtElementos = 0;
    
    FILE* fp = fopen(arquivoArvore, "r+b"); // r+b permite escrita sem apagar o arquivoArvore
    if (fp != NULL) {
        fseek(fp, sizeof(Cabecalho) + p->indice * sizeof(Pagina), SEEK_SET);
        fwrite(p, sizeof(Pagina), 1, fp);
    }
    
    // agora que está salvo no disco, removemos da memória RAM
    free(p);
    // ajusta o cabecaho da arvore
    Cabecalho header;
    fseek(arquivoArvore, 0, SEEK_SET);
    fread(&header, sizeof(Cabecalho), 1, arquivoArvore);
    header.qtdPaginas --;
    fwrite(&header, sizeof(Cabecalho), 1, fp);
    fclose(fp);

}

void inserirElemento(Pagina *p, const void* chave, int indice, int (*comparar)(const void *, const void *)){

    // Inserção na memória RAM mantendo o ponteiro genérico 
    p->chave[p->qtElementos] = (void*)chave; 
    p->filho[p->qtElementos] = indice; // Em folhas, armazena o índice do registro; em internos, o índice do filho direito
    p->qtElementos++;
    
    // Ordena a página na RAM
    ordenarPaginaFolha(p, comparar);

    // Verifica e trata o overflow (cisão) inteiramente na memória RAM
    verificarOverflow(p, comparar);

    // Ao fim, salva as alterações necessárias no disco
    FILE* arquivo = fopen(arquivoArvore, "r+b"); 
    if (arquivo != NULL) {
        // Salva a página atual (p) atualizada
        fseek(arquivo, sizeof(Cabecalho) + p->indice * sizeof(Pagina), SEEK_SET); 
        fwrite(p, sizeof(Pagina), 1, arquivo);
        fclose(arquivo);
    }
}

int removerElemento(Pagina *p, const void *chave, int (*comparar)(const void*, const void*)){
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
        p->chave[i] = p->chave[i+1]; // Atribuição direta de ponteiros genéricos
        p->filho[i] = p->filho[i+1];
    }

    // limpa a última posição do vetor
    p->chave[p->qtElementos-1] = NULL; // Definido como NULL de forma segura
    p->filho[p->qtElementos-1] = 0;    // (ou -1, dependendo do seu padrão para "sem filho")

    p->qtElementos--;
}

void verificarOverflow(Pagina *p, int (*comparar)(const void *, const void *)) {
    if (p->qtElementos <= ORDEM) 
        return; 

    // inicia a cisão da página
    FILE* arquivo = fopen(arquivoArvore, "r+b");
    if (arquivo == NULL) return;

    Cabecalho header;
    fseek(arquivo, 0, SEEK_SET);
    fread(&header, sizeof(Cabecalho), 1, arquivo);

    // cria página irmã e define seus atributos
    Pagina *novaPagina = criaPagina();
    novaPagina->indice = buscarPaginaLivre(); 
    novaPagina->ehfolha = p->ehfolha;
    novaPagina->pai = p->pai;

    int meio = p->qtElementos / 2;
    void* chaveMediana = p->chave[meio];

    if (p->ehfolha) {
        // Se folha, copia a metade superior das chaves e dos filhos para a irmã
        for (int i = meio, j = 0; i < p->qtElementos; i++, j++) {
            novaPagina->chave[j] = p->chave[i];
            novaPagina->filho[j] = p->filho[i];
            novaPagina->qtElementos++;
        }
        
        // isso faz com que não podemos acessar mais os elementos que foram movidos para a nova página
        p->qtElementos = meio; 

        // ecadeia as páginas irmãs
        novaPagina->proximaFolha = p->proximaFolha;
        p->proximaFolha = novaPagina->indice; 
    } 
    
    else {
        // Se nó interno, a chave mediana sobe (não fica na nova página)
        for (int i = meio + 1, j = 0; i < p->qtElementos; i++, j++) {
            novaPagina->chave[j] = p->chave[i];
            novaPagina->filho[j] = p->filho[i];
            novaPagina->qtElementos++;
        }
        novaPagina->filho[novaPagina->qtElementos] = p->filho[p->qtElementos];
        p->qtElementos = meio; 
    }

    // Se nó interno, atualiza o ID do pai nos filhos transferidos (atualiza no disco)
    if (!p->ehfolha) {
        for (int i = 0; i <= novaPagina->qtElementos; i++) {
            Pagina filhoTemp;
            fseek(arquivo, sizeof(Cabecalho) + novaPagina->filho[i] * sizeof(Pagina), SEEK_SET);
            fread(&filhoTemp, sizeof(Pagina), 1, arquivo);
            filhoTemp.pai = novaPagina->indice;
            fseek(arquivo, sizeof(Cabecalho) + novaPagina->filho[i] * sizeof(Pagina), SEEK_SET);
            fwrite(&filhoTemp, sizeof(Pagina), 1, arquivo);
        }
    }

    fclose(arquivo);
    // se a página for a raiz
    if (p->pai == -1) {
        Pagina *novaRaiz = criaPagina();
        novaRaiz->indice = buscarPaginaLivre();
        novaRaiz->ehfolha = 0;
        novaRaiz->pai = -1;
        
        novaRaiz->chave[0] = chaveMediana;
        novaRaiz->filho[0] = p->indice;
        novaRaiz->filho[1] = novaPagina->indice;
        novaRaiz->qtElementos = 1;

        p->pai = novaRaiz->indice;
        novaPagina->pai = novaRaiz->indice;

        arquivo = fopen(arquivoArvore, "r+b");
        // escreve a nova raiz no arquivo
        if (arquivo != NULL) {
            header.raiz = novaRaiz->indice;
            header.qtdPaginas += 2; 
            fseek(arquivo, 0, SEEK_SET);
            fwrite(&header, sizeof(Cabecalho), 1, arquivo);

            fseek(arquivo, sizeof(Cabecalho) + novaRaiz->indice * sizeof(Pagina), SEEK_SET);
            fwrite(novaRaiz, sizeof(Pagina), 1, arquivo);
            fclose(arquivo);
        }

        free(novaRaiz); 
    } 
    // caso não seja a raiz, propaga a chave mediana para o pai
    else {
        Pagina *pai = criaPagina();
        arquivo = fopen(arquivoArvore, "r+b");
        if (arquivo != NULL) {
            fseek(arquivo, sizeof(Cabecalho) + p->pai * sizeof(Pagina), SEEK_SET);
            fread(pai, sizeof(Pagina), 1, arquivo);
            
            header.qtdPaginas++;
            fseek(arquivo, 0, SEEK_SET);
            fwrite(&header, sizeof(Cabecalho), 1, arquivo);
            fclose(arquivo);
        }

        // propaga a chave mediana para o pai, e faz a cisão caso nescessário
        inserirElemento(pai, chaveMediana, novaPagina->indice, comparar);
        
        free(pai); 
    }

    arquivo = fopen(arquivoArvore, "r+b");
    // escreve a página irmã no arquivo
    if (arquivo != NULL) {
        fseek(arquivo, sizeof(Cabecalho) + novaPagina->indice * sizeof(Pagina), SEEK_SET);
        fwrite(novaPagina, sizeof(Pagina), 1, arquivo);
        fclose(arquivo);
    }
    
    free(novaPagina); 
}

void verificarUnderflow(Pagina *pagina){
    Cabecalho header;

    // lê cabeçalho
    fseek(arquivoArvore, 0, SEEK_SET);
    fread(&header, sizeof(Cabecalho), 1, arquivoArvore);

    int minimo = ORDEM / 2;

    // não existe underflow
    if (pagina->qtElementos >= minimo)
        return;

    // caso a página seja raiz
    if (pagina->indice == header.raiz){
        // se a raiz ficou vazia, árvore fica vazia
        if (pagina->qtElementos == 0){
            header.raiz = -1;

            fseek(arquivoArvore, 0, SEEK_SET);
            fwrite(&header, sizeof(Cabecalho), 1, arquivoArvore);
        }
        return;
    }

    // carrega pai
    Pagina pai;

    fseek(arquivoArvore, sizeof(Cabecalho) + pagina->pai * sizeof(Pagina), SEEK_SET);
    fread(&pai, sizeof(Pagina), 1, arquivoArvore);

    // descobre posição da página no pai
    int pos = 0;

    while (pai.filho[pos] != pagina->indice) pos++;

    Pagina irmaAdjacente;

    // tenta pegar da irmã esquerda
    if (pos > 0){
        fseek(arquivoArvore, sizeof(Cabecalho) + pai.filho[pos-1] * sizeof(Pagina), SEEK_SET);
        fread(&irmaAdjacente, sizeof(Pagina), 1, arquivoArvore);

        if (irmaAdjacente.qtElementos > minimo){

            // move elementos da página para abrir espaço
            for (int i = pagina->qtElementos; i > 0; i--){
                pagina->chave[i] = pagina->chave[i-1];
                pagina->filho[i] = pagina->filho[i-1];
            }
            // pega maior elemento do irmão esquerdo

            pagina->chave[0] = irmaAdjacente.chave[irmaAdjacente.qtElementos-1];
            pagina->filho[0] = irmaAdjacente.filho[irmaAdjacente.qtElementos-1];
            pagina->qtElementos++;
            irmaAdjacente.qtElementos--;

            // salva a irmã

            fseek(arquivoArvore, sizeof(Cabecalho) + irmaAdjacente.indice*sizeof(Pagina), SEEK_SET);
            fwrite(&irmaAdjacente,sizeof(Pagina),1,arquivoArvore);

            return;
        }
    }

    //tenta pegar da irmã direita
    if (pos < pai.qtElementos){
        fseek(arquivoArvore, sizeof(Cabecalho) + pai.filho[pos+1]*sizeof(Pagina), SEEK_SET);
        fread(&irmaAdjacente,sizeof(Pagina),1,arquivoArvore);

        if (irmaAdjacente.qtElementos > minimo){

            pagina->chave[pagina->qtElementos] = irmaAdjacente.chave[0];
            pagina->filho[pagina->qtElementos] = irmaAdjacente.filho[0];
            pagina->qtElementos++;

            // remove primeiro elemento da irmã
            for(int i=0;i<irmaAdjacente.qtElementos-1;i++){
                irmaAdjacente.chave[i] = irmaAdjacente.chave[i+1];
                irmaAdjacente.filho[i] = irmaAdjacente.filho[i+1];
            }

            irmaAdjacente.qtElementos--;

            fseek(arquivoArvore, sizeof(Cabecalho) + irmaAdjacente.indice*sizeof(Pagina), SEEK_SET);
            fwrite(&irmaAdjacente,sizeof(Pagina),1,arquivoArvore);

            return;
        }
    }

    //se chegou aqui, não conseguiu redistribuir
    //faz concatenação !!!

    if (pos > 0){

        fseek(arquivoArvore, sizeof(Cabecalho) + pai.filho[pos-1]*sizeof(Pagina), SEEK_SET);
        fread(&irmaAdjacente,sizeof(Pagina),1,arquivoArvore);

        for(int i=0;i<pagina->qtElementos;i++){
            irmaAdjacente.chave[irmaAdjacente.qtElementos] = pagina->chave[i];
            irmaAdjacente.filho[irmaAdjacente.qtElementos] = pagina->filho[i];
            irmaAdjacente.qtElementos++;
        }

        // marca página como removida
        pagina->foiDeletada = 1;

        fseek(arquivoArvore, sizeof(Cabecalho) + irmaAdjacente.indice*sizeof(Pagina), SEEK_SET);
        fwrite(&irmaAdjacente,sizeof(Pagina),1,arquivoArvore);

        // remove referência no pai
        for(int i=pos;i<pai.qtElementos;i++){
            pai.filho[i] = pai.filho[i+1];
        }
        pai.qtElementos--;

        fseek(arquivoArvore, sizeof(Cabecalho) + pai.indice*sizeof(Pagina), SEEK_SET);
        fwrite(&pai,sizeof(Pagina),1,arquivoArvore);
    }
}

void ordenarPaginaFolha(Pagina *p, int (*comparar)(const void *, const void *)) {
    // a função de ordenação vai atuar panas na memória ram
    // as funções responsáveis por chamar ela que gravam isso no disco
    
    int j;
    void* aux1; 
    int aux2; 

    for (int i = 1; i < p->qtElementos; i++) {
        aux1 = p->chave[i];
        aux2 = p->filho[i]; 

        // Loop do Insertion Sort usando a função genérica de comparação
        for (j = i; j > 0 && comparar(aux1, p->chave[j - 1]) < 0; j--) {
            p->chave[j] = p->chave[j - 1]; 
            p->filho[j] = p->filho[j - 1];             
        }
        
        // Coloca os valores nos seus devidos lugares ordenados
        p->chave[j] = aux1;
        p->filho[j] = aux2;
    }
}


int buscarPaginaLivre(){

    //abre o arquivoArvore pra leitura
    FILE *arquivoArvore = fopen(arquivoArvore, "rb");
    if (arquivoArvore == NULL){
        printf("Erro ao abrir o arquivoArvore!\n");
        return;
    }
    //pula o cabecalho
    fseek(arquivoArvore, sizeof(Cabecalho), SEEK_SET);
    //le pagina a pagina até encontrar uma que foi deletada logicamente
    Pagina p;
    int i = 0;
    while(fread(&p, sizeof(Pagina), 1, arquivoArvore)){
        if (p.foiDeletada)
            break;
        i++;
    }
    //fecha o arquivoArvore
    fclose(arquivoArvore);
    
    //retorna indice da página já deletada ou o próximo índice livre (i) caso não tenha encontrado nenhuma página deletada
    return i;
    
}

// funcões para a árvore
void inicializarArvore(int ordem, int tamChave){
    
    FILE *arquivo = fopen(arquivoArvore, "rb+");
    // Confere se arquivoArvore  da árvore já não foi criado
    if (arquivo == NULL)
    {
        Cabecalho arvore;
        arvore.raiz = -1;
        arvore.qtdPaginas = 0;
        arvore.tamChave = tamChave;
        arvore.ordem = ordem; // podemos tirar isso e usar do define
        arvore.qtdPaginas = 0;

        FILE *arquivoArvore = fopen(arquivoArvore, "wb+");
        fwrite(&arvore, sizeof(Cabecalho), 1, arquivoArvore);
    }

    else 
        printf("Arquivo da árvore já existe!\n");

    fclose(arquivo);

}

void imprimirArvore();

Pagina buscarFolha(Cabecalho *header, const void *chave, int (*comparar)(const void *, const void *)){

    FILE *arquivoArvore = fopen(arquivoArvore, "rb");
    Pagina pagina;

    //Carrega a raiz
    fseek(arquivoArvore, sizeof(Cabecalho) + header->raiz * sizeof(Pagina), SEEK_SET);
    fread(&pagina, sizeof(Pagina), 1, arquivoArvore);

    // Enquanto não chegar em uma folha
    while (pagina.ehfolha == 0){
        int i = 0;

        // Descobre qual filho seguir
        while (i < pagina.qtElementos && comparar(chave, pagina.chave[i]) > 0) i++;
        
        // Carrega o filho escolhido
        fseek(arquivoArvore, sizeof(Cabecalho) + pagina.filho[i] * sizeof(Pagina), SEEK_SET);
        fread(&pagina, sizeof(Pagina), 1, arquivoArvore);
    }   
    return pagina;
}

int buscarChaveNaArvore(const void* chave, int *enderecoRegistro, int (*comparar)(const void*, const void*)){

    FILE *arquivoArvore = fopen(arquivoArvore, "rb");

    if (arquivoArvore == NULL){
        printf("Erro ao abrir o arquivoArvore!\n");
        return -1;
    }

    Cabecalho header;

    if (fread(&header, sizeof(Cabecalho), 1, arquivoArvore) != 1){
        printf("Erro ao ler o cabeçalho!!\n");
        fclose(arquivoArvore);
        return -1;
    }


    if (header.raiz == -1){
        printf("Árvore vazia!!\n");
        fclose(arquivoArvore);
        return -1;
    }

    // busca a folha
    Pagina p = buscarFolha(&header, chave, comparar);

    // procura a chave na folha
    for (int i = 0; i < p.qtElementos; i++){

        if (comparar(chave, p.chave[i]) == 0){

            *enderecoRegistro = p.filho[i];

            fclose(arquivoArvore);
            return 1;
        }
    }


    fclose(arquivoArvore);

    return -1;
}

int* buscarChavesIntervalo(const void *chaveMin, const void *chaveMax, int *qtEncontrados, int (*comparar)(const void*, const void*)){
        FILE *arquivoArvore = fopen(arquivoArvore, "rb");

    //verifica se abriu
    if (arquivoArvore == NULL)
        return NULL;

    //le cabeçalho

    Cabecalho header;

    if (fread(&header, sizeof(Cabecalho), 1, arquivoArvore) != 1){
        fclose(arquivoArvore);
        return NULL;
    }

    //verifica se a árvore existe
    if (header.raiz == -1){
        fclose(arquivoArvore);
        return NULL;
    }

    //Encontra a folha onde chaveMin estaria
    Pagina pagina = buscarFolha(&header, chaveMin, comparar);

    //aloca vetor com um tamanho inicial, se precisar de mais realloca
    int capacidade = 10;
    int *enderecos = (int*) malloc(capacidade * sizeof(int));
    *qtEncontrados = 0;

    bool terminou = false;

    while (!terminou){

        //percorre as chaves na folha
        for (int i = 0; i < pagina.qtElementos; i++){

            //se a chave encontrada for maior que a máxima, encerra pois já terminou o intervalo
            if (comparar(pagina.chave[i], chaveMax) > 0){
                terminou = true;
                break;
            }

            //se está no intervalo, compara com a minima
            if (comparar(pagina.chave[i], chaveMin) >= 0){
                //se a chave a página estiver entro do intervalo (chaveMin, chaveMax)

                //verifica se o vetor tem espaço suficiente
                if (*qtEncontrados == capacidade){
                    capacidade *= 2;
                    enderecos = (int*) realloc(enderecos, capacidade * sizeof(int));
                }

                //salva endereço no vetor e incrementa contador
                enderecos[*qtEncontrados] = pagina.filho[i];
                (*qtEncontrados)++;
            }
        }

        if (terminou)
            break;

        //acabou a última folha?
        if (pagina.proximaFolha == -1)
            break;

        // carrega a próxima folha
        fseek(arquivoArvore, sizeof(Cabecalho) + pagina.proximaFolha * sizeof(Pagina), SEEK_SET);
        fread(&pagina, sizeof(Pagina), 1, arquivoArvore);
    }

    fclose(arquivoArvore);

    return enderecos;

}

void inserirChaveNaArvore(const void *chave, int enderecoRegistro, size_t tamanhoChave, int (*comparar)(const void *, const void *)){

    FILE *arquivoArvore = fopen(arquivoArvore, "rb+");

    if (arquivoArvore == NULL){
        printf("Erro ao abrir o arquivoArvore!\n");
        return;
    }

    Cabecalho header;
    fread(&header, sizeof(Cabecalho), 1, arquivoArvore);

    // Árvore vazia
    if (header.raiz == -1){

        Pagina *raiz = criaPagina();
        inicializarPagina(raiz, 0, 1);
        inserirElemento(raiz, chave, enderecoRegistro, comparar);
        raiz->filho[0] = enderecoRegistro;
        header.raiz = 0;
        header.qtdPaginas = 1;

        fseek(arquivoArvore, 0, SEEK_SET);
        fwrite(&header, sizeof(Cabecalho), 1, arquivoArvore);

        fseek(arquivoArvore, sizeof(Cabecalho), SEEK_SET);
        fwrite(raiz, sizeof(Pagina), 1, arquivoArvore);

        destroiPagina(raiz);

        fclose(arquivoArvore);
        return;
    }

    //encontra a folha
    Pagina pagina = buscarFolha(&header, chave, comparar);

    //insere na folha
    inserirElemento(&pagina, chave, pagina.indice, comparar);
    pagina.filho[pagina.qtElementos - 1] = enderecoRegistro;
    ordenarPaginaFolha(&pagina, comparar);
    verificarOverflow(&pagina, comparar);

    //salva a página
   
    // ISSO A FUNÇÃO ISERIR ELEMENTO JÁ FAZ!!!!!!!!!!
    // FALAR COM AS MENINAS
   
    //fseek(arquivoArvore, sizeof(Cabecalho) + pagina.indice * sizeof(Pagina), SEEK_SET);
    //fwrite(&pagina, sizeof(Pagina), 1, arquivoArvore);
    //fclose(arquivoArvore);
}

void deletarChaveNaArvore(const void *chave, int (*comparar)(const void *, const void *)){

    FILE *arquivoArvore = fopen(arquivoArvore, "rb+");

    if (arquivoArvore == NULL){
        printf("Erro ao abrir o arquivoArvore!!!\n");
        return;
    }

    Cabecalho header;

    if (fread(&header, sizeof(Cabecalho), 1, arquivoArvore) != 1){
        fclose(arquivoArvore);
        return;
    }

    if (header.raiz == -1){
        printf("Árvore vazia!\n");
        fclose(arquivoArvore);
        return;
    }

    // encontra a folha
    Pagina pagina = buscarFolha(&header, chave, comparar);

    // tenta remover
    if (!removerElemento(&pagina, chave, comparar)) {
        printf("Chave não encontrada.\n");
        fclose(arquivoArvore);
        return;
    }

    verificarUnderflow(&pagina);

    fseek(arquivoArvore, sizeof(Cabecalho) + pagina.indice * sizeof(Pagina), SEEK_SET);
    fwrite(&pagina, sizeof(Pagina), 1, arquivoArvore);

    fclose(arquivoArvore);
}



