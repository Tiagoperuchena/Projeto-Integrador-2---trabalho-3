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
#define C_SEL       12  

#define STAGE_H     16   
#define STAGE_TOP    2   


static int g_stage_w = 24;
static int g_preg_w  = 28;

static int stage_x(int s) {
    int sw = g_stage_w + 2, pw = g_preg_w + 2;
    return s * (sw + pw);
}
static int preg_x(int p) {
    int sw = g_stage_w + 2, pw = g_preg_w + 2;
    return sw + p * (sw + pw);
}

static void inicia_cores(void) {
    start_color();
    assume_default_colors(COLOR_WHITE, COLOR_BLACK);
    init_pair(C_DEFAULT, COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_HEADER,  COLOR_CYAN,    COLOR_BLACK);
    init_pair(C_STAGE,   COLOR_YELLOW,  COLOR_BLACK);
    init_pair(C_ACTIVE,  COLOR_GREEN,   COLOR_BLACK);
    init_pair(C_NOP,     COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_SIGNAL,  COLOR_BLUE,    COLOR_BLACK);
    init_pair(C_REG,     COLOR_GREEN,   COLOR_BLACK);
    init_pair(C_BAR,     COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_WARN,    COLOR_RED,     COLOR_BLACK);
    init_pair(C_KEY,     COLOR_CYAN,    COLOR_BLACK);
    init_pair(C_PREG,    COLOR_RED,     COLOR_BLACK);
    init_pair(C_SEL,     COLOR_BLACK,   COLOR_WHITE);
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

static int getmax(const int *arr, int n) {
    if (!arr || n <= 0) return 0;
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx) mx = arr[i];
    return mx;
}

