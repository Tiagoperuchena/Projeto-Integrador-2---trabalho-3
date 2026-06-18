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

#define STAGE_W      22   
#define STAGE_H      13   
#define STAGE_TOP     3   
#define STAGE_GAP     2   
#define PANEL_LEFT   96   
#define REG_TOP      16   

static void inicia_cores(void) {
    start_color();
    use_default_colors();
    init_pair(C_DEFAULT, COLOR_WHITE,   -1);
    init_pair(C_HEADER,  COLOR_CYAN,    -1);
    init_pair(C_STAGE,   COLOR_YELLOW,  -1);
    init_pair(C_ACTIVE,  COLOR_GREEN,   -1);
    init_pair(C_NOP,     COLOR_WHITE,   -1);  // ficara dim
    init_pair(C_SIGNAL,  COLOR_BLUE,    -1);
    init_pair(C_REG,     COLOR_GREEN,   -1);
    init_pair(C_BAR,     COLOR_WHITE,   -1);
    init_pair(C_WARN,    COLOR_RED,     -1);
    init_pair(C_KEY,     COLOR_CYAN,    -1);
}

static void cor(int pair, int attrs) { attron(COLOR_PAIR(pair) | attrs); }
static void descor(int pair, int attrs) { attroff(COLOR_PAIR(pair) | attrs); }

static void hline_at(int y, int x, int w) {
    cor(C_BAR, A_DIM);
    mvhline(y, x, ACS_HLINE, w);
    descor(C_BAR, A_DIM);
}

static void box_at(int y, int x, int h, int w) {
    cor(C_BAR, A_DIM);
    mvaddch(y,     x,     ACS_ULCORNER);
    mvaddch(y,     x+w-1, ACS_URCORNER);
    mvaddch(y+h-1, x,     ACS_LLCORNER);
    mvaddch(y+h-1, x+w-1, ACS_LRCORNER);
    mvhline(y,     x+1, ACS_HLINE, w-2);
    mvhline(y+h-1, x+1, ACS_HLINE, w-2);
    mvvline(y+1,   x,   ACS_VLINE, h-2);
    mvvline(y+1,   x+w-1, ACS_VLINE, h-2);
    descor(C_BAR, A_DIM);
}

static void desenha_seta(int y, int x) {
    cor(C_BAR, A_DIM);
    mvaddch(y, x,   ACS_HLINE);
    mvaddch(y, x+1, ACS_RARROW);
    descor(C_BAR, A_DIM);
}

static void ins_asm(char *out, const typ_ins *ins) {
    if (!ins || ins->instrucao_bruta == 0) {
        strcpy(out, "NOP");
        return;
    }
    asm_gerador_char(out, (typ_ins *)ins);
    out[strcspn(out, "\n")] = '\0';
}

