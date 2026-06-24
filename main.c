#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "structs.h"
#include "funcoes.h"


#define C_DEFAULT   1
#define C_HEADER    2
#define C_STAGE     3
#define C_ACTIVE    4
#define C_NOP       5
#define C_SIGNAL    6
#define C_REG       7
#define C_BAR       8
#define C_WARN      9
#define C_KEY       10
#define C_PREG      11   

#define STAGE_W     24   
#define STAGE_H     16   
#define PREG_W      28
#define PREG_H      STAGE_H
#define STAGE_TOP    2   
#define SW  (STAGE_W + 2)
#define PW  (PREG_W  + 2)

static int stage_x(int s) { return s * (SW + PW); }
static int preg_x (int p) { return SW + p * (SW + PW); }

static void inicia_cores(void) {
    start_color();
    use_default_colors();
    init_pair(C_DEFAULT, COLOR_WHITE,   -1);
    init_pair(C_HEADER,  COLOR_CYAN,    -1);
    init_pair(C_STAGE,   COLOR_YELLOW,  -1);
    init_pair(C_ACTIVE,  COLOR_GREEN,   -1);
    init_pair(C_NOP,     COLOR_WHITE,   -1);
    init_pair(C_SIGNAL,  COLOR_BLUE,    -1);
    init_pair(C_REG,     COLOR_GREEN,   -1);
    init_pair(C_BAR,     COLOR_WHITE,   -1);
    init_pair(C_WARN,    COLOR_RED,     -1);
    init_pair(C_KEY,     COLOR_CYAN,    -1);
    init_pair(C_PREG,    COLOR_MAGENTA, -1);
}

static void cor  (int p, int a) { attron (COLOR_PAIR(p) | a); }
static void descor(int p, int a) { attroff(COLOR_PAIR(p) | a); }

static void hline_at(int y, int x, int w) {
    cor(C_BAR, A_DIM);
    mvhline(y, x, ACS_HLINE, w);
    descor(C_BAR, A_DIM);
}

static void box_col(int pair, int attr, int y, int x, int h, int w) {
    cor(pair, attr);
    mvaddch(y,     x,     ACS_ULCORNER);
    mvaddch(y,     x+w-1, ACS_URCORNER);
    mvaddch(y+h-1, x,     ACS_LLCORNER);
    mvaddch(y+h-1, x+w-1, ACS_LRCORNER);
    mvhline(y,     x+1, ACS_HLINE, w-2);
    mvhline(y+h-1, x+1, ACS_HLINE, w-2);
    mvvline(y+1,   x,   ACS_VLINE, h-2);
    mvvline(y+1,   x+w-1, ACS_VLINE, h-2);
    descor(pair, attr);
}

static void htee_at(int pair, int attr, int y, int x, int w) {
    cor(pair, attr);
    mvaddch(y, x,     ACS_LTEE);
    mvhline(y, x+1,   ACS_HLINE, w-2);
    mvaddch(y, x+w-1, ACS_RTEE);
    descor(pair, attr);
}

static void ins_asm(char *out, const typ_ins *ins) {
    if (!ins || ins->instrucao_bruta == 0) { 
        strncpy(out, "NOP", 27);
        out[27] = '\0';
        return; 
    }
    asm_gerador_char(out, (typ_ins *)ins);
    char *newline = strchr(out, '\n');
    if (newline) *newline = '\0';
}

static void sinal_cell(bool val, bool ativo, const char *label) {
    if (ativo) {
        cor(C_SIGNAL, A_BOLD);
        printw("%s:%d", label, val ? 1 : 0);
        descor(C_SIGNAL, A_BOLD);
    } else {
        cor(C_NOP, A_DIM);
        printw("%s:-", label);
        descor(C_NOP, A_DIM);
    }
}


