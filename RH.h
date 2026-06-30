
#include <stdio.h>
#include <stdlib.h>


typedef struct funcionario {
    chaveComposta chave;
    afiliacao filiacao ;
    dadosContato contato;
    float historicoPagamentos[12];
    dadosContratuais contrato;
} funcionario;


typedef struct data {
    int dia;
    int mes;
    int ano;
} data;

typedef struct dadosContato {
    char endereco[200];
    char telefone[20];
} dadosContato;

typedef struct afiliacao{
    char mae [100];
    char pai [100];

}afiliacao;

typedef struct dadosContratuais{
    data dataContrato;
    int status;
    data dataDesligamento;

}dadosContratuais;

typedef struct chaveComposta{
    char nome[100];
    data dataNascimento;
}chaveComposta;

funcionario *criar_funcionario(char *nome, int dia, int mes, int ano, char *mae, char *pai, char *endereco, char *telefone);

int comparar_chave(const void *a, const void *b);

int salvar_funcionario(const funcionario *f, int *posicao);

int carregar_funcionario(funcionario *f, int posicao);

void imprimir_funcionario(const funcionario *f);

void imprimir_funcionario_resumido(const funcionario *f);

void rh_inserir_funcionario(void);

void rh_buscar_funcionario(void);

void rh_excluir_funcionario(void);

void rh_listar_intervalo(void);
