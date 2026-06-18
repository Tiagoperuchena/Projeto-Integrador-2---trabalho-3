#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "structs.h"

int carregar_memoria_instrucoes(const char *nome_arquivo, typ_instrucaoSep **mem_out)
{
    FILE *Mem_ins = fopen(nome_arquivo, "r");
    if (Mem_ins == NULL) {
        return 0;
    }

    //malloc ate 255
    typ_instrucaoSep *mem = (typ_instrucaoSep *) calloc(256 , sizeof(typ_instrucaoSep));
    if (mem == NULL) {
        fclose(Mem_ins);
        return 0;
    }

    char buffer_linha[20]; 
    int count = 0; //contador qnts inst foram lidas

    for (int k = 0; k < 256; k++) {
        if (fgets(buffer_linha, sizeof(buffer_linha), Mem_ins) == NULL) break;

        buffer_linha[strcspn(buffer_linha, "\r\n")] = '\0';
        if (strlen(buffer_linha) == 0) continue; //pular linha em branco

        uint16_t inst_bin = (uint16_t) strtol(buffer_linha, NULL, 2); // strtol(string, null, base) null p evitar o "lixo"
        mem[k].instrucao_bruta = inst_bin;

        strncpy(mem[k].total, buffer_linha, 17);
        mem[k].total[17] = '\0';

        //decodificacao bit a bit de acordo com v2
        mem[k].opcode = (inst_bin >> 12) & 0x0F; // 15-12 (4 bits)
        mem[k].rs     = (inst_bin >> 9)  & 0x07; // 11-9 (3 bits)
        mem[k].rt     = (inst_bin >> 6)  & 0x07; // 8-6 (3 bits)
        mem[k].rd     = (inst_bin >> 3)  & 0x07; // 5-3 (3 bits)
        mem[k].funct  =  inst_bin        & 0x07; // 2-0 (3 bits)
        mem[k].addr   =  inst_bin        & 0x7F; // 6-0 (7 bits) - para tipo J (J é 2, opcode 0010)

        // I-type immediate (6 bits: 5-0)
        int16_t imm6 = (int16_t)(inst_bin & 0x3F);
        if (imm6 & 0x20) imm6 |= (int16_t)0xFFC0; // Estende sinal
        mem[k].immediato = imm6;

        //classicar tipo
        if (mem[k].opcode == 0) {
            mem[k].tipo = r;
        } else if (mem[k].opcode == addi || mem[k].opcode == beq ||
                   mem[k].opcode == lw   || mem[k].opcode == sw) {
            mem[k].tipo = (enum tipos_de_instrucao)2; 
        } else if (mem[k].opcode == 2) {
            mem[k].tipo = j;
        }

        count++;
    }

    fclose(Mem_ins);
    *mem_out = mem;
    return count;
}
