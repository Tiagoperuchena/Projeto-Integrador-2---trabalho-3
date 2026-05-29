#include <stdio.h>
#include "funcoes.h"
#include "structs.h"

int executar(typ_stt *status, typ_reg reg, bool clear_data)
{
    int val_escrita;
    int valor_b;
    int valor_a;

    if (status == NULL || status->instrucao_t == NULL || status->registradores == NULL || status->mem_dados == NULL) {
        fprintf(stderr, "errro\n");
        return -1;
    }

    if (status->pc < 0 || status->pc >= 256) {
        fprintf(stderr, "erro!\n");
        return -1;
    }//ver se o pc tem um valor valido

    status->total_instrucoes++;

    if (status->instrucao_t[status->pc].instrucao_bruta == 0)
    {
        status->nop_instrucoes++;
    }else{
            switch (status->instrucao_t[status->pc].tipo) {
            case r: status->r_instrucoes++; break;
            case i: status->i_instrucoes++; break;
            case j: status->j_instrucoes++; break;
            default: break;
        }
    }

    //controle
    controlador(status, status->pc);

    //le o primeiro operando (rs)
    valor_a = status->registradores->$[status->instrucao_t[status->pc].rs];
    
    //mux rt ou imm para o segundo(ula_fon)
    if (status->sinal[ula_fon] == 0)  
    {

        valor_b = status->registradores->$[status->instrucao_t[status->pc].rt];//rt

    } else
    {
        
        valor_b = status->instrucao_t[status->pc].immediato;//imm
    }                                                                             

    //ula
    status->ular = ula(valor_a, valor_b, status->ulaop);

    //acesso a memoria
    unsigned int endereco_mem = (uint8_t)status->ular.resultado; 
    //esc_mem | mem_reg
    int8_t saida_mem = mem_data(status->mem_dados,
                               endereco_mem,
                               status->registradores->$[status->instrucao_t[status->pc].rt],
                               status->sinal[esc_mem],
                               status->sinal[mem_reg],
                               clear_data);
    
    //esc_reg
    if (status->sinal[esc_reg]) {
        if (status->sinal[mem_reg]) 
        {
            val_escrita = saida_mem;
        } else 
        {
            val_escrita = status->ular.resultado;
        }

        if (status->sinal[reg_des]) 
        {
            status->registradores->$[status->instrucao_t[status->pc].rd] = (int8_t)val_escrita;
        } else 
        {
            status->registradores->$[status->instrucao_t[status->pc].rt] = (int8_t)val_escrita;
        }
    }

    //incremento do pc so no final e usando como o base o pc atual
    int pc_antigo = status->pc;
    status->estouro = false;
    if (status->sinal[inc_pc])
    {
        if (status->sinal[jump])
        {
           
            status->pc = status->instrucao_t[pc_antigo].addr;
        }
        else if (status->sinal[branch] && status->ular.zero)
        {
            int destino = pc_antigo + 1 + status->instrucao_t[pc_antigo].immediato;
            status->pc = (unsigned char)destino; 
        }
        else
        {
            if (pc_antigo == 255){
                status->pc     = 0;
                status->estouro = true; // fim do espaco de instrucoes
            }
            else
            {
                status->pc = pc_antigo + 1;
            }
        }
    }
    return 0;
}