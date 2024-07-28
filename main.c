#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "example_sudokus.h"
#include "sudoku_util.h"
#include "sudoku_solver.h"

unsigned short int sudoku_board[9][9] = {0};

int main(int argc, char const *argv[])
{
    assert(argc == 1 && "Not supported command line arguments");
    assert(argv[1] == 0 && "Not supported command line arguments");

    printf("Trying to solve this sudoku:\n");
    print_sudoku(med1);
    unsigned short int solved[9][9] = {0};
    solve_sudoku(med1, solved);
    sleep(1);
    printf("\nResult after solve attempt:\n");
    print_sudoku(solved);
    
    sleep(1);
    printf("\nCompare with real solution:\n");
    print_sudoku(med1_sol);
    
    return 0;
}