static void desenha_estagio(int y, int x,
                             const char *sigla, const char *nome,
                             const typ_ins *ins, bool valido,
                             const bool sinais[8], const bool mascara[8])
{
    int w = SW;
    int cpair = valido ? C_ACTIVE : C_NOP;
    int abold  = valido ? A_BOLD  : A_DIM;

    box_col(C_BAR, A_DIM, y, x, STAGE_H, w);

    cor(cpair, abold);
    mvprintw(y+1, x+1, " %-*.*s ", w-4, w-4, "");
    mvprintw(y+1, x+2, "%s  %s", sigla, nome);
    descor(cpair, abold);

    htee_at(C_BAR, A_DIM, y+2, x, w);

    char asm_buf[28] = "---";
    if (valido) ins_asm(asm_buf, ins);
    asm_buf[w-3] = '\0';
    bool is_nop = (!ins || ins->instrucao_bruta == 0);
    cor(valido && !is_nop ? C_ACTIVE : C_NOP,
        valido && !is_nop ? A_BOLD   : A_DIM);
    mvprintw(y+3, x+1, " %-*.*s ", w-4, w-4, asm_buf);
    descor(valido && !is_nop ? C_ACTIVE : C_NOP,
           valido && !is_nop ? A_BOLD   : A_DIM);

    htee_at(C_BAR, A_DIM, y+4, x, w);

    if (sinais && valido && ins && ins->instrucao_bruta != 0) {
        /* linha 1 */
        move(y+5, x+2); sinal_cell(sinais[esc_reg], mascara[esc_reg], "RgW");
        printw(" "); sinal_cell(sinais[mem_reg], mascara[mem_reg], "MmR");
        /* linha 2 */
        move(y+6, x+2); sinal_cell(sinais[esc_mem], mascara[esc_mem], "MmW");
        printw(" "); sinal_cell(sinais[branch],  mascara[branch],  "Brc");
        /* linha 3 */
        move(y+7, x+2); sinal_cell(sinais[jump],    mascara[jump],    "Jmp");
        printw(" "); sinal_cell(sinais[ula_fon], mascara[ula_fon], "Src");
        /* linha 4 */
        move(y+8, x+2); sinal_cell(sinais[reg_des], mascara[reg_des], "RgD");
    } else {
        cor(C_NOP, A_DIM);
        for (int r = 5; r <= 8; r++)
            mvprintw(y+r, x+1, " %-*s ", w-4, "");
        descor(C_NOP, A_DIM);
    }

    htee_at(C_BAR, A_DIM, y+9, x, w);
   
}


