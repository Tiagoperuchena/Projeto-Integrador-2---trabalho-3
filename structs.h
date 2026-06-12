#include <stdint.h>
#include <stdbool.h>
#ifndef structs_H
#define structs_H

    enum tipos_de_instrucao
    {
        r = 1,
        i = 2,
        j = 3,

    };

    typedef struct str_instrucaoV2
    {
        enum tipos_de_instrucao tipo;
        uint16_t instrucao_bruta; //unsigned int 16 bits para instrução
        char total[18];

        uint16_t opcode;    // 15-12  (4 bits)
        uint16_t rs;        // 11-9  (3 bits)
        uint16_t rt;        // 8-6  (3 bits)
        uint16_t rd;        // 5-3 (3 bits)
        uint16_t funct;     // 2-0 (3 bits)
        int16_t  immediato; // 5-0 (6 bits) - int com sinal p aceita negativos
        uint16_t uimmediato;// 5-0 (6 bits) - mesma coisa q o de cima só q sem sinal
        uint16_t addr;      // 6-0 (7 bits)
    }typ_ins;
    typedef typ_ins typ_instrucaoSep;

    typedef struct str_regis
    {
        int8_t $[8];
    }typ_reg;

    typedef typ_reg BancoRegistradores;

    typedef struct {
    int8_t dados[256];
    } typ_mem_dados;
    typedef typ_mem_dados typ_mdd;

    //movi a ula p cima, para a typ_stt poder usar o typ_ulaR e typ_ulaOp
     /* ================ ULA ================ */
    typedef enum {
        ADD = 0,    // 0
        SUB = 2,    // 2
        AND = 4,    // 4
        OR  = 5,    // 5
        SLT
    }typ_ulaOp;

    typedef struct {
        int resultado;  // resultado
        bool zero;       // flag de 0 ou 1 beq 
        bool overflow;
    }typ_ulaR;

    typedef struct {
        uint8_t pc;
        typ_ins instrucao;
        bool valido;
    } typ_if_id;

    typedef struct {
        bool sinal[8];
        typ_ulaOp ulaop;

        uint8_t pc;
        int8_t valor_a;
        int8_t valor_b;
        int16_t imediato;
        uint8_t rt;
        uint8_t rd;
        uint16_t addr;
        bool valido;
        typ_ins instrucao;
    } typ_id_ex;
    
    typedef struct {
        bool sinal[8];

        uint8_t pc;
        uint8_t pc_branch;
        bool zero;
        int8_t resultado_ula;
        int8_t valor_b;
        uint8_t reg_destino;
        uint16_t addr;
        bool valido;
        typ_ins instrucao;
    } typ_ex_mem;

    typedef struct {
        bool sinal[8];

        uint8_t pc;
        int8_t saida_mem;
        int8_t resultado_ula;
        uint8_t reg_destino;
        bool valido;
        typ_ins instrucao;
    } typ_mem_wb;

    typedef struct {
        int pc;
        typ_reg banco_reg;
        typ_mdd mem_dados;
        typ_if_id IF_ID;
        typ_id_ex ID_EX;
        typ_ex_mem EX_MEM;
        typ_mem_wb MEM_WB;
    }print;
    


    
    typedef struct str_state
    {
        typ_ins *instrucao_t;
        typ_reg *registradores;
        typ_mdd *mem_dados;
        int cont_inst;
        unsigned char pc;
        int hist_topo;

        print *pilha_back; //backup 
        int topo_pilha;
        int capacidade_pilha;
       
        int total_instrucoes;
        int total_ciclos;
        int r_instrucoes;
        int i_instrucoes;
        int j_instrucoes;
        int nop_instrucoes;

        bool sinal[8];
        typ_ulaOp ulaop;
        typ_ulaR ular;
        bool estouro;

        // Registradores de pipeline
        typ_if_id IF_ID;
        typ_id_ex ID_EX;
        typ_ex_mem EX_MEM;
        typ_mem_wb MEM_WB;
    }typ_stt;

    enum
    {
        esc_mem = 0, 
        esc_reg = 1, 
        mem_reg = 2, 
        ula_fon = 3, //(registrador ou imediato)
        reg_des = 4, 
        inc_pc  = 5, 
        jump    = 6, 
        branch  = 7, 

    };
    enum tipo_i
    {
        addi = 4,
        beq  = 8,
        lw   = 11,
        sw   = 15,
    };



#endif