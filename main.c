#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "example_sudokus.h"
#include "sudoku_util.h"
#include "sudoku_solver.h"

unsigned short int sudoku_board[9][9] = {0};

void print_sudoku(unsigned short int board[9][9]);

int main(int argc, char const *argv[])
{
    assert(argc == 1 && "Not supported command line arguments");
    assert(argv[1] == 0 && "Not supported command line arguments");

    printf("hello, sudoku\n");
    print_sudoku(easy1);
    unsigned short int solved[9][9] = {0};
    solve_sudoku(easy1, solved);
    sleep(1);
    printf("\nafter some steps to solve it:\n");
    print_sudoku(solved);
    
    sleep(1);
    printf("\ncompare with real solution:\n");
    print_sudoku(easy1_sol);
    
    return 0;
}
