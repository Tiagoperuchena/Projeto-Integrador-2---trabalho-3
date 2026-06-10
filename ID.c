#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "funcoes.h"

void estagio_ID(typ_stt *estado)
{
    if (estado == NULL) return;

    //pega instrucao do if_id
    typ_ins ins = estado->IF_ID.instrucao;

    bool sinais_backup[8];
    memcpy(sinais_backup, estado->sinal, sizeof(bool) * 8);
    typ_ulaOp ulaop_backup = estado->ulaop;

    controlador(estado, estado->IF_ID.pc);

    memcpy(estado->ID_EX.sinal, estado->sinal, sizeof(bool) * 8);
    estado->ID_EX.ulaop = estado->ulaop;
    estado->ID_EX.pc = estado->IF_ID.pc;
    
    estado->ID_EX.valor_a = estado->registradores->$[ins.rs];
    estado->ID_EX.valor_b = estado->registradores->$[ins.rt];
    
    estado->ID_EX.imediato = ins.immediato;
    estado->ID_EX.rt = ins.rt;
    estado->ID_EX.rd = ins.rd;
    estado->ID_EX.addr = ins.addr;

    memcpy(estado->sinal, sinais_backup, sizeof(bool) * 8);
    estado->ulaop = ulaop_backup;
}
