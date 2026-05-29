#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "funcoes.h"

void limpa_buff();

int main()
{
    int menu          = 0;
    int num_instrucoes = 0;

   //incializar
    typ_stt estado;
    memset(&estado, 0, sizeof(typ_stt));
    estado.pc        = 0;

    BancoRegistradores banco;
    inicia_registradores(&banco);
    estado.registradores = &banco;

   //zerar
    typ_mem_dados mem_dados;
    memset(mem_dados.dados, 0, sizeof(mem_dados.dados)); // zerar
    estado.mem_dados = &mem_dados;

    // inicializar backup do backstep
    estado.topo_pilha=0;
    estado.total_instrucoes = 0;
    estado.r_instrucoes = 0;
    estado.i_instrucoes = 0;
    estado.j_instrucoes = 0;
    estado.nop_instrucoes = 0;
    estado.estouro = false;

    typ_instrucaoSep *memoria_instrucoes = NULL;

    do
    {
        //5,6,7 imprimir simulador, .asm, .dat
        printf("\n   -=|| MIPS UNIPAMPA ||=-   \n");
        printf(" PC: %d | Instrucoes Carregadas: %d\n", estado.pc, num_instrucoes);
        printf("+------+----------------------------------------------------+\n");
        printf("| [1]  | Carregar memoria de instrucoes (.mem)              |\n");
        printf("| [2]  | Carregar memoria de dados (.dat)                   |\n"); //implementado
        printf("| [3]  | Imprimir memorias (instrucoes e dados)             |\n");
        printf("| [4]  | Imprimir banco de registradores                    |\n");
        printf("| [5]  | Imprimir estado completo (memoria + registradores) |\n");
        printf("| [6]  | Salvar binario                                     |\n"); //asm
        printf("| [7]  | Salvar .dat                                        |\n"); //salvando o .dat
        printf("| [8]  | Executar Programa (Run)                            |\n"); // executa tudo em loop
        printf("| [9]  | Executar uma instrucao (Step)                      |\n"); 
        printf("| [10] | Voltar uma instrucao (Back)                        |\n"); //precisa refatorar, ela resgata somente o pc, mas n resgata os dados da mem e dos registradores (!!!!!)
        printf("| [11] | Gerar .asm                                         |\n");
        printf("| [12] | Gerar estatisticas                                 |\n");
        printf("| [0]  | Reset                                              |\n");
        printf("| [123]| Sair                                               |\n");
        printf("+------+----------------------------------------------------+\n");
        printf("Entrada: ");

        if (scanf("%i", &menu) != 1) {
            limpa_buff();
            continue;
        }
        limpa_buff();

        switch (menu)
        {
        case 1:
            if (memoria_instrucoes != NULL) free(memoria_instrucoes);
            num_instrucoes = carregar_memoria_instrucoes(&memoria_instrucoes);
            estado.instrucao_t = (typ_ins*)memoria_instrucoes;
            estado.pc          = 0;
            estado.sinal[inc_pc] = 0;
            break;

        case 2: {
            char nome_dat[25];
            printf("Nome do arquivo .dat: ");
            if (scanf("%24s", nome_dat) != 1) { limpa_buff(); break; }
            limpa_buff();

            FILE *f = fopen(nome_dat, "r");
            if (f == NULL) {
                printf("Erro ao abrir '%s'.\n", nome_dat);
                break;
            }
            memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
            int val, idx = 0;
            while (idx < 256 && fscanf(f, "%d", &val) == 1)
                mem_dados.dados[idx++] = (int8_t)val;
            fclose(f);
            printf("memoria de dados carregada (%d posicoes).\n", idx);
            break;
        }

        case 3:
    
            printf("\nmemoria de instrucoes\n");
            for (int i = 0; i < 255; i++) {
                printf("[%03d] %s",i,memoria_instrucoes[i].total);
                if (memoria_instrucoes[i].total[0] == 0) 
                    printf("0000000000000000");
                printf(" | Op: %d | Funct: %d\n", 
                    memoria_instrucoes[i].opcode,
                    memoria_instrucoes[i].funct);
            }

            printf("\nmem de dados \n");
                    
            for (int i = 0; i < 256; i++) 
                printf("[%03d] = %d\n", i, mem_dados.dados[i]);
            
        break;

        case 4:
            imprime_registradores(&banco);
            break;

        case 5:
            printf("\n  ESTADO COMPLETO \n");
            imprime_registradores(&banco);
            printf("\nmem de dados:\n");
            for (int i = 0; i < 256; i++) 
                printf("[%03d] = %d\n", i, mem_dados.dados[i]);

            printf("\n");
            break;

        case 6: {
            char buffer[25];
            printf("Nome do arquivo de saida: ");
            if (scanf("%24s", buffer) == 1) {
                FILE *f = fopen(buffer, "w");
                if (f) {
                    for (int i = 0; i < num_instrucoes; i++) {
                        fprintf(f, "%s\n", memoria_instrucoes[i].total);
                    }
                    fclose(f);
                    printf("Arquivo salvo: %s\n", buffer);
                } else printf("Erro ao criar %s\n", buffer);
            }
            limpa_buff();
            break;
        }

        case 7: {
            char nome_dat[25];
            printf("Nome do arquivo de saida .dat: ");
            if (scanf("%24s", nome_dat) == 1) {
                FILE *f = fopen(nome_dat, "w");
                if (f) {
                    for (int i = 0; i < 256; i++) {
                        fprintf(f, "%d\n", mem_dados.dados[i]);
                    }
                    fclose(f);
                    printf("Arquivo .dat salvo: %s\n", nome_dat);
                } else printf("Erro ao criar %s\n", nome_dat);
            }
            limpa_buff();
            break;
        }

        case 8: //run
            if (memoria_instrucoes == NULL) {
                printf("erro\n");
                break;
            }
            estado.estouro = false;
            printf("executando\n");
            do
            {
                executar(&estado, banco, false);
            } while (estado.pc != 255);
            imprime_registradores(&banco);
            printf("finalizado PC: %d\n", estado.pc);            
            break;

        case 9: //step
                // backup para backstep
                if (estado.topo_pilha < 2000) {
                    estado.pilha_back[estado.topo_pilha].pc       = estado.pc;
                    estado.pilha_back[estado.topo_pilha].banco_reg = banco;
                    estado.pilha_back[estado.topo_pilha].mem_dados = mem_dados;
                    estado.topo_pilha++;
                } else {
                    printf("limite do backstep atingido\n");
                }

                int pc_antes = estado.pc;

                //linha asm
                char linha_asm[40] = {0};
                if (estado.instrucao_t[pc_antes].instrucao_bruta == 0) {
                    sprintf(linha_asm, "add $r0, $r0, $r0  ");
                } else {
                    asm_gerador_char(linha_asm, &estado.instrucao_t[pc_antes]);
                    // \n
                    linha_asm[strcspn(linha_asm, "\n")] = '\0';
                }

                printf("\n+--[ STEP ]------------------------------------------+\n");
                printf("| PC: %-3d | Binario: %-16s            |\n",
                    pc_antes, memoria_instrucoes[pc_antes].total);
                printf("| ASM: %-45s |\n", linha_asm);
                printf("+----------------------------------------------------+\n");

                // salva registradores antes para comparar depois
                int8_t reg_antes[8];
                for (int i = 0; i < 8; i++) reg_antes[i] = banco.$[i];

                executar(&estado, banco, false);

                // resultado da ULA
                printf("| ULA -> resultado: %-4d | zero: %s | overflow: %s  |\n",
                    estado.ular.resultado,
                    estado.ular.zero     ? "sim" : "nao",
                    estado.ular.overflow ? "sim" : "nao");

                // PC apos execucao
                printf("| PC: %d -> %d", pc_antes, estado.pc);
                if (estado.sinal[branch] && estado.ular.zero)
                    printf("  (branch tomado)");
                else if (estado.sinal[jump])
                    printf("  (jump)");
                else
                    printf("  (+1)");
                printf("\n+----------------------------------------------------+\n");

                // mostra só registradores que mudaram
                printf("| Registradores alterados:                           |\n");
                bool algum = false;
                for (int i = 0; i < 8; i++) {
                    if (banco.$[i] != reg_antes[i]) {
                        printf("|   $r%d: %d -> %d\n", i, reg_antes[i], banco.$[i]);
                        algum = true;
                    }
                }
                if (!algum) printf("|   (nenhum)\n");
                printf("+----------------------------------------------------+\n");

                imprime_registradores(&banco);

                break;
                
        case 10:
            
            if (estado.topo_pilha > 0) {
                //decrementa
                estado.topo_pilha--; 

                //restauracao
                estado.pc = estado.pilha_back[estado.topo_pilha].pc;
                banco = estado.pilha_back[estado.topo_pilha].banco_reg;
                mem_dados = estado.pilha_back[estado.topo_pilha].mem_dados;
                
                printf("back: restaurado para PC=%d\n", estado.pc);
                imprime_registradores(&banco);
            } else {
                printf("nao tem instrucao anterior para voltar\n");
            }           
            break;

        
        case 11:
                char nome_arq[20];
                printf("Digite o nome do arquivo a ser gerado (ex: meu_codigo.asm): ");
                scanf("%19s", nome_arq);
                 printf("gerando :)...\n");
                asm_gerador(memoria_instrucoes, num_instrucoes, nome_arq);
                printf("arquivo .asm pronto\n");
   
            break;
        case 12:
            printf("Estatisticas: \n");
            printf("Total de instrucoes: %d\n", estado.total_instrucoes);
            printf("R: %i\n", estado.r_instrucoes);
            printf("I: %i\n", estado.i_instrucoes);
            printf("J: %i\n", estado.j_instrucoes);
            printf("Somas 0: %i\n", estado.nop_instrucoes);
            break;
        case 0:
            estado.pc = 0;
            estado.topo_pilha = 0;
            estado.nop_instrucoes=0;
            inicia_registradores(&banco);
            memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
            estado.total_instrucoes = 0;
            estado.r_instrucoes = 0;
            estado.i_instrucoes = 0;
            estado.j_instrucoes = 0;
            estado.nop_instrucoes = 0;
            printf("foi resetado :) .\n");
            break;

        case 123:
            printf("exiting\n");
            break;

        default:
            printf("invalido \n");
            break;
        }
    }
    while (menu != 123);

    if (memoria_instrucoes != NULL) free(memoria_instrucoes);
    return 0;
}

void limpa_buff()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
