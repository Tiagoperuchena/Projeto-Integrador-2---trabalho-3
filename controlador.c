#include "structs.h"
#include "funcoes.h"

typ_stt *controlador(typ_stt *state, int n)
{
    switch (state->instrucao_t[n].tipo)
    {
    case r: // Tipo R 
    state->sinal[reg_des] = 1;
    state->sinal[ula_fon] = 0;
    state->ulaop = state->instrucao_t[n].funct;
    state->sinal[mem_reg] = 0;
    state->sinal[esc_reg] = 1;
    state->sinal[jump]    = 0;
    state->sinal[esc_mem] = 0;
    state->sinal[inc_pc]  = 1;
    state->sinal[branch]  = 0;
    break;

    case i: // Tipo I
        switch (state->instrucao_t[n].opcode)
        {
        case beq:
            state->ulaop = SUB; 
            state->sinal[esc_mem] = 0;
            state->sinal[esc_reg] = 0;
            state->sinal[mem_reg] = 0;
            state->sinal[ula_fon] = 0;
            state->sinal[reg_des] = 0;
            state->sinal[inc_pc]  = 1;
            state->sinal[jump]    = 0;
            state->sinal[branch]  = 1;
           
        break;
            

        case addi:    
            state->ulaop = ADD;
            state->sinal[esc_mem] = 0;
            state->sinal[esc_reg] = 1;
            state->sinal[mem_reg] = 0;
            state->sinal[ula_fon] = 1;
            state->sinal[reg_des] = 0;
            state->sinal[inc_pc]  = 1;
            state->sinal[jump]    = 0;
            state->sinal[branch]  = 0;
        break;    


        case lw:
            // carregar do endereço de memoria(saida_ula.resultado) para o rt(state->instrucao_t[n].rt)
            state->ulaop = ADD;
            state->sinal[esc_mem] = 0;
            state->sinal[esc_reg] = 1;
            state->sinal[mem_reg] = 1;
            state->sinal[ula_fon] = 1;
            state->sinal[reg_des] = 0;
            state->sinal[inc_pc]  = 1;
            state->sinal[jump]    = 0;
            state->sinal[branch]  = 0;
        break;


        case sw:
        // salvar o rt(state->instrucao_t[n].rt) no endereço de memoria (saida_ula.resultado)
            state->ulaop = ADD;
            state->sinal[esc_mem] = 1;
            state->sinal[esc_reg] = 0;
            state->sinal[mem_reg] = 0;
            state->sinal[ula_fon] = 1;
            state->sinal[reg_des] = 0;
            state->sinal[inc_pc]  = 1;
            state->sinal[jump]    = 0;
            state->sinal[branch]  = 0;
        break;
        
        default:
            break;
        }
    break;

    case j:
        state->sinal[esc_mem] = 0;
        state->sinal[esc_reg] = 0;
        state->sinal[mem_reg] = 0;
        state->sinal[ula_fon] = 0;
        state->sinal[reg_des] = 0;
        state->sinal[inc_pc]  = 1;
        state->sinal[jump]    = 1;
        state->sinal[branch]  = 0;
    
    default:
        break;
    }

    return state;
}