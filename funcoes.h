#include"structs.h"

#ifndef funcoes_H
#define funcoes_H  

    // leitura de comandos bit a bit
    int carregar_memoria_instrucoes(typ_instrucaoSep **mem_out);
   
    // ULA
    typ_ulaR ula(int A, int B, typ_ulaOp operacao);

    void estagio_IF(typ_stt *estado);
    void estagio_ID(typ_stt *estado);
    void estagio_EX(typ_stt *estado);
    void estagio_MEM(typ_stt *estado);
    void estagio_WB(typ_stt *estado);
    
    // controlador
    typ_stt *controlador(typ_stt *state, int n);
   
    void asm_gerador(typ_ins *ins, int n,char *nome_arq);
    int asm_gerador_char(char *asembly, typ_ins *instrucao);
   
    // banco de registradores
    void inicia_registradores(BancoRegistradores *banco);
    void escreve_registrador(BancoRegistradores *banco, int indice, int valor);
    int le_registrador(BancoRegistradores *banco, int indice);
    void imprime_registradores(BancoRegistradores *banco);

    // memoria de dados
    int8_t mem_data(typ_mdd *mem, unsigned int endereco, int8_t D, bool str, bool ld, bool clear);//passando o ponteiro p mem dataz
    int executar(typ_stt *status, typ_reg reg, bool clear_data);

    //pilha
    void push_estagio(typ_stt *state);
    void pop_estagio(typ_stt *state);
#endif