static void desenha_estagio(int y, int x,
                             const char *sigla, const char *nome,
                             const typ_ins *ins, bool valido,
                             const bool sinais[8])
{
    int w = STAGE_W + 2;  // +2 bordas

    box_at(y, x, STAGE_H, w);

    int attr_hdr = valido ? A_BOLD : A_DIM;
    int cpair    = valido ? C_ACTIVE : C_NOP;
    cor(cpair, attr_hdr);
    mvprintw(y, x + 1, " %-*.*s ", w-4, w-4, "");   // limpa
    mvprintw(y + 1, x + 1, " %s  %-*s ", sigla, (int)(w - 7 - strlen(sigla)), nome);
    descor(cpair, attr_hdr);

    cor(C_BAR, A_DIM);
    mvaddch(y+2, x, ACS_LTEE);
    mvhline(y+2, x+1, ACS_HLINE, w-2);
    mvaddch(y+2, x+w-1, ACS_RTEE);
    descor(C_BAR, A_DIM);

    char asm_buf[24] = "---";
    if (valido) ins_asm(asm_buf, ins);
    asm_buf[w - 4] = '\0';   // trunca para caber

    int cpair_ins = (!valido) ? C_NOP :
                    (ins && ins->instrucao_bruta == 0) ? C_NOP : C_ACTIVE;
    int attr_ins  = (!valido || (ins && ins->instrucao_bruta == 0)) ? A_DIM : A_BOLD;
    cor(cpair_ins, attr_ins);
    mvprintw(y + 3, x + 1, " %-*.*s ", w-4, w-4, asm_buf);
    descor(cpair_ins, attr_ins);

    cor(C_BAR, A_DIM);
    mvaddch(y+4, x, ACS_LTEE);
    mvhline(y+4, x+1, ACS_HLINE, w-2);
    mvaddch(y+4, x+w-1, ACS_RTEE);
    descor(C_BAR, A_DIM);

    // ── Sinais de controle ──
    if (sinais && valido && ins && ins->instrucao_bruta != 0) {
        cor(C_SIGNAL, 0);
        mvprintw(y+5, x+1, " RgW:%d MmR:%d MmW:%d", sinais[esc_reg], sinais[mem_reg], sinais[esc_mem]);
        mvprintw(y+6, x+1, " Brc:%d Jmp:%d Src:%d", sinais[branch], sinais[jump], sinais[ula_fon]);
        mvprintw(y+7, x+1, " RgD:%d                ", sinais[reg_des]);
        descor(C_SIGNAL, 0);
    } else {
        cor(C_NOP, A_DIM);
        mvprintw(y+5, x+1, " %-*s ", w-4, "");
        mvprintw(y+6, x+1, " %-*s ", w-4, "");
        mvprintw(y+7, x+1, " %-*s ", w-4, "");
        descor(C_NOP, A_DIM);
    }

    cor(C_BAR, A_DIM);
    mvaddch(y+8, x, ACS_LTEE);
    mvhline(y+8, x+1, ACS_HLINE, w-2);
    mvaddch(y+8, x+w-1, ACS_RTEE);
    descor(C_BAR, A_DIM);
}

//stts
static void desenha_status(int y, int x, const typ_stt *st, int n_ins) {
    float cpi = (st->total_instrucoes > 0)
                ? (float)st->total_ciclos / st->total_instrucoes
                : 0.0f;

    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " METRICAS ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    cor(C_DEFAULT, 0);
    mvprintw(y+1, x, "PC         %4d",    st->pc);
    mvprintw(y+2, x, "Ciclos     %4d",    st->total_ciclos);
    mvprintw(y+3, x, "Concluidas %4d",    st->total_instrucoes);
    mvprintw(y+4, x, "CPI        %4.2f",  cpi);
    mvprintw(y+5, x, "Prog. tam  %4d",    n_ins);

    hline_at(y+6, x, 16);

    mvprintw(y+7,  x, "R-type     %4d",   st->r_instrucoes);
    mvprintw(y+8,  x, "I-type     %4d",   st->i_instrucoes);
    mvprintw(y+9,  x, "J-type     %4d",   st->j_instrucoes);
    mvprintw(y+10, x, "NOP/bubble %4d",   st->nop_instrucoes);
    descor(C_DEFAULT, 0);
}

//reg
static void desenha_registradores(int y, int x, const BancoRegistradores *banco) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " REGISTRADORES ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    for (int i = 0; i < 8; i++) {
        int val = banco->$[i];
        int cpair = (val != 0) ? C_REG : C_NOP;
        int attr  = (val != 0) ? A_BOLD : A_DIM;
        cor(cpair, attr);
        mvprintw(y + 1 + i, x, "$r%d  %6d  (0x%02X)", i,
                 val, (unsigned char)val);
        descor(cpair, attr);
    }
}