static void desenha_preg(int y, int x, int reg_id, const typ_stt *st)
{
    int w = PW;
    bool valido = false;

    const char *nomes[] = { "IF/ID", "ID/EX", "EX/MEM", "MEM/WB" };

    switch (reg_id) {
        case 0: valido = st->IF_ID.valido;  break;
        case 1: valido = st->ID_EX.valido;  break;
        case 2: valido = st->EX_MEM.valido; break;
        case 3: valido = st->MEM_WB.valido; break;
    }

    int cpair = valido ? C_PREG  : C_NOP;
    int abold  = valido ? A_BOLD  : A_DIM;

    box_col(cpair, abold, y, x, PREG_H, w);

    int tlen = strlen(nomes[reg_id]);
    int tx   = x + (w - tlen) / 2;
    cor(cpair, abold);
    mvprintw(y+1, tx, "%s", nomes[reg_id]);
    descor(cpair, abold);

    htee_at(cpair, A_DIM, y+2, x, w);

    if (!valido) {
        cor(C_NOP, A_DIM);
        for (int r = 3; r < PREG_H-1; r++)
            mvprintw(y+r, x+1, "%-*s", w-2, "");
        descor(C_NOP, A_DIM);
        return;
    }

    cor(C_PREG, 0);
    int row = y + 3;

#define PROW(fmt, ...) do { mvprintw(row++, x+1, " %-*.*s", w-3, w-3, ""); \
                            mvprintw(row-1, x+1, " " fmt, ##__VA_ARGS__); } while(0)

    switch (reg_id) {
        case 0: { // IF/ID
            typ_ins ins = st->IF_ID.instrucao;
            char asm_buf[28]; ins_asm(asm_buf, &ins); asm_buf[w-3]='\0';
            PROW("PC: %-4d", st->IF_ID.pc);
            PROW("Inst: %04X", ins.instrucao_bruta);
            PROW("%s", asm_buf);
            PROW("op=%d rs=%d rt=%d rd=%d imm=%d",
                 ins.opcode, ins.rs, ins.rt, ins.rd, ins.immediato);
            break;
        }
        case 1: { // ID/EX
            typ_ins ins = st->ID_EX.instrucao;
            char asm_buf[28]; ins_asm(asm_buf, &ins); asm_buf[w-3]='\0';
            PROW("%s", asm_buf);
            // Valor A com indicação de forwarding
            const char *fwd_a = "";
            if (st->ID_EX.fwd_a == 1) fwd_a = "[Fwd:EX]";
            else if (st->ID_EX.fwd_a == 2) fwd_a = "[Fwd:WB]";
            PROW("A($r%d)=%d %s", ins.rs, st->ID_EX.valor_a, fwd_a);
            // Valor B com indicação de forwarding
            const char *fwd_b = "";
            if (st->ID_EX.fwd_b == 1) fwd_b = "[Fwd:EX]";
            else if (st->ID_EX.fwd_b == 2) fwd_b = "[Fwd:WB]";
            PROW("B($r%d)=%d %s", ins.rt, st->ID_EX.valor_b, fwd_b);
            PROW("Imm=%d Dest: $r%d", st->ID_EX.imediato,
                 st->ID_EX.sinal[reg_des] ? st->ID_EX.rd : st->ID_EX.rt);
            PROW("Src=%d RegDst=%d ALUOp=%d",
                 st->ID_EX.sinal[ula_fon], st->ID_EX.sinal[reg_des], st->ID_EX.ulaop);
            break;
        }
        case 2: { // EX/MEM
            typ_ins ins = st->EX_MEM.instrucao;
            char asm_buf[28]; ins_asm(asm_buf, &ins); asm_buf[w-3]='\0';
            PROW("%s", asm_buf);
            PROW("ALU=%d Zero=%d", st->EX_MEM.resultado_ula, st->EX_MEM.zero);
            PROW("B(store)=%d", st->EX_MEM.valor_b);
            if (st->EX_MEM.sinal[branch]) {
                bool taken = st->EX_MEM.zero && st->EX_MEM.sinal[branch];
                PROW("PCbranch=%d %s", st->EX_MEM.pc_branch, taken ? "(taken)" : "(not)");
            } else {
                PROW("PCbranch=-");
            }
            PROW("Dest: $r%d", st->EX_MEM.reg_destino);
            PROW("MemRd=%d MemWr=%d Br=%d",
                 st->EX_MEM.sinal[mem_reg], st->EX_MEM.sinal[esc_mem], st->EX_MEM.sinal[branch]);
            break;
        }
        case 3: { // MEM/WB
            typ_ins ins = st->MEM_WB.instrucao;
            char asm_buf[28]; ins_asm(asm_buf, &ins); asm_buf[w-3]='\0';
            PROW("%s", asm_buf);
            PROW("Mem out=%d", st->MEM_WB.saida_mem);
            PROW("ALU out=%d", st->MEM_WB.resultado_ula);
            int valor_final = st->MEM_WB.sinal[mem_reg] ? st->MEM_WB.resultado_ula : st->MEM_WB.saida_mem;
            PROW("Mux(MemToReg)=%s → %d",
                 st->MEM_WB.sinal[mem_reg] ? "ALU" : "Mem", valor_final);
            PROW("Dest: $r%d RegWrite=%d", st->MEM_WB.reg_destino, st->MEM_WB.sinal[esc_reg]);
            break;
        }
    }
#undef PROW
    descor(C_PREG, 0);
}

static void desenha_status(int y, int x, const typ_stt *st, int n_ins) {
    float cpi = st->total_instrucoes > 0
                ? (float)st->total_ciclos / st->total_instrucoes : 0.f;

    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " METRICAS ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    cor(C_DEFAULT, 0);
    mvprintw(y+1,  x, "PC       %4d",   st->pc < 256 ? st->pc : 0);
    mvprintw(y+2,  x, "Ciclos   %4d",   st->total_ciclos);
    mvprintw(y+3,  x, "Concl.   %4d",   st->total_instrucoes);
    mvprintw(y+4,  x, "CPI      %4.2f", cpi);
    mvprintw(y+5,  x, "Prog.tam %4d",   n_ins > 0 ? n_ins : 0);
    hline_at(y+6, x, 15);
    mvprintw(y+7,  x, "R-type   %4d",   st->r_instrucoes);
    mvprintw(y+8,  x, "I-type   %4d",   st->i_instrucoes);
    mvprintw(y+9,  x, "J-type   %4d",   st->j_instrucoes);
    mvprintw(y+10, x, "NOP  %4d",   st->nop_instrucoes);
    mvprintw(y+11, x, "Bolhas   %4d",   st->total_bolhas);
    mvprintw(y+12, x, "Fwd      %4d",   st->total_forwardings); 
    
    descor(C_DEFAULT, 0);
}

static void desenha_registradores(int y, int x, const BancoRegistradores *b) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " REGISTRADORES ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    for (int i = 0; i < 8; i++) {
        int v = b->$[i]; 
        cor(v ? C_REG : C_NOP, v ? A_BOLD : A_DIM);
        mvprintw(y+1+i, x, "$r%d %6d (0x%02X)", i, v, (unsigned char)v);
        descor(v ? C_REG : C_NOP, v ? A_BOLD : A_DIM);
    }
}

