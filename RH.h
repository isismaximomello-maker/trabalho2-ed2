/*
 * ============================================================
 * ARQUIVO: rh_types.h
 * ============================================================
 * 
 * ESTRUTURAS ESPECÍFICAS DO SISTEMA DE RH
 * 
 * ------------------------------------------------------------
 * PASSO 1 – DEFINIÇÃO DA DATA
 * ------------------------------------------------------------
 * 1.1. Struct Data com dia, mês, ano (inteiros).
 * 1.2. Função auxiliar para comparar duas datas (usada no desempate).
 * 1.3. Função para formatar data em string (para exibição).
 * 
 * ------------------------------------------------------------
 * PASSO 2 – DEFINIÇÃO DO HISTÓRICO DE PAGAMENTOS
 * ------------------------------------------------------------
 * 2.1. Struct Pagamento com mês/ano, valor (double), status (pago/pendente).
 * 2.2. Vetor estático de 12 posições para armazenar os últimos 12 meses.
 * 2.3. Função para inicializar o histórico vazio.
 * 
 * ------------------------------------------------------------
 * PASSO 3 – DEFINIÇÃO DA CHAVE COMPOSTA (NOME + DATA NASCIMENTO)
 * ------------------------------------------------------------
 * 3.1. Struct ChaveComposta:
 *      - nome: char[100] (tamanho fixo).
 *      - data_nascimento: Data.
 * 3.2. Importante: ordem de comparação (nome primeiro, depois data).
 * 
 * ------------------------------------------------------------
 * PASSO 4 – DEFINIÇÃO DO REGISTRO COMPLETO DO FUNCIONÁRIO
 * ------------------------------------------------------------
 * 4.1. Campos obrigatórios:
 *      - Nome (string)
 *      - Data de Nascimento
 *      - Filiação (mãe e pai) – strings
 *      - Dados de Contato (endereço, telefone) – strings
 *      - Dados Contratuais (data_contratacao, status Ativo/Inativo, data_desligamento)
 *      - Histórico de Pagamentos (vetor de 12 Pagamentos)
 * 4.2. Definir tamanho máximo para cada string (ex: 100 caracteres).
 * 
 * ------------------------------------------------------------
 * PASSO 5 – CONSTANTES E MACROS
 * ------------------------------------------------------------
 * 5.1. Tamanho total do registro em bytes (para uso nos callbacks).
 * 5.2. Macros para acessar campos com facilidade.
 */