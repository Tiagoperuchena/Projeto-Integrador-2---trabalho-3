#include <stdio.h>
#include "structs.h"
#include "funcoes.h"

void estagio_WB(typ_stt *estado)
{
    int valor_escrita;

    if (estado == NULL || estado->registradores == NULL)
        return;

    // Verifica se deve escrever em registrador
    if (estado->MEM_WB.sinal[esc_reg])
    {
        // MUX MemToReg
        if (estado->MEM_WB.sinal[mem_reg])
        {
            valor_escrita = estado->MEM_WB.saida_mem;
        }
        else
        {
            valor_escrita = estado->MEM_WB.resultado_ula;
        }

        escreve_registrador(
            estado->registradores,
            estado->MEM_WB.reg_destino,
            valor_escrita
        );
    }
}
