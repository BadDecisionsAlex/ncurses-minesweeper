all: minesweeper

minesweeper: main.c field.c
	gcc -o minesweeper -lncurses main.c field.c
