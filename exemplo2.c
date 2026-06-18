#include <panel.h>
//#include <cstring>
//#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _PANEL_DATA {
  int x, y, w, h;
  char label[80]; 
  int label_color;
  PANEL *next;
}PANEL_DATA;

#define NLINES 10
#define NCOLS 40

void init_wins(WINDOW ** wins, int n);
void win_show(WINDOW * win, char * label, int label_color);
void print_in_middle(WINDOW * win, int starty, int startx, int width, char * string, chtype color);
void set_user_ptrs(PANEL ** panels, int n);

int main(){
  WINDOW * my_wins[3];
  PANEL  * my_panels[3];
  PANEL_DATA  * top;
  PANEL * stack_top;
  WINDOW * temp_win, * old_win;
  int ch;
  int newx, newy, neww, newh;
  int size = FALSE, ismove = FALSE;

  // Inicializa curses
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  // Inicializa todas as cores
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);

  init_wins(my_wins, 3);

  // Anexa um painel a cada janela
  // A ordem é de baixo para cima
  my_panels[0] = new_panel (my_wins[0]);   // Move 0, ordem: stdscr-0
  my_panels[1] = new_panel (my_wins[1]);   // Move 1, ordem: stdscr-0-1
  my_panels[2] = new_panel (my_wins[2]);   // Move 2, ordem: stdscr-0-1-2

  set_user_ptrs(my_panels, 3);
  // Atualiza a ordem de empilhamento. O 2º painel ficará no topo
  update_panels();

  // Mostra-o na tela
  //attron(COLOR_PAIR(4));
  //mvprintw (NLINES - 3, 0, "Use 'm' para mover, 'r' para redimensionar");
  //mvprintw (NLINES - 2, 0, "Use tab para navegar pelas janelas (F1 para sair)");
  //attroff(COLOR_PAIR(4));
  doupdate();

  stack_top = my_panels[2];
  top = (PANEL_DATA *)panel_userptr(stack_top);
  newx = top->x;
  newy = top->y;
  neww = top->w;
  newh = top->h;
  while((ch = getch()) != KEY_F(1)){
    switch(ch){
      case 9:         // Tab 
        top = (PANEL_DATA *)panel_userptr(stack_top);
        top_panel(top->next);
        stack_top = top->next;
        top = (PANEL_DATA *)panel_userptr(stack_top);
        newx = top->x;
        newy = top->y;
        neww = top->w;
        newh = top->h;
        break;
      case 'r':       // Redimensionar
        size = TRUE;
        //attron(COLOR_PAIR(4));
        mvprintw (20, 0, "Redimensionamento inserido: Use as setas para redimensionar e pressione <ENTER> para encerrar o redimensionamento ");
        refresh();
        //attroff(COLOR_PAIR(4));
        break;
      case 'm':       // Move 
        //attron(COLOR_PAIR(4));
        mvprintw (21, 0, "Movimento inserido: Use as setas para mover e pressione <ENTER> para parar de se mover ");
        refresh();
        //attroff(COLOR_PAIR(4));
        ismove = TRUE;
        break;
      case KEY_LEFT:
        if(size == TRUE){
          --newx;
          ++neww;
        }
        if(ismove == TRUE)
          --newx;
        break;
      case KEY_RIGHT:
        if(size == TRUE){
          ++newx;
          --neww;
        }
        if(ismove == TRUE){
          ++newx;
        }
        break;
      case KEY_UP:
        if(size == TRUE){
          --newy;
          ++newh;
        }
        if(ismove == TRUE){
          --newy;
        }
        break;
      case KEY_DOWN:
        if(size == TRUE){
          ++newy;
          --newh;
        }
        if(ismove == TRUE){
          ++newy;
        }
        break;
      case 10: // ENTER
        //move(NLINES - 4, 0);
        clrtoeol();
        refresh();
        if(size == TRUE){
          old_win = panel_window(stack_top);
          temp_win = newwin(newh, neww, newy, newx);
          replace_panel(stack_top, temp_win);
          win_show(temp_win, top->label, top->label_color); 
          delwin(old_win);
          size = FALSE;
        }
        if(ismove == TRUE){
          move_panel(stack_top, newy, newx);
          ismove = FALSE;
        }
        break;

    }
    //attron(COLOR_PAIR(4));
    mvprintw (22, 0, "Use 'm' para mover, 'r' para redimensionar");
    mvprintw (23, 0, "Use tab para navegar pelas janelas (F1 para sair)");
    //attroff(COLOR_PAIR(4));
    refresh();      
    update_panels();
    doupdate();
  }
  endwin();
  return 0;
}

// Coloca todas as janelas
void init_wins(WINDOW **wins, int n){
  int x, y, i;
  char label[80];

  y = 2;
  x = 10;
  for(i = 0; i < n; ++i){
    wins[i] = newwin(NLINES, NCOLS, y, x);
    sprintf(label, "Janela %d", i + 1);
    win_show(wins[i], label, i + 1);
    y += 3;
    x += 7;
  }
}

// Define as estruturas PANEL_DATA para painéis individuais
void set_user_ptrs(PANEL **panels, int n){
  PANEL_DATA *ptrs;
  WINDOW *win;
  int x, y, w, h, i;
  char temp[80];

  ptrs = (PANEL_DATA *)calloc(n, sizeof(PANEL_DATA));

  for(i = 0;i < n; ++i){
    win = panel_window(panels[i]);
    getbegyx(win, y, x);
    getmaxyx(win, h, w);
    ptrs[i].x = x;
    ptrs[i].y = y;
    ptrs[i].w = w;
    ptrs[i].h = h;
    sprintf(temp, "JANELA %d", i + 1);
    strcpy(ptrs[i].label, temp);
    ptrs[i].label_color = i + 1;
    if(i + 1 == n)
      ptrs[i].next = panels[0];
    else
      ptrs[i].next = panels[i + 1];
    set_panel_userptr(panels[i], &ptrs[i]);
  }
}

// Mostra a janela com uma borda e um rótulo
void win_show(WINDOW *win, char *label, int label_color){
  int startx, starty, height, width;

  getbegyx(win, starty, startx);
  getmaxyx(win, height, width);

  box(win, 0, 0);
  mvwaddch(win, 2, 0, ACS_LTEE); 
  mvwhline(win, 2, 1, ACS_HLINE, width - 2); 
  mvwaddch(win, 2, width - 1, ACS_RTEE); 

  print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color){
  int length, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if(startx != 0)
    x = startx;
  if(starty != 0)
    y = starty;
  if(width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}