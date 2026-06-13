#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "funcoes.h"

static void ins_asm(char *out, const typ_ins *ins) {
    if (!ins || ins->instrucao_bruta == 0) {
        strcpy(out, "NOP");
        return;
    }
    asm_gerador_char(out, (typ_ins *)ins);
    out[strcspn(out, "\n")] = '\0';
}

static void print_sinais(const bool sinais[8]) {
    if (!sinais) return;
    printf("  [RgW:%d MmR:%d MmW:%d Brc:%d Jmp:%d Src:%d RgD:%d]",
           sinais[esc_reg], sinais[mem_reg], sinais[esc_mem],
           sinais[branch], sinais[jump], sinais[ula_fon], sinais[reg_des]);
}

static void imprime_estado(const typ_stt *st, int n_ins) {
    float cpi = (st->total_instrucoes > 0)
                ? (float)st->total_ciclos / st->total_instrucoes
                : 0.0f;

    printf("\n==================== ESTADO ATUAL ====================\n");
    printf("PC: %d | Ciclos: %d | Inst. Concluidas: %d | CPI: %.2f\n",
           st->pc, st->total_ciclos, st->total_instrucoes, cpi);
    printf("Tamanho do Programa: %d\n", n_ins);
    printf("------------------------------------------------------\n");

    char asm_buf[64];

    // IF
    ins_asm(asm_buf, (st->pc < 256) ? &st->instrucao_t[st->pc] : NULL);
    printf("IF  (Fetch)    : %s\n", asm_buf);

    // ID
    ins_asm(asm_buf, &st->IF_ID.instrucao);
    printf("ID  (Decode)   : %s", asm_buf);
    if (st->IF_ID.valido) print_sinais(st->ID_EX.sinal);
    printf("\n");

    // EX
    ins_asm(asm_buf, &st->ID_EX.instrucao);
    printf("EX  (Execute)  : %s", asm_buf);
    if (st->ID_EX.valido) print_sinais(st->ID_EX.sinal);
    printf("\n");

    // MEM
    ins_asm(asm_buf, &st->EX_MEM.instrucao);
    printf("MEM (Memory)   : %s", asm_buf);
    if (st->EX_MEM.valido) print_sinais(st->EX_MEM.sinal);
    printf("\n");

    // WB
    ins_asm(asm_buf, &st->MEM_WB.instrucao);
    printf("WB  (Writeback): %s", asm_buf);
    if (st->MEM_WB.valido) print_sinais(st->MEM_WB.sinal);
    printf("\n");

    printf("------------------------------------------------------\n");
    printf("REGISTRADORES:\n");
    for (int i = 0; i < 8; i++) {
        printf("$r%d: %4d (0x%02X)  ", i, st->registradores->$[i], (unsigned char)st->registradores->$[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("======================================================\n");
}

static void imprime_memorias(const typ_stt *st, int n_ins) {
    printf("\n--- MEMORIA DE INSTRUCOES ---\n");
    for (int i = 0; i < n_ins; i++) {
        char asm_buf[64];
        ins_asm(asm_buf, &st->instrucao_t[i]);
        printf("[%03d] %s  |  %s\n", i, st->instrucao_t[i].total, asm_buf);
    }

    printf("\n--- MEMORIA DE DADOS ---\n");
    for (int i = 0; i < 256; i += 4) {
        printf("[%03d]: %4d  [%03d]: %4d  [%03d]: %4d  [%03d]: %4d\n",
               i,   st->mem_dados->dados[i],
               i+1, st->mem_dados->dados[i+1],
               i+2, st->mem_dados->dados[i+2],
               i+3, st->mem_dados->dados[i+3]);
    }
}

int main(void) {
    typ_stt estado;
    memset(&estado, 0, sizeof(typ_stt));

    BancoRegistradores banco;
    inicia_registradores(&banco);
    estado.registradores = &banco;

    typ_mem_dados mem_dados;
    memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
    estado.mem_dados = &mem_dados;

    estado.capacidade_pilha = 100;
    estado.pilha_back = (print *)malloc(estado.capacidade_pilha * sizeof(print));
    estado.topo_pilha = 0;

    typ_instrucaoSep *memoria_instrucoes = NULL;
    int num_instrucoes = 0;

    char entrada[256];
    bool sair = false;

    printf("Simulador MIPS Pipeline (Versao CLI)\n");

    while (!sair) {
        imprime_estado(&estado, num_instrucoes);
        printf("\nComandos: [1] Carregar .mem, [2] Ver memorias, [SPACE/9] Avancar, [b] Voltar, [0] Reset, [q] Sair\n");
        printf("Insira o comando: ");
        
        if (!fgets(entrada, sizeof(entrada), stdin)) break;
        char ch = entrada[0];

        switch (ch) {
            case '1': {
                printf("Nome do arquivo .mem: ");
                if (fgets(entrada, sizeof(entrada), stdin)) {
                    entrada[strcspn(entrada, "\n")] = '\0';
                    if (memoria_instrucoes) free(memoria_instrucoes);
                    num_instrucoes = carregar_memoria_instrucoes(entrada, &memoria_instrucoes);
                    estado.instrucao_t = (typ_ins *)memoria_instrucoes;
                    estado.pc = 0;
                    printf("Carregadas %d instrucoes.\n", num_instrucoes);
                }
                break;
            }
            case '2':
                imprime_memorias(&estado, num_instrucoes);
                printf("\nPressione ENTER para continuar...");
                getchar();
                break;

            case '9':
            case ' ':
                push_estagio(&estado);
                executar(&estado, banco, false);
                break;

            case 'b':
            case 'B':
                if (estado.topo_pilha > 0) pop_estagio(&estado);
                else printf("Nao ha estados anteriores.\n");
                break;

            case '0':
                estado.pc = 0;
                estado.total_ciclos   = 0;
                estado.total_instrucoes = 0;
                estado.r_instrucoes   = 0;
                estado.i_instrucoes   = 0;
                estado.j_instrucoes   = 0;
                estado.nop_instrucoes = 0;
                inicia_registradores(&banco);
                memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
                memset(&estado.IF_ID,  0, sizeof(estado.IF_ID));
                memset(&estado.ID_EX,  0, sizeof(estado.ID_EX));
                memset(&estado.EX_MEM, 0, sizeof(estado.EX_MEM));
                memset(&estado.MEM_WB, 0, sizeof(estado.MEM_WB));
                estado.topo_pilha = 0;
                printf("Simulador resetado.\n");
                break;

            case 'q':
            case 'Q':
                sair = true;
                break;
            
            case '\n':
                // Se o usuário apenas apertou enter, talvez avançar?
                // Em main.c espaço avança. Aqui vou manter explícito.
                break;
                
            default:
                printf("Comando desconhecido.\n");
                break;
        }
    }

    if (memoria_instrucoes) free(memoria_instrucoes);
    if (estado.pilha_back)  free(estado.pilha_back);
    return 0;
}
