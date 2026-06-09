#include <stdio.h>
#include "structs.h"

void estagio_IF(typ_stt *estado)
{
    if (estado == NULL || estado->instrucao_t == NULL)
    {
        return;
    }

    // Busca instrução da memória de instruções
    estado->IF_ID.instrucao =
        estado->instrucao_t[estado->pc];

    // Guarda o PC atual
    estado->IF_ID.pc = estado->pc;
    estado->IF_ID.valido = true;

    // Próxima instrução
    estado->pc++;
}
