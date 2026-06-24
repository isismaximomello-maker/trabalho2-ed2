#include <RH.h>
#include<Bplus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================
 * ARQUIVO: rh_system.c
 * ============================================================
 * 
 * IMPLEMENTAÇÃO DAS FUNÇÕES CALLBACK E DA LÓGICA DE NEGÓCIO DO RH
 * 
 * ------------------------------------------------------------
 * PASSO 1 – FUNÇÕES CALLBACK PARA A ÁRVORE B+
 * ------------------------------------------------------------
 * 1.1. comparar_chave_composta(const void* a, const void* b)
 *      - Retornar -1, 0 ou 1 (prioridade: nome, depois data_nasc).
 * 1.2. tamanho_registro(const void* chave) – retorna sizeof(ChaveComposta).
 * 1.3. escrever_registro(const void* chave, void* buffer)
 *      - Copiar os campos da struct para o buffer (sem padding).
 * 1.4. ler_registro(void* destino, const void* buffer)
 *      - Copiar do buffer para a struct.
 * 
 * ------------------------------------------------------------
 * PASSO 2 – FUNÇÃO DE INSERIR FUNCIONÁRIO (FLUXO COMPLETO)
 
 * ------------------------------------------------------------
 * 2.1. Solicitar Nome e Data Nascimento.
 * 2.2. Buscar na árvore se a chave composta já existe.
 * 2.3. Se existir: exibir dados atuais e perguntar se quer atualizar.
 * 2.4. Se não existir: solicitar demais campos, inicializar pagamentos vazio.
 * 2.5. Montar a struct Funcionario e inserir na B+ (chave + RID do dado).
 * 2.6. O RID do dado pode ser o próprio posicao do arquivo de dados (ou nó folha).
 * 
 * ------------------------------------------------------------
 * PASSO 3 – FUNÇÃO DE BUSCAR FUNCIONÁRIO (COM DESEMPATE)
 * ------------------------------------------------------------
 * 3.1. Solicitar Nome.
 * 3.2. Buscar todos os registros com esse nome (range com mesmo prefixo).
 * 3.3. Se apenas um: exibir ficha completa.
 * 3.4. Se vários (homônimos): listar nome + data_nasc, pedir para escolher.
 * 3.5. Exibir ficha completa do selecionado (incluindo histórico).
 * 
 * ------------------------------------------------------------
 * PASSO 4 – FUNÇÃO DE EXCLUIR FUNCIONÁRIO (COM CONFIRMAÇÃO)
 * ------------------------------------------------------------
 * 4.1. Seguir fluxo de busca (item 3) para encontrar o registro exato.
 * 4.2. Exibir dados cadastrais (sem histórico) e pedir confirmação (S/N).
 * 4.3. Se confirmado: chamar bpl us_delete() com a chave composta.
 * 4.4. Remover também o dado do arquivo de dados (ou marcar como inativo).
 * 
 * ------------------------------------------------------------
 * PASSO 5 – FUNÇÃO DE LISTAR POR INTERVALO (RANGE)
 * ------------------------------------------------------------
 * 5.1. Solicitar Nome A e Nome B (strings delimitadoras).
 * 5.2. Montar chaves compostas com esses nomes (data mínima/máxima).
 * 5.3. Chamar bplus_range_search() para obter lista de RIDs.
 * 5.4. Para cada RID, carregar o funcionário e exibir (nome e data).
 * 
 * ------------------------------------------------------------
 * PASSO 6 – FUNÇÕES AUXILIARES DE E/S E FORMATAÇÃO
 * ------------------------------------------------------------
 * 6.1. ler_string_segura(char* destino, int tamanho).
 * 6.2. ler_data(Data* d) – validar dia/mês/ano.
 * 6.3. imprimir_ficha_completa(Funcionario* f).
 * 6.4. imprimir_historico_pagamentos(Pagamento* hist[12]).
 * 6.5. serializar_funcionario() e desserializar_funcionario() para arquivo de dados.
 */

//cria pessoa
funcionario* criarPessoa(char* nome, int dia, int mes, int ano, char* mae, char* pai ,char*endereco, char* telefone) {
    funcionario* novaPessoa = (funcionario*)malloc(sizeof(funcionario));
    if (novaPessoa == NULL) {
        return NULL;
    }
    
    // nome e data
    strcpy(novaPessoa->chave.nome, nome);
    novaPessoa->chave.dataNascimento.dia = dia;
    novaPessoa->chave.dataNascimento.mes = mes;
    novaPessoa->chave.dataNascimento.ano = ano;
    
    // mae pai
    strcpy(novaPessoa->filiacao.mae, mae);
    strcpy(novaPessoa->filiacao.pai, pai);
    
    // contato
    strcpy(novaPessoa->contato.endereco, endereco);
    strcpy(novaPessoa->contato.telefone, telefone);
    
    // inicializa contrato
    novaPessoa->contrato.dataContrato.dia = 0;
    novaPessoa->contrato.dataContrato.mes = 0;
    novaPessoa->contrato.dataContrato.ano = 0;
    novaPessoa->contrato.status = false;
    novaPessoa->contrato.dataDesligamento.dia = 0;
    novaPessoa->contrato.dataDesligamento.mes = 0;
    novaPessoa->contrato.dataDesligamento.ano = 0;

    //zera os pagamentos;
    for (int i = 0; i < 12; i++) {
        novaPessoa->historicoPagamentos[i] = 0.0;
    }

    
    return novaPessoa;
}

