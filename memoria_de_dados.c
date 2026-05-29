#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "structs.h"

int8_t mem_data(typ_mdd *mem, unsigned int endereco, int8_t D, bool str, bool ld, bool clear)
{
    if (mem == NULL) 
    {
        printf("mem_dados null");
        return 0;
    }

    if (clear) 
    {
        memset(mem->dados, 0, sizeof(mem->dados));
    }

    if (endereco >= 256) 
    {
        endereco &= 0xFF; //1111 1111 e cai na pos 0
    }

    if (str) 
    {
        mem->dados[endereco] = D;
    }

    if (ld) 
    {
        return mem->dados[endereco];
    }

    return D;
}