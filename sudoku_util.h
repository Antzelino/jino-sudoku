#ifndef SUDOKU_UTIL_H
#define SUDOKU_UTIL_H

#include <stdio.h>

void print_sudoku(unsigned short int board[9][9]) {
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if(board[i][j] > 9) {
                fprintf(stderr, "INVALID: Sudoku contains invalid value \'%hu\' at [%zu][%zu]. Cannot print board.\n", board[i][j], i, j);
                return;
            }
        }
    }

    for (size_t i = 0; i < 9; i++)
    {
        if (i == 0)
        {
            printf("╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n");
        }
        else if (i % 3 == 0) {
            printf("╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣░\n");
        }
        else {
            printf("╟───┼───┼───╫───┼───┼───╫───┼───┼───╢░\n");
        }
        for (size_t j = 0; j < 9; j++)
        {
            if (j % 3 == 0)
            {
                printf("║ ");
            }
            else {
                printf("│ ");
            }

            if (board[i][j] == 0)
            {
                printf("  ");
            }
            else 
            {
                printf("%hu ", board[i][j]);
            }
        }
        printf("║░\n");
    }
    printf("╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝░\n");
    printf(" ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n");
}

#endif // SUDOKU_UTIL_H
