CC=gcc
CFLAGS=-lm -Wall -pedantic-errors -Wall -Wextra -O2 -std=gnu11 -I include/

cplot: src/plotter.c src/parser.c src/tokenizer.c src/main.c
	$(CC) -o cplot src/plotter.c src/parser.c src/tokenizer.c src/main.c $(CFLAGS)
