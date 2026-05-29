#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "funcoes.h"

int asm_gerador_char(char *asembly, typ_ins *instrucao)
{

		if (instrucao->instrucao_bruta == 0) {
			sprintf(asembly, "add $r0, $r0, $r0\n");
			return 0;
		}
        switch ((*instrucao).tipo)
        {
            case r: // Tipo R 
                switch((*instrucao).funct)
                {
                    case ADD:
                        sprintf(asembly, "add ");
                    break;

                    case SUB:
                        sprintf(asembly, "sub ");
                    break;

                    case AND:
                        sprintf(asembly, "and ");
                    break;

                    case OR:
                        sprintf(asembly, "or ");
                    break;
                }
                sprintf(asembly + strlen(asembly), "$r%i, $r%i, $r%i\n", (*instrucao).rd, (*instrucao).rs, (*instrucao).rt);


            break;

            case i: // Tipo I
                switch ((*instrucao).opcode)
                {
                case beq:
                    sprintf(asembly, "beq $r%i, $r%i, %i\n", (*instrucao).rs, (*instrucao).rt, (*instrucao).immediato);
                break;
                    

                case addi:    
                    sprintf(asembly, "addi $r%i, $r%i, %i\n", (*instrucao).rt, (*instrucao).rs, (*instrucao).immediato);
                break;    


                case lw:
                    sprintf(asembly, "lw $r%i, %i($r%i)\n", (*instrucao).rt, (*instrucao).immediato, (*instrucao).rs);
                break;


                case sw:
                    sprintf(asembly, "sw $r%i, %i($r%i)\n", (*instrucao).rt, (*instrucao).immediato, (*instrucao).rs);
                break;
                
                default:
                    break;
                }
                
            break;

            case j:
                sprintf(asembly, "jump %i\n",(*instrucao).addr);
            break;
            default:
            break;
        }
}
