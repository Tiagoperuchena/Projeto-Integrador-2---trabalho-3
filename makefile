APP = simulador


SRC = EX.c asm_gerador.c asm2_gerador.c controlador.c executar.c ID.c IF.c leitura_de_comandos.c main.c MEM.c memoria_de_dados.c pilha.c registradores.c ULA.c WB.c 


all:
	gcc $(SRC) -o $(APP) -lncurses


clear:
	rm -f $(APP)

gdb:
	gcc $(SRC) -g -o $(APP)