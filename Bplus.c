#include "Bplus.h"
#include <stdio.h>
#include <stdlib.h>

// funções para a página
Pagina *criaPagina();
void inicializarPagina(Pagina *pagina, int indice, int tipo);
void destroiPagina(Pagina *p);
void inserirElemento(Pagina *p, Chave chave);
void removerElemento(Pagina *p, Chave chave);
void verificarOverflow(Pagina *p);
void verificarUnderflow(Pagina *p);
void ordenarPaginaFolha(Pagina *p);

// funções para a árvore
void inicializarArvore();
void imprimirArvore();
int buscarChave(Chave chave, int *indexPagina);
void inserirChave(Chave chave, int enderecoRegistro);
void deletarChave(Chave chave);
void imprimirChavesIntervalo(Chave inicial, Chave final);