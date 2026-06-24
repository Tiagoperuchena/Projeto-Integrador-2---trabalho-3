#include <stdio.h>
#include "structs.h"
#include "funcoes.h"
#include <string.h>

void verificar_hazards(typ_stt *status) {
    status->dect_stall = false;

    if (status->ID_EX.valido && status->ID_EX.instrucao.opcode == lw && status->ID_EX.rt != 0) { 
        uint16_t lw_dest = status->ID_EX.rt;

        if (status->IF_ID.valido && status->IF_ID.instrucao.tipo != bolha) {
            uint16_t prox_rs = status->IF_ID.instrucao.rs;
            uint16_t prox_rt = status->IF_ID.instrucao.rt;

            
            bool le_rs = (status->IF_ID.instrucao.tipo != j); 

           
            bool le_rt = (status->IF_ID.instrucao.tipo == r || 
                          status->IF_ID.instrucao.opcode == beq || 
                          status->IF_ID.instrucao.opcode == sw);

           
            if ((le_rs && lw_dest == prox_rs) || (le_rt && lw_dest == prox_rt)) {
                status->dect_stall = true;
            }
        }
    }
}

int executar(typ_stt *status, typ_reg reg, bool clear_data)
{
    if (status == NULL || status->instrucao_t == NULL || 
        status->registradores == NULL || status->mem_dados == NULL) {
        fprintf(stderr, "erro\n");
        return -1;
    }

    verificar_hazards(status);

    
    typ_ex_mem old_EX_MEM = status->EX_MEM;
    typ_mem_wb old_MEM_WB = status->MEM_WB;

   
    estagio_WB(status);   

    
    estagio_EX(status);   
    typ_ex_mem new_EX_MEM = status->EX_MEM; 

    status->EX_MEM = old_EX_MEM;
    estagio_MEM(status); 

    status->EX_MEM = new_EX_MEM;

    
    if (status->dect_stall) {
    
        status->total_bolhas++;
        memset(status->ID_EX.sinal, 0, sizeof(status->ID_EX.sinal));
        status->ID_EX.valido = false;
        status->ID_EX.instrucao.tipo = bolha;
        status->ID_EX.instrucao.instrucao_bruta = 0;
        strcpy(status->ID_EX.instrucao.total, "STALL (Load-Use)");
        
    } else {
       
        estagio_ID(status);
        estagio_IF(status);
    }

    if (status->EX_MEM.sinal[jump] && status->EX_MEM.valido) {
        status->total_bolhas++;
        status->pc = status->EX_MEM.addr;
        status->IF_ID.valido = false;
        status->ID_EX.valido = false;
        status->IF_ID.instrucao.instrucao_bruta = 0;
        status->ID_EX.instrucao.instrucao_bruta = 0;
        status->IF_ID.instrucao.tipo = bolha;
        status->ID_EX.instrucao.tipo = bolha;
    } else if (status->EX_MEM.sinal[branch] && status->EX_MEM.zero && status->EX_MEM.valido) {
        status->total_bolhas++;
        status->pc = status->EX_MEM.pc_branch;
        status->IF_ID.valido = false;
        status->ID_EX.valido = false;
        status->IF_ID.instrucao.instrucao_bruta = 0;
        status->ID_EX.instrucao.instrucao_bruta = 0;
        status->IF_ID.instrucao.tipo = bolha;
        status->ID_EX.instrucao.tipo = bolha;
    }

    status->total_ciclos++;
    return 0;
}