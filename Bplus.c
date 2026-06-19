/*
 * ============================================================
 * ARQUIVO: Bplus.c
 * ============================================================
 * 
 * IMPLEMENTAÇÃO DO NÚCLEO DA ÁRVORE B+ EM DISCO
 * 
 * ------------------------------------------------------------
 * PASSO 1 – VARIÁVEIS ESTÁTICAS GLOBAIS (ESTADO DA ÁRVORE)
 * ------------------------------------------------------------
 * 1.1. FILE* para o arquivo de índice.
 * 1.2. Cabeçalho carregado em memória (raiz, primeira folha, free_list).
 * 1.3. Callbacks registrados (cmp, size, write, read).
 * 
 * ------------------------------------------------------------
 * PASSO 2 – IMPLEMENTAÇÃO DA INICIALIZAÇÃO E FINALIZAÇÃO
 * ------------------------------------------------------------
 * 2.1. bplus_init() – abre/cria arquivo, lê cabeçalho, valida assinatura.
 * 2.2. bplus_close() – escreve cabeçalho, fecha arquivo.
 * 
 * ------------------------------------------------------------
 * PASSO 3 – IMPLEMENTAÇÃO DA BUSCA POR IGUALDADE
 * ------------------------------------------------------------
 * 3.1. Percorrer da raiz até a folha usando comparação.
 * 3.2. Na folha, varrer chaves e retornar RID do dado.
 * 3.3. Tratar chave não encontrada.
 * 
 * ------------------------------------------------------------
 * PASSO 4 – IMPLEMENTAÇÃO DA BUSCA POR INTERVALO (RANGE)
 * ------------------------------------------------------------
 * 4.1. Encontrar a primeira folha com chave >= limite inferior.
 * 4.2. Percorrer folhas encadeadas até chave > limite superior.
 * 4.3. Coletar todos os RIDs dentro do intervalo.
 * 
 * ------------------------------------------------------------
 * PASSO 5 – IMPLEMENTAÇÃO DA INSERÇÃO (COM SPLIT)
 * ------------------------------------------------------------
 * 5.1. Função recursiva ou iterativa para descer até a folha.
 * 5.2. Inserir chave na folha (ordem mantida via callback).
 * 5.3. Se folha transbordar: dividir (split) e promover chave mediana.
 * 5.4. Subir a chave promovida para o nó pai (recursivamente).
 * 5.5. Se a raiz transbordar: criar nova raiz e aumentar nível.
 * 5.6. Atualizar encadeamento entre folhas (próximo RID).
 * 
 * ------------------------------------------------------------
 * PASSO 6 – IMPLEMENTAÇÃO DA REMOÇÃO (REDISTRIBUIÇÃO / MERGE)
 * ------------------------------------------------------------
 * 6.1. Localizar a folha que contém a chave.
 * 6.2. Remover a chave da folha.
 * 6.3. Se folha ficar abaixo do preenchimento mínimo:
 *      6.3.1. Tentar redistribuir (pegar chave emprestada) do irmão.
 *      6.3.2. Caso contrário, concatenar (merge) com o irmão.
 * 6.4. Subir a correção (ajuste de chave separadora) para os pais.
 * 6.5. Se a raiz ficar vazia, atualizar raiz para o único filho.
 * 
 * ------------------------------------------------------------
 * PASSO 7 – IMPLEMENTAÇÃO DA IMPRESSÃO HIERÁRQUICA
 * ------------------------------------------------------------
 * 7.1. Função recursiva que percorre níveis.
 * 7.2. Imprimir indentação conforme profundidade.
 * 7.3. Mostrar chaves (usando callback de impressão ou formatação).
 * 7.4. Destacar limites entre nós e níveis.
 * 
 * ------------------------------------------------------------
 * PASSO 8 – FUNÇÕES INTERNAS DE MANIPULAÇÃO DE NÓS
 * ------------------------------------------------------------
 * 8.1. criar_no_interno() e criar_no_folha().
 * 8.2. destruir_no() (apenas memória, não disco).
 * 8.3. carregar_no_do_disco(rid) e salvar_no_em_disco(no).
 * 8.4. encontrar_posicao_para_inserir() e encontrar_posicao_para_buscar().
 */