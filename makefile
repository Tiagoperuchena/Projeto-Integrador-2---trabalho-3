APP = simulador
APP_CLI = simulador_cli

SRC_COMMON = EX.c asm_gerador.c asm2_gerador.c controlador.c executar.c ID.c IF.c leitura_de_comandos.c MEM.c memoria_de_dados.c pilha.c registradores.c ULA.c WB.c 
SRC = $(SRC_COMMON) main.c
SRC_CLI = $(SRC_COMMON) simulador.c

all: ncurses cli

ncurses:
	gcc $(SRC) -o $(APP) -lncurses

cli:
	gcc $(SRC_CLI) -o $(APP_CLI)

clear:
	rm -f $(APP) $(APP_CLI)

gdb:
	gcc $(SRC) -g -o $(APP)
	gcc $(SRC_CLI) -g -o $(APP_CLI)