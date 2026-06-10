#include <stdio.h>
#include "structs.h"
#include "funcoes.h"

//so a ideia inicial, precisa ser revisado :)
int executar(typ_stt *status, typ_reg reg, bool clear_data)
{
    if (status == NULL || status->instrucao_t == NULL || status->registradores == NULL || status->mem_dados == NULL) {
        fprintf(stderr, "erro\n");
        return -1;
    }

    estagio_WB(status);
    estagio_MEM(status);
    estagio_EX(status);
    estagio_ID(status);
    estagio_IF(status);

    status->total_ciclos++;

    if(status->EX_MEM.sinal[jump] && status->EX_MEM.valido){
        status->pc = status->EX_MEM.addr;
        status->IF_ID.valido = false; 
        status->ID_EX.valido = false;
        status->IF_ID.instrucao.instrucao_bruta = 0;
        status->ID_EX.instrucao.instrucao_bruta = 0;

    }else if(status->EX_MEM.sinal[branch] && status->EX_MEM.zero && status->EX_MEM.valido){
        status->pc = status->EX_MEM.pc_branch;
        status->IF_ID.valido = false;
        status->ID_EX.valido = false;
        status->IF_ID.instrucao.instrucao_bruta = 0;
        status->ID_EX.instrucao.instrucao_bruta = 0;

    }

    return 0;
}