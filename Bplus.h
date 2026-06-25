#define ORDEM 4
#define TAMANHO_Pagina 4096
#include <stdbool.h>
#include "RH.h"


typedef struct Pagina {
    funcionario chave[ORDEM + 1];
    int filho[ORDEM + 2];
    int pai; //pai do vetor de chave
    int indice; //endereço da página
    int proximaFolha; //endereço irma adjacente
    int qtElementos; //qt elementos ocupados 
    int ehfolha;
    int foiDeletada; //marcador lógico
} Pagina;

typedef struct {
    long raiz_rid;          // Onde a raiz da árvore está no arquivo (RID)
    long topo_lista_livre;  // Ponteiro para reuso de nós deletados (isso o GPT que fez, funciona como uma lista emcadeada
                            // é uma lista encadeada de nós livres, cada nó livre tem um ponteiro para o próximo nó livre
    int tamanho_chave;      // Configurado na inicialização (ex: sizeof(ChaveComposta))
    int tamanho_dado;       // Configurado na inicialização (ex: sizeof(Funcionario))
} CabecalhoArquivo;

// funções para a página
Pagina *criaPagina(int ordem);
void inicializarPagina(Pagina *Pagina, int ordem, int index, int tipo);
void destroiPagina(Pagina *p);
void inserirElemento(Pagina *p, int chave);
void removerElemento(Pagina *p, int chave);
void verificarOverflow(Pagina *p);
void verificarUnderflow(Pagina *p);
void ordenarPaginaFolha(Pagina *p);

// funções para a árvore
void inicializarArvore();
void imprimirArvore();
int buscarChave(chaveComposta chave, int *indexPagina);
void inserirChave(chaveComposta chave, int enderecoRegistro);
void deletarChave(chaveComposta chave);
int removerChave(chaveComposta chave);
void imprimirChavesIntervalo(chaveComposta inicial, chaveComposta final);