//comando
static void desenha_menu(int y, int x) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " COMANDOS ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    struct { const char *key; const char *desc; } cmds[] = {
        { "[1]",     "Carregar arquivo .mem"  },
        { "[2]",     "Ver memorias (terminal)"},
        { "[SPACE]", "Avançar 1 ciclo"        },
        { "[9]",     "Avançar 1 ciclo"        },
        { "[b]",     "Voltar 1 ciclo"         },
        { "[0]",     "Reset completo"         },
        { "[q]",     "Sair"                   },
    };
    int n = (int)(sizeof(cmds)/sizeof(cmds[0]));
    for (int i = 0; i < n; i++) {
        cor(C_KEY, A_BOLD);
        mvprintw(y + 1 + i, x, "%-8s", cmds[i].key);
        descor(C_KEY, A_BOLD);
        cor(C_DEFAULT, 0);
        printw(" %s", cmds[i].desc);
        descor(C_DEFAULT, 0);
    }
}

//leg
static void desenha_legenda(int y, int x) {
    cor(C_HEADER, A_BOLD | A_UNDERLINE);
    mvprintw(y, x, " LEGENDA SINAIS ");
    descor(C_HEADER, A_BOLD | A_UNDERLINE);

    cor(C_SIGNAL, A_DIM);
    mvprintw(y+1, x, "RgW RegWrite  MmR MemRead");
    mvprintw(y+2, x, "MmW MemWrite  Brc Branch ");
    mvprintw(y+3, x, "Jmp Jump      Src AluSrc ");
    mvprintw(y+4, x, "RgD RegDst                ");
    descor(C_SIGNAL, A_DIM);
}

static void atualiza_tela(const typ_stt *st, int n_ins) {
    clear();

    cor(C_HEADER, A_BOLD);
    mvprintw(0, 0, " Mini Mips - Pipeline ");
    descor(C_HEADER, A_BOLD);
    cor(C_DEFAULT, A_DIM);
   // mvprintw(0, 37, "UNIPAMPA");
    //descor(C_DEFAULT, A_DIM);
    //hline_at(1, 0, COLS);

    const char *fases[] = {
        "1 FETCH", "2 DECODE", "3 EXECUTE", "4 MEMORY", "5 WRITEBACK"
    };
    // Cada caixa tem w = STAGE_W+2 = 20; gap entre elas = STAGE_GAP+2 = 3
    int step = STAGE_W + 2 + STAGE_GAP + 2;   

    for (int s = 0; s < 5; s++) {
        int cx = s * step;
        cor(C_STAGE, A_BOLD);
        mvprintw(STAGE_TOP - 1, cx + 1, "%s", fases[s]);
        descor(C_STAGE, A_BOLD);
    }

  
    struct {
        const char     *sigla;
        const char     *nome;
        const typ_ins  *ins;
        bool            valido;
        const bool     *sinais;
    } stages[5] = {
        { "IF",  "Fetch",     &st->instrucao_t[st->pc], (st->pc < 256),    NULL              },
        { "ID",  "Decode",    &st->IF_ID.instrucao,     st->IF_ID.valido,  st->ID_EX.sinal   },
        { "EX",  "Execute",   &st->ID_EX.instrucao,     st->ID_EX.valido,  st->ID_EX.sinal   },
        { "MEM", "Memory",    &st->EX_MEM.instrucao,    st->EX_MEM.valido, st->EX_MEM.sinal  },
        { "WB",  "Writeback", &st->MEM_WB.instrucao,    st->MEM_WB.valido, st->MEM_WB.sinal  },
    };

    for (int s = 0; s < 5; s++) {
        int cx = s * step;
        desenha_estagio(
            STAGE_TOP, cx,
            stages[s].sigla, stages[s].nome,
            stages[s].ins,  stages[s].valido,
            stages[s].sinais
        );
        // Seta entre estagios
        if (s < 4) desenha_seta(STAGE_TOP + STAGE_H/2, cx + STAGE_W + 2);
    }

    int px = 5 * step + 2;
    desenha_status(STAGE_TOP, px, st, n_ins);
    desenha_legenda(STAGE_TOP + 12, px);

    int bot = STAGE_TOP + STAGE_H + 1;
    hline_at(bot, 0, COLS);
    desenha_registradores(bot + 1, 0, st->registradores);
    desenha_menu(bot + 1, 28);

    hline_at(LINES - 2, 0, COLS);
    cor(C_KEY, A_BOLD);
   // mvprintw(LINES - 1, 0, "▶ ");
    //descor(C_KEY, A_BOLD);
    cor(C_DEFAULT, 0);
    printw("Insira: ");
    descor(C_DEFAULT, 0);

    refresh();
}