static int getmax8(const int8_t *arr, int n) {
    if (!arr || n <= 0) return 0;
    int mx = (int)arr[0];
    for (int i = 1; i < n; i++)
        if ((int)arr[i] > mx) mx = (int)arr[i];
    return mx;
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
    int w = g_stage_w + 2;
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
        
        move(y+5, x+2); sinal_cell(sinais[esc_reg], mascara[esc_reg], "RgW");
        printw(" "); sinal_cell(sinais[mem_reg], mascara[mem_reg], "MmR");
       
        move(y+6, x+2); sinal_cell(sinais[esc_mem], mascara[esc_mem], "MmW");
        printw(" "); sinal_cell(sinais[branch],  mascara[branch],  "Brc");
      
        move(y+7, x+2); sinal_cell(sinais[jump],    mascara[jump],    "Jmp");
        printw(" "); sinal_cell(sinais[ula_fon], mascara[ula_fon], "Src");
      
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
    int w = g_preg_w + 2;
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

    box_col(cpair, abold, y, x, STAGE_H, w);

    int tlen = strlen(nomes[reg_id]);
    int tx   = x + (w - tlen) / 2;
    cor(cpair, abold);
    mvprintw(y+1, tx, "%s", nomes[reg_id]);
    descor(cpair, abold);

    htee_at(cpair, A_DIM, y+2, x, w);

    if (!valido) {
        cor(C_NOP, A_DIM);
        for (int r = 3; r < STAGE_H-1; r++)
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
            
            const char *fwd_a = "";
            if (st->ID_EX.fwd_a == 1) fwd_a = "[Fwd:EX]";
            else if (st->ID_EX.fwd_a == 2) fwd_a = "[Fwd:WB]";
            PROW("A($r%d)=%d %s", ins.rs, st->ID_EX.valor_a, fwd_a);
           
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

    const int R = 14;          

    cor(C_WARN, A_BOLD);                         
    mvprintw(y+1, x,   "PC  %4d", st->pc < 256 ? st->pc : 0);
    descor(C_WARN, A_BOLD);

    cor(C_ACTIVE, A_BOLD);                         
    mvprintw(y+2, x,   "Cic %4d", st->total_ciclos);
    descor(C_ACTIVE, A_BOLD);

    cor(C_DEFAULT, 0);
    mvprintw(y+3, x,   "Cnc %4d", st->total_instrucoes);

    cor(C_ACTIVE, A_BOLD);                         
    mvprintw(y+4, x,   "CPI %4.2f", cpi);
    descor(C_ACTIVE, A_BOLD);

    cor(C_DEFAULT, 0);
    mvprintw(y+5, x,   "Tam %4d", n_ins > 0 ? n_ins : 0);

   
    hline_at(y+6, x, 28);

    
    mvprintw(y+7,  x,   "R %4d", st->r_instrucoes);
    mvprintw(y+8,  x,   "I %4d", st->i_instrucoes);
    mvprintw(y+9,  x,   "J %4d", st->j_instrucoes);

    
    mvprintw(y+7,  x+R, "NOP %4d", st->nop_instrucoes);
    mvprintw(y+8,  x+R, "Bub %4d", st->total_bolhas);
    mvprintw(y+9,  x+R, "Fwd %4d", st->total_forwardings);

    descor(C_DEFAULT, 0);
}

static void desenha_registradores(int y, int x, const BancoRegistradores *b) {
    
    const int CW = 18;   
    const int GAP = 2;  

    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " REGISTRADORES ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

   
    for (int i = 0; i < 4; i++) {
     
        int vl = b->$[i];
        cor(vl ? C_REG : C_NOP, vl ? A_BOLD : A_DIM);
        mvprintw(y+1+i, x,           "$r%d %6d (0x%02X)", i,   vl, (unsigned char)vl);
        descor(vl ? C_REG : C_NOP, vl ? A_BOLD : A_DIM);

        
        int vr = b->$[i+4];
        cor(vr ? C_REG : C_NOP, vr ? A_BOLD : A_DIM);
        mvprintw(y+1+i, x + CW + GAP, "$r%d %6d (0x%02X)", i+4, vr, (unsigned char)vr);
        descor(vr ? C_REG : C_NOP, vr ? A_BOLD : A_DIM);
    }


    int mx = getmax8(b->$, (int)(sizeof(b->$) / sizeof(b->$[0])));
    cor(C_DEFAULT, A_DIM);
    mvprintw(y+5, x, "MaxReg %4d", mx);
    descor(C_DEFAULT, A_DIM);
}

#define N_CMDS 6

static void desenha_menu(int y, int x, int sel) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " COMANDOS ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    struct { const char *k; const char *d; } cmds[N_CMDS] = {
        { "[1]",     "Carregar .mem"   },
        { "[2]",     "Ver memorias"    },
        { "[SPACE]", "Avancar ciclo"   },
        { "[b]",     "Voltar ciclo"    },
        { "[0]",     "Reset"           },
        { "[q]",     "Sair"            },
    };

    for (int i = 0; i < N_CMDS; i++) {
        bool ativo = (i == sel);
        char linha[24];
        snprintf(linha, sizeof(linha), "%-8s %s", cmds[i].k, cmds[i].d);

        if (ativo) {
            attron(COLOR_PAIR(C_SEL) | A_BOLD);
            mvprintw(y+1+i, x, " %-21s", linha);
            attroff(COLOR_PAIR(C_SEL) | A_BOLD);
        } else {
            cor(C_KEY, A_BOLD);
            mvprintw(y+1+i, x, " %-8s", cmds[i].k);
            descor(C_KEY, A_BOLD);
            cor(C_DEFAULT, 0);
            printw(" %s", cmds[i].d);
            descor(C_DEFAULT, 0);
        }
    }

    
    cor(C_NOP, A_DIM);
    mvprintw(y + 1 + N_CMDS + 1, x, "[Up][Dn] mover  [Enter] exec");
    descor(C_NOP, A_DIM);
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

