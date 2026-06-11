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

    if (stt->ID_EX.sinal[reg_des])
        mux = stt->ID_EX.rd;
    else
        mux = stt->ID_EX.rt;

    //===========================================================

    stt->EX_MEM.pc            = stt->ID_EX.pc;
    stt->EX_MEM.pc_branch     = (stt->ID_EX.pc + stt->ID_EX.imediato);
    stt->EX_MEM.reg_destino   = mux;
    stt->EX_MEM.resultado_ula = saida_ula.resultado;
    stt->EX_MEM.sinal[0]      = stt->ID_EX.sinal[0];
    stt->EX_MEM.sinal[1]      = stt->ID_EX.sinal[1];
    stt->EX_MEM.sinal[2]      = stt->ID_EX.sinal[2];
    stt->EX_MEM.sinal[3]      = stt->ID_EX.sinal[3];
    stt->EX_MEM.sinal[4]      = stt->ID_EX.sinal[4];
    stt->EX_MEM.sinal[5]      = stt->ID_EX.sinal[5];
    stt->EX_MEM.sinal[6]      = stt->ID_EX.sinal[6];
    stt->EX_MEM.sinal[7]      = stt->ID_EX.sinal[7];
    stt->EX_MEM.valor_b       = stt->ID_EX.valor_b;
    stt->EX_MEM.zero          = saida_ula.zero;

        
}

void control(typ_stt *stt)
{
    switch (stt->IF_ID.instrucao.tipo)
    {
    case r: // Tipo R 
    stt->sinal[reg_des] = 1;
    stt->ulaop = stt->IF_ID.instrucao.funct;
    stt->sinal[ula_fon] = 0;

    stt->sinal[esc_mem] = 0;
    stt->sinal[branch]  = 0; // DVC  n sei pq mudou de nome
    stt->sinal[jump]    = 0; // DVI  tbm n sei
    
    stt->sinal[mem_reg] = 0;
    stt->sinal[esc_reg] = 1;


    stt->sinal[inc_pc]  = 1; 
    break;

    case i: // Tipo I
        switch (stt->IF_ID.instrucao.opcode)
        {
        case beq:
            stt->ulaop = SUB; 
            stt->sinal[esc_mem] = 0;
            stt->sinal[esc_reg] = 0;
            stt->sinal[mem_reg] = 0;
            stt->sinal[ula_fon] = 0;
            stt->sinal[reg_des] = 0;
            stt->sinal[inc_pc]  = 1;
            stt->sinal[jump]    = 0;
            stt->sinal[branch]  = 1;
           
        break;
            

        case addi:    
            stt->ulaop = ADD;
            stt->sinal[esc_mem] = 0;
            stt->sinal[esc_reg] = 1;
            stt->sinal[mem_reg] = 0;
            stt->sinal[ula_fon] = 1;
            stt->sinal[reg_des] = 0;
            stt->sinal[inc_pc]  = 1;
            stt->sinal[jump]    = 0;
            stt->sinal[branch]  = 0;
        break;    


        case lw:
            stt->ulaop = ADD;
            stt->sinal[esc_mem] = 0;
            stt->sinal[esc_reg] = 1;
            stt->sinal[mem_reg] = 1;
            stt->sinal[ula_fon] = 1;
            stt->sinal[reg_des] = 0;
            stt->sinal[inc_pc]  = 1;
            stt->sinal[jump]    = 0;
            stt->sinal[branch]  = 0;
        break;


        case sw:
            stt->ulaop = ADD;
            stt->sinal[esc_mem] = 1;
            stt->sinal[esc_reg] = 0;
            stt->sinal[mem_reg] = 0;
            stt->sinal[ula_fon] = 1;
            stt->sinal[reg_des] = 0;
            stt->sinal[inc_pc]  = 1;
            stt->sinal[jump]    = 0;
            stt->sinal[branch]  = 0;
        break;
        
        default:
            break;
        }
    break;

    case j:
        stt->sinal[esc_mem] = 0;
        stt->sinal[esc_reg] = 0;
        stt->sinal[mem_reg] = 0;
        stt->sinal[ula_fon] = 0;
        stt->sinal[reg_des] = 0;
        stt->sinal[inc_pc]  = 1;
        stt->sinal[jump]    = 1;
        stt->sinal[branch]  = 0;
    
    default:
        break;
    }
}