static void desenha_menu(int y, int x) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " COMANDOS ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    struct { const char *k; const char *d; } cmds[] = {
        { "[1]",     "Carregar .mem"   },
        { "[2]",     "Ver memorias"    },
        { "[SPACE]", "Avancar ciclo"   },
        { "[b]",     "Voltar ciclo"    },
        { "[0]",     "Reset"           },
        { "[q]",     "Sair"            },
    };
    int n = (int)(sizeof(cmds)/sizeof(cmds[0]));
    for (int i = 0; i < n; i++) {
        cor(C_KEY, A_BOLD);  mvprintw(y+1+i, x, "%-8s", cmds[i].k); descor(C_KEY, A_BOLD);
        cor(C_DEFAULT, 0);   printw(" %s", cmds[i].d);               descor(C_DEFAULT, 0);
    }
}

static void desenha_legenda(int y, int x) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " LEGENDA ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    cor(C_SIGNAL, A_DIM);
    mvprintw(y+1, x, "RgW RegWrite");
    mvprintw(y+2, x, "MmR MemRead ");
    mvprintw(y+3, x, "MmW MemWrite");
    mvprintw(y+4, x, "Brc Branch  ");
    mvprintw(y+5, x, "Jmp Jump    ");
    mvprintw(y+6, x, "Src AluSrc  ");
    mvprintw(y+7, x, "RgD RegDst  ");
    descor(C_SIGNAL, A_DIM);
}

