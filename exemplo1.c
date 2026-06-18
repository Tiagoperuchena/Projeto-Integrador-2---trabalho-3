#include <ncurses.h>

#define MENU_HEIGHT 6
#define MENU_WIDTH 20

void draw_button(WINDOW *win, int y, int x, const char *label, bool is_selected) {
    if (is_selected) {
        wattron(win, A_REVERSE);
    }
    mvwprintw(win, y, x, "[%s]", label);
    wattroff(win, A_REVERSE);
}

int main() {
    initscr(); // Inicializa a tela
    curs_set(0); // Esconde o cursor

    // Cria a janela do menu
    WINDOW *menu_win = newwin(MENU_HEIGHT, MENU_WIDTH, (LINES - MENU_HEIGHT) / 2, (COLS - MENU_WIDTH) / 2);

    // Habilita a detecção de teclas especiais
    keypad(menu_win, TRUE);

    int choice = -1;
    int highlight = 0;
    int ch;
    do {
        // Limpa a janela do menu
        werase(menu_win);

        // Desenha os botões do menu
        draw_button(menu_win, 1, 1, "Opção 1", (highlight == 0));
        draw_button(menu_win, 2, 1, "Opção 2", (highlight == 1));
        draw_button(menu_win, 3, 1, "Opção 3", (highlight == 2));
        draw_button(menu_win, 4, 1, "Sair", (highlight == 3));

        // Atualiza a janela do menu
        wrefresh(menu_win);

        ch = wgetch(menu_win); // Obtém a entrada do usuário

        switch(ch) {
            case KEY_UP:
                if (highlight > 0) {
                    highlight--;
                }
                break;
            case KEY_DOWN:
                if (highlight < 3) {
                    highlight++;
                }
                break;
            case '\n':
                choice = highlight;
                break;
        }
    } while (choice == -1);

    // Processa a escolha do usuário
    switch (choice) {
        case 0:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 1.");
            break;
        case 1:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 2.");
            break;
        case 2:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 3.");
            break;
        case 3:
            mvprintw(LINES - 2, 0, "Saindo...");
            break;
    }

    // Atualiza a tela principal
    refresh();

    // Aguarda o pressionamento de uma tecla para continuar
    getch();

    // Finaliza a janela ncurses
    endwin();

    return 0;
}