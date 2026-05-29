#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "structs.h"

int carregar_memoria_instrucoes(typ_instrucaoSep **mem_out)
{
    char nome_arquivo[25];
    printf("Nome do arquivo .mem: ");
    if (scanf("%24s", nome_arquivo) != 1) return 0;

    FILE *Mem_ins = fopen(nome_arquivo, "r");
    if (Mem_ins == NULL) {
        printf("erro ao abrir o arquivo '%s'.\n", nome_arquivo);
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

        buffer_linha[strcspn(buffer_linha, "\n")] = '\0';
        if (strlen(buffer_linha) == 0) continue; //pular linha em branco

        uint16_t inst_bin = (uint16_t) strtol(buffer_linha, NULL, 2); // strtol(string, null, base) null p evitar o "lixo"
        mem[k].instrucao_bruta = inst_bin;

        strncpy(mem[k].total, buffer_linha, 17);
        mem[k].total[17] = '\0';

        //decodificacao bit a bit de acordo com v2
        mem[k].opcode = (inst_bin >> 12) & 0x0F; // 0000 1111/ 15
        mem[k].rs     = (inst_bin >> 9)  & 0x07; // 00000 111/ 7
        mem[k].rt     = (inst_bin >> 6)  & 0x07; // 00000 111/ 7
        mem[k].rd     = (inst_bin >> 3)  & 0x07;
        mem[k].funct  =  inst_bin        & 0x07;
        mem[k].addr   =  inst_bin        & 0x7F; 

    
        int16_t imm6 = (int16_t)(inst_bin & 0x3F);//111111 6 bits
        if (imm6 & 0x20) imm6 |= (int16_t)0xFFC0; // estende sinal 0x20 = 100000(6 bit -) ou 0xFFC0(1111 1111 1100 0000)
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

        char asm_buffer[30];
        if (mem[k].instrucao_bruta == 0) {
            sprintf(asm_buffer, "add $r0, $r0, $r0");
            printf("instrucao [%3d]: %s | Asm: %-20s | campos -> (Vazio)\n", 
                   k, mem[k].total, asm_buffer);
            count++;
            continue; 
        }
        switch (mem[k].tipo)
        {
            case r: // Tipo R
                switch(mem[k].funct)
                {
                    case ADD: sprintf(asm_buffer, "add $r%d, $r%d, $r%d", mem[k].rd, mem[k].rs, mem[k].rt); break;
                    case SUB: sprintf(asm_buffer, "sub $r%d, $r%d, $r%d", mem[k].rd, mem[k].rs, mem[k].rt); break;
                    case AND: sprintf(asm_buffer, "and $r%d, $r%d, $r%d", mem[k].rd, mem[k].rs, mem[k].rt); break;
                    case OR:  sprintf(asm_buffer, "or $r%d, $r%d, $r%d",  mem[k].rd, mem[k].rs, mem[k].rt); break;
                    default:  sprintf(asm_buffer, "??? $r%d, $r%d, $r%d", mem[k].rd, mem[k].rs, mem[k].rt); break;
                }
                printf("instrucao [%3d]: %s | Asm: %-20s | campos -> Op:%2d  rs:%d  rt:%d  rd:%d  funct:%d\n", 
                       k, mem[k].total, asm_buffer, mem[k].opcode, mem[k].rs, mem[k].rt, mem[k].rd, mem[k].funct);
                break;

            case i: // Tipo I
                if (mem[k].opcode == lw) {
                    sprintf(asm_buffer, "lw $r%d, %d($r%d)", mem[k].rt, mem[k].immediato, mem[k].rs);
                } 
                else if (mem[k].opcode == sw) {
                    sprintf(asm_buffer, "sw $r%d, %d($r%d)", mem[k].rt, mem[k].immediato, mem[k].rs);
                } 
                else if (mem[k].opcode == beq) {
                    sprintf(asm_buffer, "beq $r%d, $r%d, %d", mem[k].rs, mem[k].rt, mem[k].immediato);
                } 
                else if (mem[k].opcode == addi) {
                    sprintf(asm_buffer, "addi $r%d, $r%d, %d", mem[k].rt, mem[k].rs, mem[k].immediato);
                } 
                else {
                    sprintf(asm_buffer, "??? $r%d, $r%d, %d", mem[k].rs, mem[k].rt, mem[k].immediato);
                }

                printf("instrucao [%3d]: %s | Asm: %-20s | campos -> Op:%2d  rs:%d  rt:%d  imm:%-4d\n", 
                       k, mem[k].total, asm_buffer, mem[k].opcode, mem[k].rs, mem[k].rt, mem[k].immediato);
                break;

            case j: // Tipo J
                sprintf(asm_buffer, "jump %d", mem[k].addr);
                printf("instrucao [%3d]: %s | Asm: %-20s | campos -> Op:%2d  addr:%d\n", 
                       k, mem[k].total, asm_buffer, mem[k].opcode, mem[k].addr);
                break;

            default:
                sprintf(asm_buffer, "Desconhecido");
                printf("instrucao [%3d]: %s | Asm: %-20s | campos -> Op:%2d\n", 
                       k, mem[k].total, asm_buffer, mem[k].opcode);
                break;
        }

        count++;
    }

    fclose(Mem_ins);
    *mem_out = mem;
    return count;
}
