#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

void draw_button(WINDOW *win, int y, int x, const char *label, int is_selected);

int main(){
initscr();
//noecho();
//cbreak(); 
curs_set(0); //habilita o uso do mouse

int linhas,colunas,meioColunas, meioLinhas, inicioLinhaBox, inicioColunaBox;
getmaxyx(stdscr,linhas,colunas); //obtem o tamanho da tela aberta
meioLinhas 		= 18;
meioColunas 	= colunas/2;
inicioLinhaBox 	= linhas/4; 
inicioColunaBox = colunas/4;
//newwin (y,x,y,x) -> (alturaY, larguraX, inicio do y, inicio do X)
WINDOW *minhaJanela = newwin(meioLinhas,meioColunas,inicioLinhaBox,inicioColunaBox);
keypad(minhaJanela,TRUE); //habilita o uso do teclado
//wprintw(minhaJanela,"%d linhas, %d colunas",linhas,colunas);
	
	int destacado=1, choice=-1;
	
	do{
		
	wrefresh(minhaJanela);
	werase(minhaJanela);
	box(minhaJanela, 0, 0); //cria uma borda na tela criada na window
	/* Mostra aquela caixa 	*/
	//bkgd(COLOR_PAIR(0)); //background da página
	wattron(minhaJanela,A_BLINK | A_BOLD); //italico e piscando ligado
	mvwprintw(minhaJanela,1,inicioColunaBox-(strlen("==============SIMULADOR MINI-MIPS UNIPAMPA==============")/2),"==============SIMULADOR MINI-MIPS UNIPAMPA=============="); //imprime na janela no ponto (y,x) vertical e horizontal.
	wattroff(minhaJanela,A_BLINK | A_BOLD); //italico e piscando desligado
	//apresenta menu iterativo
	mvwprintw(minhaJanela,3,(inicioColunaBox-(strlen("Menu principal:")/2)),"Menu principal:");
	draw_button(minhaJanela,4,inicioColunaBox-(strlen("1. Carregar memória")/2),"1. Carregar memória",(destacado==1));
	draw_button(minhaJanela,5,inicioColunaBox-(strlen("2. Carregar Memória de Dados")/2),"2. Carregar Memória de Dados",(destacado==2));
	draw_button(minhaJanela,6,inicioColunaBox-(strlen("3. Imprimir memórias")/2),"3. Imprimir memórias",(destacado==3));
	draw_button(minhaJanela,7,inicioColunaBox-(strlen("4. Imprimir registradores")/2),"4. Imprimir registradores",(destacado==4));
	draw_button(minhaJanela,8,inicioColunaBox-(strlen("5. Imprimir todo o simulador")/2),"5. Imprimir todo o simulador",(destacado==5));
	draw_button(minhaJanela,9,inicioColunaBox-(strlen("6. Salvar .asm")/2),"6. Salvar .asm",(destacado==6));
	draw_button(minhaJanela,10,inicioColunaBox-(strlen("7. Salvar memória de dados .dat")/2),"7. Salvar memória de dados .dat",(destacado==7));
	draw_button(minhaJanela,11,inicioColunaBox-(strlen("8. Executa Programa (run)")/2),"8. Executa Programa (run)",(destacado==8));
	draw_button(minhaJanela,12,inicioColunaBox-(strlen("9. Executa uma instrução (Step)")/2),"9. Executa uma instrução (Step)",(destacado==9));
	draw_button(minhaJanela,13,inicioColunaBox-(strlen("10. Volta uma instrução (Back)")/2),"10. Volta uma instrução (Back)",(destacado==10));
	draw_button(minhaJanela,14,inicioColunaBox-(strlen("11. Reinicia PC")/2),"11. Reinicia PC",(destacado==11));
	wattron(minhaJanela,A_BLINK | A_BOLD);
	mvwprintw(minhaJanela,15,inicioColunaBox-(strlen("===========================================================")/2),"===========================================================");
	mvwprintw(minhaJanela,16,inicioColunaBox-(strlen("Escolha sua opção [w-pra cima s-pra baixo] e aperte Enter")/2),"Escolha sua opção [w-pra cima s-pra baixo] e aperte Enter");
	wattroff(minhaJanela,A_BLINK | A_BOLD);
	
        // Atualiza a janela do menu
		//wgetch(minhaJanela); //para para pegar um caractere e dar tempo de ver o resultado

        char ch = wgetch(minhaJanela); // Obtém a entrada do usuário

        switch(ch) {
            case 'w':
                if (destacado > 1) {
                    destacado--;
                }
                break;
            case 's':
                if (destacado < 11) {
                    destacado++;
                }
                break;
            case '\n':
                choice = destacado;
                break;
        }
		wrefresh(minhaJanela);
		refresh();
    } while (choice == -1);

    // Processa a escolha do usuário
    switch (choice) {
        case 0:
            mvprintw(LINES - 2, 0, "saindo.....");
            break;
        case 1:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 1.");
            break;
        case 2:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 2.");
            break;
        case 3:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 3.");
            break;
        case 4:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 4.");
            break;
        case 5:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 5.");
            break;
        case 6:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 6.");
            break;
        case 7:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 7.");
            break;
        case 8:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 8.");
            break;
        case 9:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 9.");
            break;
        case 10:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 10.");
            break;
        case 11:
            mvprintw(LINES - 2, 0, "Você escolheu a opção 11.");
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


void draw_button(WINDOW *minhaJanela, int y, int x, const char *label, int is_selected) {
    if (is_selected==1) {
        wattron(minhaJanela, A_REVERSE);
    }
    mvwprintw(minhaJanela, y, x, "[%s]", label);
    wattroff(minhaJanela, A_REVERSE);
    wrefresh(minhaJanela);
}