static void imprime_memorias(const typ_stt *st, int n_ins) {
    endwin();

    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  MEMORIA DE INSTRUCOES                                       ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    for (int i = 0; i < n_ins; i++) {
        char asm_buf[30];
        ins_asm(asm_buf, &st->instrucao_t[i]);
        printf("║ [%03d]  %-16s  %-28s  ║\n",
               i, st->instrucao_t[i].total, asm_buf);
    }
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  MEMORIA DE DADOS                                            ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    for (int i = 0; i < 256; i += 4) {
        printf("║ [%03d]: %4d  [%03d]: %4d  [%03d]: %4d  [%03d]: %4d         ║\n",
               i,   st->mem_dados->dados[i],
               i+1, st->mem_dados->dados[i+1],
               i+2, st->mem_dados->dados[i+2],
               i+3, st->mem_dados->dados[i+3]);
    }
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    printf("\nPressione ENTER para voltar...\n");
    while (getchar() != '\n');

    initscr();
    inicia_cores();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

int main(void)
{
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

    typ_instrucaoSep *memoria_instrucoes = NULL;
    int num_instrucoes = 0;

    initscr();
    inicia_cores();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int ch;
    bool sair = false;

    while (!sair) {
        atualiza_tela(&estado, num_instrucoes);
        ch = getch();

        switch (ch) {
            case '1': {
                // Captura nome do arquivo
                echo();
                curs_set(1);
                mvprintw(LINES - 1, 0, "Arquivo .mem: ");
                clrtoeol();
                char nome[64];
                getnstr(nome, sizeof(nome) - 1);
                noecho();
                curs_set(0);

                if (memoria_instrucoes) free(memoria_instrucoes);
                num_instrucoes = carregar_memoria_instrucoes(nome, &memoria_instrucoes);
                estado.instrucao_t = (typ_ins *)memoria_instrucoes;
                estado.pc = 0;
                break;
            }
            case '2':
                imprime_memorias(&estado, num_instrucoes);
                break;

            case '9':
            case ' ':
                push_estagio(&estado);
                executar(&estado, banco, false);
                break;

            case 'b':
            case 'B':
                if (estado.topo_pilha > 0) pop_estagio(&estado);
                break;

            case '0':
                estado.pc = 0;
                estado.total_ciclos   = 0;
                estado.total_instrucoes = 0;
                estado.r_instrucoes   = 0;
                estado.i_instrucoes   = 0;
                estado.j_instrucoes   = 0;
                estado.nop_instrucoes = 0;
                inicia_registradores(&banco);
                memset(mem_dados.dados, 0, sizeof(mem_dados.dados));
                memset(&estado.IF_ID,  0, sizeof(estado.IF_ID));
                memset(&estado.ID_EX,  0, sizeof(estado.ID_EX));
                memset(&estado.EX_MEM, 0, sizeof(estado.EX_MEM));
                memset(&estado.MEM_WB, 0, sizeof(estado.MEM_WB));
                estado.topo_pilha = 0;
                break;

            case 'q':
            case 'Q':
                sair = true;
                break;
        }
    }

    endwin();
    if (memoria_instrucoes) free(memoria_instrucoes);
    if (estado.pilha_back)  free(estado.pilha_back);
    return 0;
}