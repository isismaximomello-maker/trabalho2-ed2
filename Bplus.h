#ifndef Bplus_H

#define ORDEM 4

typedef struct {
    int ordem, qtdPaginas, raiz; //raiz recebe -1 na criação do cabeçalho == VAZIA
    size_t tamChave;
} Cabecalho;

typedef struct Pagina{
    void *chave[ORDEM + 1];
    int filho[ORDEM + 2];
    int pai; //pai do vetor de Chave
    int indice; //endereço da página
    int proximaFolha; //endereço irma adjacente
    int qtElementos; //qt elementos ocupados 
    int ehfolha; // 0 == pag interna e 1 == pag folha
    int foiDeletada; //marcador lógico
} Pagina;

// funções para a página
Pagina *criaPagina(); // eduardo
void inicializarPagina(Pagina *pagina, int indice, int tipo); // eduardo
void destroiPagina(Pagina *p); // eduardo
void inserirElemento(Pagina *p, const void *chave); // eduardo
void removerElemento(Pagina *p, const void *chave); // isis
void verificarOverflow(Pagina *p); // eduardo
void verificarUnderflow(Pagina *p); // isis
void ordenarPaginaFolha(Pagina *p); // eduardo
int buscarPaginaLivre(); // isis

// funções para a árvore
void inicializarArvore(char* nomeArquivo, int ordem, int tamChave, int (*comparar)(const void*, const void*)); // eduardo
void imprimirArvore();
int buscarChave(const void *chave, int (*comparar)(const void*, const void*)); // isis
void inserirChave(const void *chave, int resgistro); // isis
void deletarChave(const void *chave); // isis
void imprimirChavesIntervalo(const void *chave_min, const void *chave_max);

#endif