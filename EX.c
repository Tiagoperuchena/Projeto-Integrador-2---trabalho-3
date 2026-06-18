#include <stdio.h>
#include "funcoes.h"
#include "structs.h"

void estagio_EX(typ_stt *stt)
{
    if(!stt->ID_EX.valido){
        stt->EX_MEM.valido = false;
        return;
    }

    int16_t mux = 0;
    typ_ulaR saida_ula = {0};
    if (stt->ID_EX.sinal[ula_fon])
        mux = stt->ID_EX.imediato;
    else
        mux = stt->ID_EX.valor_b;

    saida_ula = ula(stt->ID_EX.valor_a, mux, stt->ID_EX.ulaop);

    if (stt->ID_EX.sinal[reg_des])
        mux = stt->ID_EX.rd;
    else
        mux = stt->ID_EX.rt;

    stt->EX_MEM.instrucao = stt->ID_EX.instrucao;
    stt->EX_MEM.valido = true;

    //===========================================================

    stt->EX_MEM.pc            = stt->ID_EX.pc;
    stt->EX_MEM.pc_branch     = (stt->ID_EX.pc + stt->ID_EX.imediato);
    stt->EX_MEM.reg_destino   = mux;
    stt->EX_MEM.resultado_ula = saida_ula.resultado;
    //propaga sinais tava faltando    
    for(int i=0; i<8; i++) stt->EX_MEM.sinal[i] = stt->ID_EX.sinal[i];

    stt->EX_MEM.valor_b       = stt->ID_EX.valor_b;
    stt->EX_MEM.zero          = saida_ula.zero;

        
}