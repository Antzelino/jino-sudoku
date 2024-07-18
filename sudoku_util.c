#include <stdio.h>
#include <assert.h>
#include "sudoku_util.h"

static void print_top_line_bold() {
    printf("┏━━━┯━━━┯━━━┳━━━┯━━━┯━━━┳━━━┯━━━┯━━━┓\n");
}

static void print_middle_line_bold()
{
    printf("┣━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┫\n");
}

static void print_middle_line_light()
{
    printf("┠┈┈┈┼┈┈┈┼┈┈┈╂┈┈┈┼┈┈┈┼┈┈┈╂┈┈┈┼┈┈┈┼┈┈┈┨\n");
}

static void print_bottom_line_bold()
{
    printf("┗━━━┷━━━┷━━━┻━━━┷━━━┷━━━┻━━━┷━━━┷━━━┛\n");
    printf(" \n");
}

static void print_board_cell_value(unsigned short int value) {
    if (value == 0)
    {
        printf("  ");
    }
    else 
    {
        printf("%hu ", value);
    }
}

static void validate_board_values(unsigned short int board[9][9]) {
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            assert(board[i][j] <= 9 && "Invalid value in sudoku board, cannot print");
        }
    }
}

static void print_line_with_values(unsigned short int board[9][9], size_t i) {
    for (size_t j = 0; j < 9; j++)
    {
        if (j % 3 == 0)
        {
            printf("┃ ");
        }
        else
        {
            printf("┊ ");
        }
        
        print_board_cell_value(board[i][j]);
    }
    printf("┃\n");
}

// -- PUBLIC -- //
void print_sudoku(unsigned short int board[9][9])
{
    validate_board_values(board);

    for (size_t i = 0; i < 9; i++)
    {
        if (i == 0)
        {
            print_top_line_bold();
        }
        else if (i % 3 == 0)
        {
            print_middle_line_bold();
        }
        else
        {
            print_middle_line_light();
        }

        print_line_with_values(board, i);
    }
    print_bottom_line_bold();
}
