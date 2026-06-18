#include <stdio.h>
#include "structs.h"
#include "funcoes.h"

void estagio_WB(typ_stt *estado)
{
    int valor_escrita;

    if (estado == NULL || estado->registradores == NULL)
        return;



     if (estado->MEM_WB.valido)
    {
        estado->total_instrucoes++;
        typ_ins ins = estado->MEM_WB.instrucao;
        if (ins.instrucao_bruta == 0) {
            estado->nop_instrucoes++;
            estado->r_instrucoes++; 
        } else {
            switch (ins.tipo) {
                case r: estado->r_instrucoes++; break;
                case i: estado->i_instrucoes++; break;
                case j: estado->j_instrucoes++; break;
                default: break;
            }
        }
    }

    // Verifica se deve escrever no registrador
    if (estado->MEM_WB.sinal[esc_reg] && estado->MEM_WB.valido)
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