static void atualiza_tela(const typ_stt *st, int n_ins, int sel) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);   


    {
       
        int avail = cols - 2;
 
        int sw = (avail - 34) / 9;
        if (sw < 20) sw = 20;   
        if (sw > 36) sw = 36;   
        g_stage_w = sw;
        g_preg_w  = sw + 4;
    }

    clear();
    bkgd(COLOR_PAIR(C_DEFAULT));

    {
        const char *titulo = "Mini MIPS - Pipeline  (UNIPAMPA)";
        int tlen = (int)strlen(titulo);
        int tx   = (cols - tlen) / 2;
        if (tx < 0) tx = 0;
        cor(C_HEADER, A_BOLD);
        mvprintw(0, tx, "%s", titulo);
        descor(C_HEADER, A_BOLD);
    }
    hline_at(1, 0, cols);

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
            (void)px;   
            desenha_preg(STAGE_TOP + 1, preg_x(s), s, st);
        }
    }

    int bot = STAGE_TOP + 1 + STAGE_H + 1;  
    hline_at(bot, 0, cols);


    {
       
        const int W_REG  = 40;
        const int W_MET  = 30;
        const int W_MENU = 27;
        const int W_LEG  = 16;

        
        int total_fixo = W_REG + W_MET + W_MENU + W_LEG;
        int sobra = cols - total_fixo;
        int gap = (sobra > 0) ? sobra / 3 : 0;
        if (gap > 6) gap = 6;  

        int col_reg  = 0;
        int col_met  = col_reg  + W_REG  + gap;
        int col_menu = col_met  + W_MET  + gap;
        int col_leg  = col_menu + W_MENU + gap;

        int bot_y = bot + 1;

        desenha_registradores(bot_y, col_reg, st->registradores);

        if (col_met + W_MET <= cols)
            desenha_status(bot_y, col_met, st, n_ins);

        if (col_menu + W_MENU <= cols)
            desenha_menu(bot_y, col_menu, sel);

        if (col_leg + W_LEG <= cols)
            desenha_legenda(bot_y, col_leg);
    }

    hline_at(rows - 2, 0, cols);
    cor(C_DEFAULT, 0);
    mvprintw(rows - 1, 0, "Insira: ");
    descor(C_DEFAULT, 0);

    refresh();
}


static void pad_hline(WINDOW *pad, int row, int w, chtype l, chtype m, chtype r) {
    mvwaddch(pad, row, 0, l);
    mvwhline(pad, row, 1, m, w - 2);
    mvwaddch(pad, row, w - 1, r);
}

static void pad_titulo(WINDOW *pad, int row, int w, int pair, const char *txt) {
    wattron(pad, COLOR_PAIR(pair) | A_BOLD);
    pad_hline(pad, row, w, ACS_VLINE, ' ', ACS_VLINE);
    int tlen = (int)strlen(txt);
    int tx   = (w - tlen) / 2;
    if (tx < 1) tx = 1;
    mvwprintw(pad, row, tx, "%s", txt);
    wattroff(pad, COLOR_PAIR(pair) | A_BOLD);
}

