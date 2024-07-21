CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99
DEPS = example_sudokus.h sudoku_util.h sudoku_solver.h
OBJ = main.o sudoku_solver.o sudoku_util.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sudoku: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean run

clean:
	rm -f *.o sudoku

run: sudoku
	./sudoku