static void atualiza_tela(const typ_stt *st, int n_ins) {
    clear();

    cor(C_HEADER, A_BOLD);
    mvprintw(0, 0, " Mini MIPS - Pipeline ");
    descor(C_HEADER, A_BOLD);
    cor(C_DEFAULT, A_DIM);
    mvprintw(0, 24, "(UNIPAMPA)");
    descor(C_DEFAULT, A_DIM);
    hline_at(1, 0, COLS);

                               
    static const bool mask_IF [8] = {0,0,0,0,0,0,0,0};
    static const bool mask_ID [8] = {0,0,0,0,0,0,0,0};
    static const bool mask_EX [8] = {1,1,1,1,1,0,1,1};
    static const bool mask_MEM[8] = {1,1,1,0,0,0,1,1};
    static const bool mask_WB [8] = {0,1,1,0,0,0,0,0};

    struct {
        const char    *sigla, *nome, *fase;
        const typ_ins *ins;
        bool           valido;
        const bool    *sinais, *mascara;
    } stages[5] = {
        {"IF",  "Fetch",     "1 FETCH",
         st->instrucao_t ? &st->instrucao_t[st->pc] : NULL,
         (st->instrucao_t != NULL && st->pc < 256),
         NULL,              mask_IF},
        {"ID",  "Decode",    "2 DECODE",
         &st->IF_ID.instrucao, st->IF_ID.valido,
         st->ID_EX.sinal,  mask_ID},
        {"EX",  "Execute",   "3 EXECUTE",
         &st->ID_EX.instrucao, st->ID_EX.valido,
         st->ID_EX.sinal,  mask_EX},
        {"MEM", "Memory",    "4 MEMORY",
         &st->EX_MEM.instrucao, st->EX_MEM.valido,
         st->EX_MEM.sinal, mask_MEM},
        {"WB",  "Writeback", "5 WRITEBACK",
         &st->MEM_WB.instrucao, st->MEM_WB.valido,
         st->MEM_WB.sinal, mask_WB},
    };

    for (int s = 0; s < 5; s++) {
        int cx = stage_x(s);
        cor(C_STAGE, A_BOLD);
        mvprintw(STAGE_TOP, cx + 1, "%s", stages[s].fase);
        descor(C_STAGE, A_BOLD);
    }

    for (int s = 0; s < 5; s++) {
        int cx = stage_x(s);
        desenha_estagio(STAGE_TOP + 1, cx,
                        stages[s].sigla, stages[s].nome,
                        stages[s].ins, stages[s].valido,
                        stages[s].sinais, stages[s].mascara);

        if (s < 4) {
            int px = preg_x(s);
            const char *rn[] = {"IF/ID","ID/EX","EX/MEM","MEM/WB"};
            /* já desenhado dentro de desenha_preg */
            desenha_preg(STAGE_TOP + 1, px, s, st);
        }
    }

    int bot = STAGE_TOP + 1 + STAGE_H + 1;  /* linha logo após os blocos */
    hline_at(bot, 0, COLS);

    desenha_registradores(bot + 1,  0,  st->registradores);
    desenha_status       (bot + 1,  26, st, n_ins);
    desenha_menu         (bot + 1,  44);
    desenha_legenda      (bot + 1,  68);

    hline_at(LINES - 2, 0, COLS);
    cor(C_DEFAULT, 0);
    mvprintw(LINES - 1, 0, "Insira: ");
    descor(C_DEFAULT, 0);

    refresh();
}