static void imprime_memorias(const typ_stt *st, int n_ins) {
    int scr_rows, scr_cols;
    getmaxyx(stdscr, scr_rows, scr_cols);

    int marg_y = 1, marg_x = 2;
    int win_h = scr_rows - marg_y * 2;
    int win_w = scr_cols - marg_x * 2;
    if (win_h < 7)  win_h = 7;
    if (win_w < 42) win_w = 42;
    int cont_h = win_h - 2;
    int cont_w = win_w - 2;

    int n_dados_linhas = (st->mem_dados != NULL) ? (256 / 8) : 1;
    int n_ins_linhas   = (st->instrucao_t != NULL && n_ins > 0) ? (n_ins + 1) : 1;
    int tot = 3 + n_ins_linhas + 3 + n_dados_linhas + 1;

    WINDOW *win = newwin(win_h, win_w, marg_y, marg_x);
    WINDOW *pad = newpad(tot + 2, cont_w);
    if (!win || !pad) {
        if (win) delwin(win);
        if (pad) delwin(pad);
        return;
    }
    keypad(win, TRUE);

   
    int row = 0;
    int pw  = cont_w;   

  
    wattron(pad, COLOR_PAIR(C_BAR) | A_DIM);
    pad_hline(pad, row++, pw, ACS_ULCORNER, ACS_HLINE, ACS_URCORNER);
    wattroff(pad, COLOR_PAIR(C_BAR) | A_DIM);

    pad_titulo(pad, row++, pw, C_HEADER, "MEMORIA DE INSTRUCOES");

    wattron(pad, COLOR_PAIR(C_BAR) | A_DIM);
    pad_hline(pad, row++, pw, ACS_LTEE, ACS_HLINE, ACS_RTEE);
    wattroff(pad, COLOR_PAIR(C_BAR) | A_DIM);

    if (st->instrucao_t == NULL || n_ins == 0) {
        wattron(pad, COLOR_PAIR(C_WARN) | A_BOLD);
        mvwprintw(pad, row++, 2, "NENHUMA INSTRUCAO CARREGADA");
        wattroff(pad, COLOR_PAIR(C_WARN) | A_BOLD);
    } else {
      
        int *vals = (int *)malloc(n_ins * sizeof(int));
        if (vals) {
            for (int i = 0; i < n_ins; i++)
                vals[i] = (int)st->instrucao_t[i].instrucao_bruta;
            int mx = getmax(vals, n_ins);
            free(vals);
            wattron(pad, COLOR_PAIR(C_WARN) | A_BOLD);
           
            wattroff(pad, COLOR_PAIR(C_WARN) | A_BOLD);
        }
        for (int i = 0; i < n_ins; i++) {
            char ab[30];
            ins_asm(ab, &st->instrucao_t[i]);
            ab[29] = '\0';
            wattron(pad, COLOR_PAIR(C_DEFAULT));
            mvwprintw(pad, row++, 2, "[%03d]  %-16s  %-28s",
                      i, st->instrucao_t[i].total, ab);
            wattroff(pad, COLOR_PAIR(C_DEFAULT));
        }
    }

    wattron(pad, COLOR_PAIR(C_BAR) | A_DIM);
    pad_hline(pad, row++, pw, ACS_LTEE, ACS_HLINE, ACS_RTEE);
    wattroff(pad, COLOR_PAIR(C_BAR) | A_DIM);

    pad_titulo(pad, row++, pw, C_HEADER, "MEMORIA DE DADOS");

    wattron(pad, COLOR_PAIR(C_BAR) | A_DIM);
    pad_hline(pad, row++, pw, ACS_LTEE, ACS_HLINE, ACS_RTEE);
    wattroff(pad, COLOR_PAIR(C_BAR) | A_DIM);

    if (st->mem_dados == NULL) {
        wattron(pad, COLOR_PAIR(C_WARN) | A_BOLD);
        mvwprintw(pad, row++, 2, "MEMORIA DE DADOS NAO INICIALIZADA");
        wattroff(pad, COLOR_PAIR(C_WARN) | A_BOLD);
    } else {
        int mx_dado = getmax8(st->mem_dados->dados, 256);
        wattron(pad, COLOR_PAIR(C_WARN) | A_BOLD);
       
        wattroff(pad, COLOR_PAIR(C_WARN) | A_BOLD);
        for (int i = 0; i < 256; i += 8) {
            wattron(pad, COLOR_PAIR(C_DEFAULT));
            mvwprintw(pad, row++, 2,
                      "[%03d]:%-4d [%03d]:%-4d [%03d]:%-4d [%03d]:%-4d"
                      " [%03d]:%-4d [%03d]:%-4d [%03d]:%-4d [%03d]:%-4d",
                      i,   st->mem_dados->dados[i],
                      i+1, st->mem_dados->dados[i+1],
                      i+2, st->mem_dados->dados[i+2],
                      i+3, st->mem_dados->dados[i+3],
                      i+4, st->mem_dados->dados[i+4],
                      i+5, st->mem_dados->dados[i+5],
                      i+6, st->mem_dados->dados[i+6],
                      i+7, st->mem_dados->dados[i+7]);
            wattroff(pad, COLOR_PAIR(C_DEFAULT));
        }
    }

    wattron(pad, COLOR_PAIR(C_BAR) | A_DIM);
    pad_hline(pad, row++, pw, ACS_LLCORNER, ACS_HLINE, ACS_LRCORNER);
    wattroff(pad, COLOR_PAIR(C_BAR) | A_DIM);

    tot = row; 

    int scroll = 0;
    int max_scroll = tot - cont_h;
    if (max_scroll < 0) max_scroll = 0;

    
    int pad_top  = marg_y + 1;           
    int pad_left = marg_x + 1;           
    int pad_bot  = pad_top + cont_h - 1; 
    int pad_right= pad_left + cont_w - 1;

    bool fechar = false;
    while (!fechar) {
        
        int cur_rows, cur_cols;
        getmaxyx(stdscr, cur_rows, cur_cols);
        (void)cur_rows; (void)cur_cols;

        int wh_now, ww_now;
        getmaxyx(win, wh_now, ww_now);
        int ch_now = wh_now - 2;  

        wattron(win, COLOR_PAIR(C_BAR) | A_DIM);
        box(win, 0, 0);
        wattroff(win, COLOR_PAIR(C_BAR) | A_DIM);

        wattron(win, COLOR_PAIR(C_STAGE) | A_BOLD);
        mvwprintw(win, 0, 2, " VISUALIZADOR DE MEMORIAS ");
        wattroff(win, COLOR_PAIR(C_STAGE) | A_BOLD);

        
        const char *nav = " [k/UP] [j/DN] [PgUp] [PgDn] [Home] [End] [q/ESC] ";
        int nav_len = (int)strlen(nav);
        int nav_x = (ww_now - nav_len) / 2;
        if (nav_x < 1) nav_x = 1;
        wattron(win, COLOR_PAIR(C_NOP) | A_DIM);
        mvwprintw(win, wh_now - 1, nav_x, "%s", nav);
        
        mvwprintw(win, wh_now - 1, ww_now - 10, "[%3d/%3d]", scroll + 1, tot);
        wattroff(win, COLOR_PAIR(C_NOP) | A_DIM);

        wrefresh(win);

        
        int ms = tot - ch_now;
        if (ms < 0) ms = 0;
        if (scroll > ms) scroll = ms;

        prefresh(pad, scroll, 0,
                 pad_top, pad_left,
                 pad_bot, pad_right);

        int c = wgetch(win);
        switch (c) {
            case KEY_UP:   case 'k': case 'K':
                if (scroll > 0) scroll--;
                break;
            case KEY_DOWN: case 'j': case 'J':
                if (scroll < ms) scroll++;
                break;
            case KEY_PPAGE:
                scroll -= ch_now - 1;
                if (scroll < 0) scroll = 0;
                break;
            case KEY_NPAGE:
                scroll += ch_now - 1;
                if (scroll > ms) scroll = ms;
                break;
            case KEY_HOME: scroll = 0;   break;
            case KEY_END:  scroll = ms;  break;
            case 'q': case 'Q': case 27:
                fechar = true;
                break;
        }
    }

    delwin(pad);
    delwin(win);
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
    bkgd(COLOR_PAIR(C_DEFAULT));   

    int sel = 0;          
    bool sair = false;
    while (!sair) {
        atualiza_tela(&estado, num_instrucoes, sel);

        int cur_rows, cur_cols;
        getmaxyx(stdscr, cur_rows, cur_cols);
        (void)cur_cols;

        static int ultima_bolha = 0;
        if (estado.total_bolhas > ultima_bolha) {
            ultima_bolha = estado.total_bolhas;
            mvprintw(cur_rows - 1, 20, ">>> BOLHA INSERIDA! <<<");
        } else {
            mvprintw(cur_rows - 1, 20, "                        ");
        }
        refresh();

        int ch = getch();
        switch (ch) {
            
            case KEY_RESIZE:
                endwin();
                refresh();
                bkgd(COLOR_PAIR(C_DEFAULT));
                clear();
                break;

            
            case KEY_UP:
                sel = (sel - 1 + N_CMDS) % N_CMDS;
                break;
            case KEY_DOWN:
                sel = (sel + 1) % N_CMDS;
                break;

            
            case '\n': case KEY_ENTER: {
                
                switch (sel) {
                    case 0: ch = '1'; break;
                    case 1: ch = '2'; break;
                    case 2: ch = ' '; break;
                    case 3: ch = 'b'; break;
                    case 4: ch = '0'; break;
                    case 5: ch = 'q'; break;
                }
                
                goto executa;
            }

           
            default:
            executa:
                switch (ch) {
                    case '1': {
                        int r1, c1; getmaxyx(stdscr, r1, c1); (void)c1;
                        echo(); curs_set(1);
                        mvprintw(r1 - 1, 0, "Arquivo .mem: "); clrtoeol();
                        char nome[64]; getnstr(nome, sizeof(nome)-1);
                        noecho(); curs_set(0);
                        if (memoria_instrucoes) free(memoria_instrucoes);
                        num_instrucoes = carregar_memoria_instrucoes(nome, &memoria_instrucoes);
                        estado.instrucao_t = (typ_ins *)memoria_instrucoes;
                        estado.pc = 0;
                        sel = 0;
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
                break;
        }
    }

    endwin();
    if (memoria_instrucoes) free(memoria_instrucoes);
    if (estado.pilha_back)  free(estado.pilha_back);
    return 0;
}