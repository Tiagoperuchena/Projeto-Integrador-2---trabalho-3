#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "funcoes.h"

void push_estagio(typ_stt *state) {
    if (state->topo_pilha >= state->capacidade_pilha) {
        state->capacidade_pilha *= 2;
        state->pilha_back = (print *)realloc(state->pilha_back, state->capacidade_pilha * sizeof(print));
    }
    
    print *p = &state->pilha_back[state->topo_pilha];
    p->pc = state->pc;
    p->banco_reg = *state->registradores;
    p->mem_dados = *state->mem_dados;
    p->IF_ID = state->IF_ID;
    p->ID_EX = state->ID_EX;
    p->EX_MEM = state->EX_MEM;
    p->MEM_WB = state->MEM_WB;
    
    state->topo_pilha++;
}

void pop_estagio(typ_stt *state) {
    if (state->topo_pilha > 0) {
        state->topo_pilha--;
        print *p = &state->pilha_back[state->topo_pilha];
        
        state->pc = p->pc;
        *state->registradores = p->banco_reg;
        *state->mem_dados = p->mem_dados;
        state->IF_ID = p->IF_ID;
        state->ID_EX = p->ID_EX;
        state->EX_MEM = p->EX_MEM;
        state->MEM_WB = p->MEM_WB;
    }
}
