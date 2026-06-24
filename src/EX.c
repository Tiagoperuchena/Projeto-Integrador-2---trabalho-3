#include <stdio.h>
#include "funcoes.h"
#include "structs.h"
#include <string.h>

void estagio_EX(typ_stt *stt)
{
  
    if(!stt->ID_EX.valido){
        stt->EX_MEM.valido = false;
        stt->EX_MEM.instrucao.tipo = bolha;
        stt->EX_MEM.instrucao.instrucao_bruta = 0;
        stt->EX_MEM.zero = false;
        stt->EX_MEM.resultado_ula = 0;
        memset(stt->EX_MEM.sinal, 0, sizeof(stt->EX_MEM.sinal));
        return;
    }

    int valor_a = stt->ID_EX.valor_a;
    int valor_b = stt->ID_EX.valor_b;
    int16_t mux_ula_b = 0; 

    stt->ID_EX.fwd_a = 0;
    stt->ID_EX.fwd_b = 0;

    if (stt->EX_MEM.valido && stt->EX_MEM.sinal[esc_reg] && stt->EX_MEM.reg_destino != 0 && stt->EX_MEM.reg_destino == stt->ID_EX.instrucao.rs) {
        valor_a = stt->EX_MEM.resultado_ula;
        stt->ID_EX.fwd_a = 1; 
        stt->total_forwardings++;
    }
    else if (stt->MEM_WB.valido && stt->MEM_WB.sinal[esc_reg] && stt->MEM_WB.reg_destino != 0 && stt->MEM_WB.reg_destino == stt->ID_EX.instrucao.rs) {
        valor_a = (stt->MEM_WB.sinal[mem_reg] == 0) ? stt->MEM_WB.saida_mem : stt->MEM_WB.resultado_ula;
        stt->ID_EX.fwd_a = 2; 
        stt->total_forwardings++;
    }

    
    if (stt->EX_MEM.valido && stt->EX_MEM.sinal[esc_reg] && stt->EX_MEM.reg_destino != 0 && stt->EX_MEM.reg_destino == stt->ID_EX.instrucao.rt) {
        valor_b = stt->EX_MEM.resultado_ula;
        stt->ID_EX.fwd_b = 1; 
        stt->total_forwardings++;
    }
    else if (stt->MEM_WB.valido && stt->MEM_WB.sinal[esc_reg] && stt->MEM_WB.reg_destino != 0 && stt->MEM_WB.reg_destino == stt->ID_EX.instrucao.rt) {
        valor_b = (stt->MEM_WB.sinal[mem_reg] == 0) ? stt->MEM_WB.saida_mem : stt->MEM_WB.resultado_ula;
        stt->ID_EX.fwd_b = 2; 
        

    }

    if (stt->ID_EX.sinal[ula_fon])
        mux_ula_b = stt->ID_EX.imediato;
    else
        mux_ula_b = valor_b;

    typ_ulaR saida_ula = ula(valor_a, mux_ula_b, stt->ID_EX.ulaop);

    uint16_t reg_destino_final = 0;
    if (stt->ID_EX.sinal[reg_des])
        reg_destino_final = stt->ID_EX.rd;
    else
        reg_destino_final = stt->ID_EX.rt;

    stt->EX_MEM.instrucao = stt->ID_EX.instrucao;
    stt->EX_MEM.valido = true;

    stt->EX_MEM.pc            = stt->ID_EX.pc;
    stt->EX_MEM.pc_branch     = (stt->ID_EX.pc + stt->ID_EX.imediato);
    stt->EX_MEM.reg_destino   = reg_destino_final;
    stt->EX_MEM.resultado_ula = saida_ula.resultado;
    stt->EX_MEM.zero          = saida_ula.zero;
    stt->EX_MEM.valor_b       = valor_b;

    for (int i = 0; i < 8; i++) {
        stt->EX_MEM.sinal[i] = stt->ID_EX.sinal[i];
    }
}