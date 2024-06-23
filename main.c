#include <stdio.h>
#include "example_sudokus.h"
#include "sudoku_util.h"

unsigned short int sudoku_board[9][9] = {
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0}
};

void print_sudoku(unsigned short int board[9][9]);

int main(int argc, char const *argv[])
{
    printf("hello, sudoku\n");
    print_sudoku(easy1);
    return 0;
}