static void imprime_memorias(const typ_stt *st, int n_ins) {
    def_prog_mode();
    
    endwin();

    
    if (st->instrucao_t == NULL || n_ins == 0) {
        printf("\r\n\r\n============================================================\r\n");
        printf("                  MEMORIA DE INSTRUCOES\r\n");
        printf("============================================================\r\n");
        printf("               NENHUMA INSTRUCAO CARREGADA!\r\n");
        printf("============================================================\r\n");
    } else {
      
        printf("\r\n\r\n============================================================\r\n");
        printf("                  MEMORIA DE INSTRUCOES\r\n");
        printf("============================================================\r\n");
        for (int i = 0; i < n_ins; i++) {
            char ab[30]; 
            ins_asm(ab, &st->instrucao_t[i]);
            ab[29] = '\0';
            printf("[%03d]  %-16s  %-28s\r\n", i, st->instrucao_t[i].total, ab);
        }
    }

    printf("\r\n\r\n============================================================\r\n");
    printf("                     MEMORIA DE DADOS\r\n");
    printf("============================================================\r\n");
    
    if (st->mem_dados == NULL) {
        printf("               MEMORIA DE DADOS NAO INICIALIZADA!\r\n");
    } else {
        for (int i = 0; i < 256; i += 4) {
            printf("[%03d]:%-5d [%03d]:%-5d [%03d]:%-5d [%03d]:%-5d\r\n",
                   i,   st->mem_dados->dados[i],
                   i+1, st->mem_dados->dados[i+1],
                   i+2, st->mem_dados->dados[i+2],
                   i+3, st->mem_dados->dados[i+3]);
        }
    }
    printf("============================================================\r\n");

    
    printf("\r\n>>> Pressione ENTER para voltar ao simulador...\r\n");
    fflush(stdout);
    
    
    system("stty sane");
    
    
    char buffer[10];
    fgets(buffer, sizeof(buffer), stdin);

   
    reset_prog_mode();
    
    
    clear();
    refresh();
}
/* ── main ─────────────────────────────────────────────────────────── */
int main(void) {
    typ_stt estado;
    memset(&estado, 0, sizeof(typ_stt));

    BancoRegistradores banco;
    inicia_registradores(&banco);
    estado.registradores = &banco;

    typ_mem_dados mem_dados;
    memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
    estado.mem_dados = &mem_dados;

    estado.capacidade_pilha = 100;
    estado.pilha_back = (print *)malloc(estado.capacidade_pilha * sizeof(print));
    estado.topo_pilha = 0;
    estado.total_bolhas = 0;

    typ_instrucaoSep *memoria_instrucoes = NULL;
    int num_instrucoes = 0;

    initscr(); inicia_cores(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0);

    int ch; bool sair = false;
    while (!sair) {
        atualiza_tela(&estado, num_instrucoes);
        static int ultima_bolha = 0;

    if (estado.total_bolhas > ultima_bolha) {
        ultima_bolha = estado.total_bolhas;
        mvprintw(LINES-1, 20, ">>> BOLHA INSERIDA! <<<");
    } else {
        mvprintw(LINES-1, 20, "                        ");  
}
        ch = getch();
        switch (ch) {
            case '1': {
                echo(); curs_set(1);
                mvprintw(LINES-1, 0, "Arquivo .mem: "); clrtoeol();
                char nome[64]; getnstr(nome, sizeof(nome)-1);
                noecho(); curs_set(0);
                if (memoria_instrucoes) free(memoria_instrucoes);
                num_instrucoes = carregar_memoria_instrucoes(nome, &memoria_instrucoes);
                estado.instrucao_t = (typ_ins *)memoria_instrucoes;
                estado.pc = 0;
                break;
            }
            case '2': imprime_memorias(&estado, num_instrucoes); break;
            case '9': case ' ':
                push_estagio(&estado);
                executar(&estado, banco, false);
                break;
            case 'b': case 'B':
                if (estado.topo_pilha > 0) pop_estagio(&estado);
                break;
            case '0':
                estado.pc = 0; estado.total_ciclos = 0; estado.total_instrucoes = 0;
                estado.r_instrucoes = 0; estado.i_instrucoes = 0;
                estado.j_instrucoes = 0; estado.nop_instrucoes = 0;
                estado.total_forwardings = 0;
                estado.total_bolhas = 0;
                inicia_registradores(&banco);
                memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
                memset(&estado.IF_ID, 0, sizeof(estado.IF_ID));
                memset(&estado.ID_EX, 0, sizeof(estado.ID_EX));
                memset(&estado.EX_MEM, 0, sizeof(estado.EX_MEM));
                memset(&estado.MEM_WB, 0, sizeof(estado.MEM_WB));
                estado.topo_pilha = 0;
                break;
            case 'q': case 'Q': sair = true; break;
        }
    }

    endwin();
    if (memoria_instrucoes) free(memoria_instrucoes);
    if (estado.pilha_back)  free(estado.pilha_back);
    return 0;
}