#ifndef Bplus_H

#define ORDEM 4

typedef struct{
    char valor[100];
} Chave;

typedef struct Pagina{
    int Chave[ORDEM + 1];
    int filho[ORDEM + 2];
    int pai; //pai do vetor de Chave
    int indice; //endereço da página
    int proximaFolha; //endereço irma adjacente
    int qtElementos; //qt elementos ocupados 
    int ehfolha;
    int foiDeletada; //marcador lógico
} Pagina;

// funções para a página
Pagina *criaPagina(); // eduardo
void inicializarPagina(Pagina *pagina, int indice, int tipo); // eduardo
void destroiPagina(Pagina *p); // eduardo
void inserirElemento(Pagina *p, Chave chave); // eduardo
void removerElemento(Pagina *p, Chave chave); // isis
void verificarOverflow(Pagina *p); // eduardo
void verificarUnderflow(Pagina *p); // isis
void ordenarPaginaFolha(Pagina *p); // eduardo
int buscarPaginaLivre(); // isis

// funções para a árvore
void inicializarArvore(); // eduardo
void imprimirArvore();
int buscarChave(Chave chave, int *indexPagina); // isis
void inserirChave(Chave chave, int enderecoRegistro); // isis
void deletarChave(Chave chave); // isis
void imprimirChavesIntervalo(Chave inicial, Chave final);

#endif