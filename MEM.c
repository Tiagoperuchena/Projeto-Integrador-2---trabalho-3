#include <stdio.h>
#include "structs.h"
#include "funcoes.h"

void estagio_MEM(typ_stt *estado)
{
    if (estado == NULL) return;

    // EX_MEM --> MEM_WB
    for (int i = 0; i < 8; i++) {
        estado->MEM_WB.sinal[i] = estado->EX_MEM.sinal[i];
    }

    estado->MEM_WB.pc = estado->EX_MEM.pc;
    estado->MEM_WB.resultado_ula = estado->EX_MEM.resultado_ula;
    estado->MEM_WB.reg_destino = estado->EX_MEM.reg_destino;
    estado->MEM_WB.valido = estado->EX_MEM.valido;
    estado->MEM_WB.instrucao = estado->instrucao_t[estado->EX_MEM.pc];
    //acesso mem
    estado->MEM_WB.saida_mem = mem_data(
        estado->mem_dados,(unsigned int)estado->EX_MEM.resultado_ula,estado->EX_MEM.valor_b,estado->EX_MEM.sinal[esc_mem],estado->EX_MEM.sinal[mem_reg],false );

    //(Branch/Jump)
    if (estado->EX_MEM.sinal[jump]) {
        // tratar no executar dps
    }
}
