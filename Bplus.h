/*
 * ============================================================
 * ARQUIVO: Bplus.h
 * ============================================================
 * 
 * INTERFACE PÚBLICA DA ÁRVORE B+ GENÉRICA EM DISCO
 * 
 * ------------------------------------------------------------
 * PASSO 1 – CONSTANTES GLOBAIS
 * ------------------------------------------------------------
 * 1.1. Definir ORDEM da árvore (número máximo de chaves por nó).
 * 1.2. Definir TAMANHO_PAGINA (em bytes) para alinhamento com disco.
 * 1.3. Definir valores para RID_NULO (ponteiro de disco inválido).
 * 
 * ------------------------------------------------------------
 * PASSO 2 – ESTRUTURAS DOS NÓS (REPRESENTAÇÃO EM MEMÓRIA)
 * ------------------------------------------------------------
 * 2.1. Struct do nó INTERNO:
 *      - vetor de chaves (genérico, void*)
 *      - vetor de RIDs (filhos)
 *      - contador de chaves
 *      - RID do próprio nó (para persistência)
 * 2.2. Struct do nó FOLHA:
 *      - vetor de chaves (genérico, void*)
 *      - vetor de RIDs (apontando para os registros de dados)
 *      - RID do próximo nó folha (encadeamento para range scan)
 *      - contador de chaves
 *      - RID do próprio nó
 * 2.3. Struct do CABEÇALHO do arquivo de índice:
 *      - RID da raiz
 *      - RID da primeira folha
 *      - RID da lista de blocos livres
 *      - assinatura/versão do arquivo
 * 
 * ------------------------------------------------------------
 * PASSO 3 – TIPOS DAS FUNÇÕES DE CALLBACK (GENÉRICO)
 * ------------------------------------------------------------
 * 3.1. Definir tipo para comparador: int (*cmp)(const void*, const void*)
 * 3.2. Definir tipo para calculador de tamanho em bytes: size_t (*size)(const void*)
 * 3.3. Definir tipo para serializador: void (*write)(const void*, void* buffer)
 * 3.4. Definir tipo para desserializador: void (*read)(void* destino, const void* buffer)
 * 
 * ------------------------------------------------------------
 * PASSO 4 – PROTÓTIPOS DA API PÚBLICA
 * ------------------------------------------------------------
 * 4.1. Inicializar / abrir arquivo de índice: bplus_init()
 * 4.2. Inserir chave + RID do dado: bplus_insert()
 * 4.3. Buscar por igualdade: bplus_search()
 * 4.4. Busca por intervalo (range): bplus_range_search()
 * 4.5. Remover chave: bplus_delete()
 * 4.6. Imprimir estrutura hierárquica: bplus_print_tree()
 * 4.7. Fechar arquivo e liberar recursos: bplus_close()
 * 
 * ------------------------------------------------------------
 * PASSO 5 – FUNÇÕES AUXILIARES INTERNAS (NÃO EXPORTADAS)
 * ------------------------------------------------------------
 * 5.1. Localizar folha para uma chave (search_leaf)
 * 5.2. Dividir (split) nó interno / folha
 * 5.3. Redistribuir ou concatenar (merge) nós na remoção
 * 5.4. Atualizar cabeçalho do arquivo
 */