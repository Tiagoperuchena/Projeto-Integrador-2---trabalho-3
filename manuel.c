#include "manuel.h"
#include "funcoes.h"
#include "struct.h"

int main()
{

}

void EX(typ_stt *stt)
{
    int16_t mux = 0;
    typ_ulaR saida_ula = {0};
    if (stt->ID_EX.sinal[ula_fon])
        mux = stt->instrucao_t->immediato;
    else
        mux = stt->ID_EX.valor_b;

    saida_ula = ula(stt->ID_EX.valor_a, mux, stt->ID_EX.ulaop);

    //===========================================================

    stt->EX_MEM.pc            = stt->ID_EX.pc;
    stt->EX_MEM.pc_branch     = (stt->ID_EX.pc + stt->ID_EX.imediato);
    //stt->EX_MEM.reg_destino   = stt->ID_EX.;
    stt->EX_MEM.resultado_ula = saida_ula.resultado;
    stt->EX_MEM.sinal[0]         = stt->ID_EX.sinal[0];
    stt->EX_MEM.sinal[1]         = stt->ID_EX.sinal[1];
    stt->EX_MEM.sinal[2]         = stt->ID_EX.sinal[2];
    stt->EX_MEM.sinal[3]         = stt->ID_EX.sinal[3];
    stt->EX_MEM.sinal[4]         = stt->ID_EX.sinal[4];
    stt->EX_MEM.sinal[5]         = stt->ID_EX.sinal[5];
    stt->EX_MEM.sinal[6]         = stt->ID_EX.sinal[6];
    stt->EX_MEM.sinal[7]         = stt->ID_EX.sinal[7];
    stt->EX_MEM.valor_b       = stt->ID_EX.valor_b;
    stt->EX_MEM.zero          = saida_ula.zero;

        
}

void *control()
{
    
}