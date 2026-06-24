#include <stdio.h>
#include "structs.h"

void estagio_IF(typ_stt *estado)
{
    if (estado == NULL || estado->instrucao_t == NULL)
    {
        return;
    }

    //busca na mem instrucoes
    estado->IF_ID.instrucao =
        estado->instrucao_t[estado->pc];

    estado->IF_ID.pc = estado->pc;
    estado->IF_ID.valido = true;

    // prox pc
    estado->pc++;
}