//calbacks
int comparar_chave(const void*a, const void*b){
    const chaveComposta *chave1 = (const chaveComposta *)a;
    const chaveComposta *chave2 = (const chaveComposta *)b;

    int compararNome = strcmp(chave1->nome,chave2->nome);
    if (compararNome != 0) return 2;


    if (chave1->dataNascimento.ano != chave2->dataNascimento.ano) {
        return (chave1->dataNascimento.ano < chave2->dataNascimento.ano) ? -1 : 1;
    }
    if (chave1->dataNascimento.mes != chave2->dataNascimento.mes) {
        return (chave1->dataNascimento.mes < chave2->dataNascimento.mes) ? -1 : 1;
    }
    if (chave1->dataNascimento.dia != chave2->dataNascimento.dia) {
        return (chave1->dataNascimento.dia < chave2->dataNascimento.dia) ? -1 : 1;
    }
    return 0; //data igual

}

//callback 
size_t tamanho_chave(const void* chave) {
    return sizeof(chaveComposta);
}

void escrever_chave(const void* chave, void* buffer) {
    memcpy(buffer, chave, sizeof(chaveComposta));
}

void ler_chave(void* destino, const void* buffer) {
    memcpy(destino, buffer, sizeof(chaveComposta));
}

//precisa??
// //serializacao funcionario

// void serializar_funcionario(const funcionario* f, void* buffer) {
//     memcpy(buffer, f, sizeof(funcionario));
// }

// //desserializacao funcionario
// void desserializar_funcionario(funcionario* f, const void* buffer) {
//     memcpy(f, buffer, sizeof(funcionario));
// }





//CONTINUAR DAQUIIIIIIIIIIIIII



int salvar_funcionario(const funcionario* f, long* posicao) {
    FILE* arquivo = fopen("funcionarios.dat", "a+b");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo.\n");
        return 0;
    }

    
    fseek(arquivo, 0, SEEK_END);
    *posicao = ftell(arquivo);
    
    size_t escrito = fwrite(f, sizeof(funcionario), 1, arquivo);
    fclose(arquivo);
    
    return (escrito == 1);
}


void rh_inserir_funcionario() {
    char nome[100], mae[100], pai[100], endereco[200], telefone[20];
    data dataNasc;
    data dataCont;
    long posicao;
    int encontrado;
    funcionario* novo;
    chaveComposta chave;
    
    printf("  INSERIR NOVO FUNCIONARIO\n");

    // 1. NOME
    printf("Nome: ");
    fgets(nome, 100, stdin);
    nome[strcspn(nome, "\n")] = '\0' ;//coloca o \0 no final
    
    // 2. DATA NASCIMENTO
    printf("Data de Nascimento (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &dataNasc.dia, &dataNasc.mes, &dataNasc.ano);
    getchar();

    // PRECISA DE FUNCOES AINDA NAO IMPLEMENTADAS, APENAS TESTE DE ESTRUTURA
    strcpy(chave.nome, nome);
    chave.dataNascimento = dataNasc;

    encontrado = bplus_procura(&chave);

    if (encontrado != -1) {
        printf("\n Funcionario ja cadastrado com esta data!\n");
        //chamar para atualizar funcionario;
        return;
    }

    
    // 3. MAE
    printf("Nome da Mae: ");
    fgets(mae, 100, stdin);
    mae[strcspn(mae, "\n")] = '\0';
    
    // 4. PAI
    printf("Nome do Pai: ");
    fgets(pai, 100, stdin);
    pai[strcspn(pai, "\n")] = '\0';
    
    // 5. ENDERECO
    printf("Endereco: ");
    fgets(endereco, 200, stdin);
    endereco[strcspn(endereco, "\n")] = '\0';
    
    // 6. TELEFONE
    printf("Telefone: ");
    fgets(telefone, 20, stdin);
    telefone[strcspn(telefone, "\n")] = '\0';
    
    // 7. DATA CONTRATACAO
    printf("Data de Contratacao (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &dataCont.dia, &dataCont.mes, &dataCont.ano);
    getchar();
    
    // 8. CRIAR FUNCIONARIO (usando dataNasc, não dataCont!)
    novo = criarPessoa(nome, dataNasc.dia, dataNasc.mes, dataNasc.ano, mae, pai, endereco, telefone);
    
    
    // 9. PREENCHER DADOS CONTRATUAIS
    novo->contrato.dataContrato = dataCont;
    novo->contrato.status = true ;
    novo->contrato.dataDesligamento.dia = 0;
    novo->contrato.dataDesligamento.mes = 0;
    novo->contrato.dataDesligamento.ano = 0;
    
    // 10. SALVAR NO ARQUIVO
    if (!salvar_funcionario(novo, &posicao)) {
        printf("Erro ao salvar funcionario.\n");
        free(novo);
        return;
    }
    
    // 11. CONFIRMAR
    printf("\nFuncionario inserido com SUCESSO!\n");
    printf("   posicao: %ld\n", posicao);
    
    //fazer imprime funcionario? 

    free(novo);
}




