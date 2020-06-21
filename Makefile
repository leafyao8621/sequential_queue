CC = gcc
SRC = $(wildcard src/*/*.c) src/main.c
OBJ = $(SRC:.c=.o)
BIN = main

%.o: %.c
	$(CC) -g -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) -pthread -lm

.PHONY: clean
clean: $(BIN)
	@rm $(OBJ) $(BIN)
