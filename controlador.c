#include "structs.h"
#include "funcoes.h"

typ_stt *control(typ_stt *stt, int n)
{

    stt->sinal[inc_pc]  = 1;

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


    
    break;

    case i: // Tipo I
        switch (stt->IF_ID.instrucao.opcode)
        {
        case beq:
            stt->sinal[reg_des] = 0;
            stt->ulaop = SUB; 
            stt->sinal[ula_fon] = 0;

            stt->sinal[branch]  = 1;
            stt->sinal[jump]    = 0;
            stt->sinal[esc_mem] = 0;
            
            stt->sinal[esc_reg] = 0;
            stt->sinal[mem_reg] = 0;
           
        break;
            

        case addi:    
            stt->sinal[reg_des] = 0;
            stt->ulaop = ADD;
            stt->sinal[ula_fon] = 1;

            stt->sinal[branch]  = 0;
            stt->sinal[jump]    = 0;
            stt->sinal[esc_mem] = 0;
            
            stt->sinal[esc_reg] = 1;
            stt->sinal[mem_reg] = 0;
        break;    


        case lw:
            stt->sinal[reg_des] = 0;
            stt->ulaop = ADD;
            stt->sinal[ula_fon] = 1;

            stt->sinal[esc_mem] = 0;
            stt->sinal[branch]  = 0;
            stt->sinal[jump]    = 0;
            
            stt->sinal[esc_reg] = 1;
            stt->sinal[mem_reg] = 1;
        break;


        case sw:
            stt->sinal[reg_des] = 0;
            stt->ulaop = ADD;
            stt->sinal[ula_fon] = 1;

            stt->sinal[branch]  = 0;
            stt->sinal[jump]    = 0;
            stt->sinal[esc_mem] = 1;
        
            stt->sinal[esc_reg] = 0;
            stt->sinal[mem_reg] = 0;
        break;
        
        default:
            break;
        }
    break;

    case j:
        stt->sinal[reg_des] = 0;
        stt->sinal[ula_fon] = 0;

        stt->sinal[branch]  = 0;
        stt->sinal[jump]    = 1;
        stt->sinal[esc_mem] = 0;
        
        stt->sinal[esc_reg] = 0;
        stt->sinal[mem_reg] = 0;
    default:
        break;
    }

    return stt;
}