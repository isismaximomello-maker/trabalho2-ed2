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
Pagina *criaPagina();
void inicializarPagina(Pagina *pagina, int indice, int tipo);
void destroiPagina(Pagina *p);
void inserirElemento(Pagina *p, Chave chave);
void removerElemento(Pagina *p, Chave chave);
void verificarOverflow(Pagina *p);
void verificarUnderflow(Pagina *p);
void ordenarPaginaFolha(Pagina *p);
int buscarPaginaLivre();

// funções para a árvore
void inicializarArvore();
void imprimirArvore();
int buscarChave(Chave chave, int *indexPagina);
void inserirChave(Chave chave, int enderecoRegistro);
void deletarChave(Chave chave);
void imprimirChavesIntervalo(Chave inicial, Chave final);

#endif