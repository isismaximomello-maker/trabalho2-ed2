#include "RH.h"
#include "Bplus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
    novaPessoa->contrato.status = 0;
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

    if (compararNome != 0) return compararNome; 

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

//manda o arquivo para a memoria

int carregar_funcionario(funcionario* f, long posicao) {
    FILE* arquivo = fopen("funcionarios.dat", "rb");
    if (arquivo == NULL) {
        return 0;
    }
    
    fseek(arquivo, posicao, SEEK_SET);
    size_t lido = fread(f, sizeof(funcionario), 1, arquivo);
    fclose(arquivo);
    
    return (lido == 1);
}

//imprimir o funcionario
void imprimir_funcionario(const funcionario* f) {
    printf("\n========================================\n");
    printf("FICHA CADASTRAL\n");
    printf("========================================\n");
    printf("Nome: %s\n", f->chave.nome);
    printf("Data Nascimento: %02d/%02d/%04d\n",
           f->chave.dataNascimento.dia,
           f->chave.dataNascimento.mes,
           f->chave.dataNascimento.ano);
    printf("Mae: %s\n", f->filiacao.mae);
    printf("Pai: %s\n", f->filiacao.pai);
    printf("Endereco: %s\n", f->contato.endereco);
    printf("Telefone: %s\n", f->contato.telefone);
    printf("Data Contratacao: %02d/%02d/%04d\n",
           f->contrato.dataContrato.dia,
           f->contrato.dataContrato.mes,
           f->contrato.dataContrato.ano);
    printf("Status: %s\n", f->contrato.status ? "ATIVO" : "INATIVO");
    if (!f->contrato.status) {
        printf("Data Desligamento: %02d/%02d/%04d\n",
               f->contrato.dataDesligamento.dia,
               f->contrato.dataDesligamento.mes,
               f->contrato.dataDesligamento.ano);
    }
    printf("========================================\n");
}

void imprimir_funcionario_resumido(const funcionario* f) {
    printf("\n----------------------------------------\n");
    printf("Nome: %s\n", f->chave.nome);
    printf("Data Nascimento: %02d/%02d/%04d\n",
           f->chave.dataNascimento.dia,
           f->chave.dataNascimento.mes,
           f->chave.dataNascimento.ano);
    printf("Status: %s\n", f->contrato.status ? "ATIVO" : "INATIVO");
    printf("----------------------------------------\n");
}



