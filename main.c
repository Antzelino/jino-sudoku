#include <stdio.h>
#include "example_sudokus.h"

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
    print_sudoku(easy1_sol);
    return 0;
}

void print_sudoku(unsigned short int board[9][9]) {
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if(board[i][j] < 1 || board[i][j] > 9) {
                fprintf(stderr, "INVALID: Sudoku contains invalid value \'%hu\' at [%zu][%zu]. Cannot print board.\n", board[i][j], i, j);
                return;
            }
        }
    }

    short printing_line = 1;
    for (size_t i = 0; i < 9; i++)
    {
        printf("+---+---+---+---+---+---+---+---+---+\n");
        for (size_t j = 0; j < 9; j++)
        {
            printf("| %hu ", board[i][j]);
        }
        printf("|\n");
    }
    printf("+---+---+---+---+---+---+---+---+---+\n");
}