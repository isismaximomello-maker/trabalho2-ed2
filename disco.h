/*
 * ============================================================
 * ARQUIVO: disk_manager.h
 * ============================================================
 * 
 * GERENCIAMENTO DE PÁGINAS E PONTEIROS DE DISCO
 * 
 * ------------------------------------------------------------
 * PASSO 1 – DEFINIÇÕES DE ESTRUTURAS DE DISCO
 * ------------------------------------------------------------
 * 1.1. Definir struct Page (representação crua do nó em disco).
 * 1.2. Definir struct FreeBlock (para lista encadeada de blocos livres).
 * 
 * ------------------------------------------------------------
 * PASSO 2 – FUNÇÕES DE LEITURA / ESCRITA DIRETA
 * ------------------------------------------------------------
 * 2.1. ler_pagina(rid, buffer) – fseek + fread
 * 2.2. escrever_pagina(rid, buffer) – fseek + fwrite
 * 2.3. alocar_nova_pagina() – retorna RID disponível (reuso ou append)
 * 2.4. liberar_pagina(rid) – insere RID na lista de blocos livres
 * 
 * ------------------------------------------------------------
 * PASSO 3 – CONTROLE DO ARQUIVO
 * ------------------------------------------------------------
 * 3.1. abrir_arquivo_índice(nome, modo)
 * 3.2. fechar_arquivo_índice()
 * 3.3. ler_cabecalho() e escrever_cabecalho()
 * 3.4. sincronizar_escritas (fflush)
 * 
 * ------------------------------------------------------------
 * PASSO 4 – MAPEAMENTO ENTRE NÓ E PÁGINA
 * ------------------------------------------------------------
 * 4.1. converter_no_para_buffer(nó, buffer)
 * 4.2. converter_buffer_para_no(buffer, nó)
 * 4.3. garantir que o buffer tenha o tamanho exato de TAMANHO_PAGINA
 */