void rh_inserir_funcionario() {
    char nome[100], mae[100], pai[100], endereco[200], telefone[20];
    data dataNasc;
    data dataCont;
    long posicao;
    chaveComposta chave;
    
    printf(" \nINSERIR NOVO FUNCIONARIO\n");

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

    if (bplus_buscar(&chave, &posicao)) {
        funcionario f;
        if (carregar_funcionario(&f, posicao)) {
            printf("\nFuncionario ja cadastrado com esta data!\n");
            imprimir_funcionario_resumido(&f);
            printf("Deseja atualizar os dados? (s/n): ");
            char resp;
            scanf(" %c", &resp);
            getchar();
            if (resp == 's' || resp == 'S') {
                // Aqui poderia implementar atualização, mas por simplicidade,
                // apenas informamos que não foi implementado.
                printf("Atualizacao nao implementada neste exemplo.\n");
            }
        }
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
    funcionario *novo = criarPessoa(nome, dataNasc.dia, dataNasc.mes, dataNasc.ano, mae, pai, endereco, telefone);
    
    
    // 9. PREENCHER DADOS CONTRATUAIS
    novo->contrato.dataContrato = dataCont;
    novo->contrato.status = 1;
    novo->contrato.dataDesligamento.dia = 0;
    novo->contrato.dataDesligamento.mes = 0;
    novo->contrato.dataDesligamento.ano = 0;
    
    // 10. SALVAR NO ARQUIVO
    if (!salvar_funcionario(novo, &posicao)) {
        printf("Erro ao salvar funcionario.\n");
        free(novo);
        return;
    }
    
    bplus_inserir(&chave, posicao);

    // 11. CONFIRMAR
    printf("\nFuncionario inserido com sucesso!\n");
    
    //fazer imprime funcionario? 

    free(novo);
}

void rh_excluir_funcionario() {
    char nome[100];
    long posicoes[100];
    int qtd;

    printf("\n=== EXCLUIR FUNCIONARIO ===\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';

    qtd = bplus_buscar_por_nome(nome, posicoes, 100);

    if (qtd == 0) {
        printf("Nenhum funcionario encontrado com esse nome.\n");
        return;
    }

    int idx = 0; // índice do registro a excluir
    if (qtd == 1) {
        idx = 0;
    } else {
        printf("Encontrados %d homonimos:\n", qtd);
        for (int i = 0; i < qtd; i++) {
            funcionario f;
            if (carregar_funcionario(&f, posicoes[i])) {
                printf("[%d] ", i+1);
                imprimir_funcionario_resumido(&f);
            }
        }
        printf("Digite o numero do funcionario que deseja excluir: ");
        scanf("%d", &idx);
        getchar();
        idx--; // ajusta para índice 0-based
        if (idx < 0 || idx >= qtd) {
            printf("Opcao invalida.\n");
            return;
        }
    }

    // Exibe dados (sem histórico) e pede confirmação
    funcionario f;
    if (!carregar_funcionario(&f, posicoes[idx])) {
        printf("Erro ao carregar dados.\n");
        return;
    }
    printf("\nDados do funcionario a ser excluido:\n");
    imprimir_funcionario_resumido(&f);
    printf("Confirma a exclusao? (s/n): ");
    char resp;
    scanf(" %c", &resp);
    getchar();
    if (resp != 'n' && resp != 'N') {
        printf("Exclusao cancelada.\n");
        return;
    }
    
    // Remove do índice
    chaveComposta chave = f.chave;
    bplus_remover(&chave);
    
    // Opcional: marcar como excluído no arquivo de dados (não fizemos, mas poderíamos)
    printf("Funcionario removido com sucesso.\n");
}


void rh_buscar_funcionario() {
    char nome[100];
    long posicoes[100];
    int qtd;
    
    printf("\n=== BUSCAR FUNCIONARIO ===\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';
    
    // Busca todos os registros com esse nome (homônimos)
    qtd = bplus_buscar_por_nome(nome, posicoes, 100);
    
    if (qtd == 0) {
        printf("Nenhum funcionario encontrado com esse nome.\n");
        return;
    }
    
    if (qtd == 1) {
        funcionario f;
        if (carregar_funcionario(&f, posicoes[0])) {
            imprimir_funcionario(&f);
        } else {
            printf("Erro ao carregar dados.\n");
        }
        return;
    }
    
    // Múltiplos homônimos
    printf("Encontrados %d funcionarios com o nome '%s':\n", qtd, nome);
    for (int i = 0; i < qtd; i++) {
        funcionario f;
        if (carregar_funcionario(&f, posicoes[i])) {
            printf("[%d] ", i+1);
            imprimir_funcionario_resumido(&f);
        }
    }
    
    printf("Digite o numero correspondente ao funcionario desejado: ");
    int escolha;
    scanf("%d", &escolha);
    getchar();
    
    if (escolha < 1 || escolha > qtd) {
        printf("Opcao invalida.\n");
        return;
    }
    
    funcionario f;
    if (carregar_funcionario(&f, posicoes[escolha-1])) {
        imprimir_funcionario(&f);
    } else {
        printf("Erro ao carregar dados.\n");
    }
}


void rh_listar_intervalo() {


    char nomeA[100], nomeB[100];
    chaveComposta chaveMin,chaveMax;
    printf("\n=== LISTAGEM POR INTERVALO ===\n");
    printf("Nome (A): ");
    fgets(nomeA, sizeof(nomeA), stdin);
    nomeA[strcspn(nomeA, "\n")] = '\0';
    printf("Nome (B): ");
    fgets(nomeB, sizeof(nomeB), stdin);
    nomeB[strcspn(nomeB, "\n")] = '\0';
    
    //faz as chaves para comparar
    strcpy(chaveMin.nome, nomeA);
    chaveMin.dataNascimento.dia = 0;
    chaveMin.dataNascimento.mes = 0;
    chaveMin.dataNascimento.ano = 0;
    
    strcpy(chaveMax.nome, nomeB);
    chaveMax.dataNascimento.dia = 31;
    chaveMax.dataNascimento.mes = 12;
    chaveMax.dataNascimento.ano = 9999;

    printf("\nFuncionarios no intervalo (%s, %s):\n", nomeA, nomeB);
    printf("----------------------------------------\n");
   
    imprimirChavesIntervalo(&chaveMin, &chaveMax);

    printf("----------------------------------------\n");
}
    


void rh_exibir_estrutura() {
    printf("\n=== ESTRUTURA DA ARVORE B+ ===\n");
    bplus_imprimir_estrutura();
}
