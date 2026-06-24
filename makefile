SRC_DIR = src
INC_DIR = include
BUILD_DIR = build


APP = $(BUILD_DIR)/simulador
APP_CLI = $(BUILD_DIR)/simulador_cli


CFLAGS = -I$(INC_DIR) -Wall


SRC_COMMON = $(SRC_DIR)/EX.c $(SRC_DIR)/asm_gerador.c $(SRC_DIR)/asm2_gerador.c \
             $(SRC_DIR)/controlador.c $(SRC_DIR)/executar.c $(SRC_DIR)/ID.c \
             $(SRC_DIR)/IF.c $(SRC_DIR)/leitura_de_comandos.c $(SRC_DIR)/MEM.c \
             $(SRC_DIR)/memoria_de_dados.c $(SRC_DIR)/pilha.c \
             $(SRC_DIR)/registradores.c $(SRC_DIR)/ULA.c $(SRC_DIR)/WB.c 

SRC = $(SRC_COMMON) $(SRC_DIR)/main.c
SRC_CLI = $(SRC_COMMON) $(SRC_DIR)/simulador.c


all: ncurses cli


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

ncurses: $(BUILD_DIR)
	gcc $(SRC) $(CFLAGS) -o $(APP) -lncurses

cli: $(BUILD_DIR)
	gcc $(SRC_CLI) $(CFLAGS) -o $(APP_CLI)

clear:
	rm -f $(APP) $(APP_CLI)
	

gdb: $(BUILD_DIR)
	gcc $(SRC) $(CFLAGS) -g -o $(APP) -lncurses
	gcc $(SRC_CLI) $(CFLAGS) -g -o $(APP_CLI)