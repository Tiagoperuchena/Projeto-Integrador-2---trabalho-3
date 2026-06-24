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
    p->total_ciclos     = state->total_ciclos;
    p->total_instrucoes = state->total_instrucoes;
    p->r_instrucoes     = state->r_instrucoes;
    p->i_instrucoes     = state->i_instrucoes;
    p->j_instrucoes     = state->j_instrucoes;
    p->nop_instrucoes   = state->nop_instrucoes;
    p->total_bolhas     = state->total_bolhas;
    p->total_forwardings = state->total_forwardings;
    
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
        state->total_ciclos     = p->total_ciclos;
        state->total_instrucoes = p->total_instrucoes;
        state->r_instrucoes     = p->r_instrucoes;
        state->i_instrucoes     = p->i_instrucoes;
        state->j_instrucoes     = p->j_instrucoes;
        state->nop_instrucoes   = p->nop_instrucoes;
        state->total_bolhas     = p->total_bolhas;
        state->total_forwardings = p->total_forwardings;
    }
}
