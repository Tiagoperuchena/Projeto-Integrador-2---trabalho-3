APP = simulador


SRC = executar.c controlador.c asm_gerador.c leitura_de_comandos.c main.c memoria_de_dados.c registradores.c ULA.c asm2_gerador.c


all:
	gcc $(SRC) -o $(APP)


clear:
	rm -f $(APP)