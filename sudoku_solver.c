#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "sudoku_solver.h"

#define SUDOKU_MAX_ITERATIONS 10

/*
 * Each element value from 1 to 9.
 * If 0 means that value is not possible in this cell.
 */
typedef struct Notes {
    unsigned short int values[9];
} Notes;

Notes possible_values[9][9];

void init_possible_values() {
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            for (size_t k = 0; k < 9; k++)
            {
                possible_values[i][j].values[k] = k + 1;
            }
        }
    }
}

void set_only_possible_number_in_cell(unsigned short int n, unsigned short int i, unsigned short int j) {
    for (unsigned short int n_ = 0; n_ < 9; n_++)
    {
        if (n_ == n-1)
        {
            assert(possible_values[i][j].values[n_] != 0 && "Cell should have value, not 0");
            assert(possible_values[i][j].values[n_] == n && "Cell has wrong value in index");
            continue;
        }
        possible_values[i][j].values[n_] = 0;
    }
}

void clear_number_from_3x3(unsigned short int n, unsigned short int i, unsigned short int j) {
    unsigned short int i_start, i_end, j_start, j_end;
    i_start = (i / 3) * 3;
    i_end = i_start + 2;
    j_start = (j / 3) * 3;
    j_end = j_start + 2;
    
    assert((i_start == 0 || i_start == 3 || i_start == 6) && "Calculation for indices in 3x3 are wrong");
    assert((j_start == 0 || j_start == 3 || j_start == 6) && "Calculation for indices in 3x3 are wrong");
    assert((i_end == 2 || i_end == 5 || i_end == 8) && "Calculation for indices in 3x3 are wrong");
    assert((j_end == 2 || j_end == 5 || j_end == 8) && "Calculation for indices in 3x3 are wrong");

    for (size_t i_ = i_start; i_ <= i_end; i_++)
    {
        for (size_t j_ = j_start; j_ <= j_end; j_++)
        {
            if (i == i_ && j == j_)
            {
                assert(possible_values[i][j].values[n-1] != 0 && "Cell should have value, not 0");
                assert(possible_values[i][j].values[n-1] == n && "Cell has wrong value in index");
                continue;
            }
            possible_values[i_][j_].values[n-1] = 0;
        }
    }
}

void clear_number_from_column(unsigned short int n, unsigned short int i, unsigned short int j) {
    for (size_t i_ = 0; i_ < 9; i_ ++)
    {
        if (i_ == i)
        {
            assert(possible_values[i][j].values[n-1] != 0 && "Cell should have value, not 0");
            assert(possible_values[i][j].values[n-1] == n && "Cell has wrong value in index");
            continue;
        }
        possible_values[i_][j].values[n-1] = 0;
    }
}

void clear_number_from_row(unsigned short int n, unsigned short int i, unsigned short int j) {
    for (size_t j_ = 0; j_ < 9; j_ ++)
    {
        if (j_ == j)
        {
            assert(possible_values[i][j].values[n-1] != 0 && "Cell should have value, not 0");
            assert(possible_values[i][j].values[n-1] == n && "Cell has wrong value in index");
            continue;
        }
        possible_values[i][j_].values[n-1] = 0;
    }
}

void handle_number_in_cell(unsigned short int n, unsigned short int i, unsigned short int j) {
    assert(n != 0 && "Must not set number in cell to 0");
    clear_number_from_row(n, i, j);
    clear_number_from_column(n, i, j);
    clear_number_from_3x3(n, i, j);
    set_only_possible_number_in_cell(n, i, j);
}

int only_one_possible_value_in_cell(unsigned short int i, unsigned short int j) {
    unsigned short int possible_value = 0;
    for (size_t n_ = 0; n_ < 9; n_++)
    {
        if (possible_value == 0)
        {
            possible_value = possible_values[i][j].values[n_];
        }
        else if (possible_values[i][j].values[n_] != 0)
        {
            return 0;
        }
    }
    return possible_value;
}

void check_hidden_singles() {
    // First check for each value in each row:
    for (unsigned short n = 1; n <= 9; n++)
    {
        for (unsigned short i = 0; i < 9; i++)
        {
            unsigned short row_n_count = 0;
            short row_index = -1;

            for (unsigned short j = 0; j < 9; j++)
            {
                if (possible_values[i][j].values[n-1] != 0)
                {
                    row_n_count++;
                    row_index = j;
                }
            }
            
            if (row_n_count == 1)
            {
                assert(row_index >= 0 && row_index < 9 && "Row index not in valid range");
                handle_number_in_cell(n, i, row_index);
            }
        }
    }
    // Then check for each value in each column
    for (unsigned short n = 1; n <= 9; n++)
    {
        for (unsigned short j = 0; j < 9; j++)
        {
            unsigned short column_n_count = 0;
            short column_index = -1;

            for (unsigned short i = 0; i < 9; i++)
            {
                if (possible_values[i][j].values[n-1] != 0)
                {
                    column_n_count++;
                    column_index = i;
                }
            }
            
            if (column_n_count == 1)
            {
                assert(column_index >= 0 && column_index < 9 && "Column index not in valid range");
                handle_number_in_cell(n, column_index, j);
            }
        }
    }
    // Lastly check for each value in each 3x3 box
    for (unsigned short n = 1; n <= 9; n++)
    {
        for (unsigned short box_center_i = 1; box_center_i < 9; box_center_i+=3)
        {
            for (unsigned short box_center_j = 1; box_center_j < 9; box_center_j+=3)
            {
                unsigned short box3x3_n_count = 0;
                short n_index_i = -1;
                short n_index_j = -1;

                unsigned short int i_start, i_end, j_start, j_end;
                i_start = (box_center_i / 3) * 3;
                i_end = i_start + 2;
                j_start = (box_center_j / 3) * 3;
                j_end = j_start + 2;

                // Iterating the 9 cells in this 3x3 box
                for (unsigned short i_ = i_start; i_ <= i_end; i_++)
                {
                    for (unsigned short j_ = j_start; j_ <= j_end; j_++)
                    {
                        if (possible_values[i_][j_].values[n-1] != 0)
                        {
                            box3x3_n_count++;
                            n_index_i = i_;
                            n_index_j = j_;
                        }
                    }
                }
                if (box3x3_n_count == 1) {
                    assert(n_index_i >= 0 && n_index_i < 9 && "Column index not in valid range");
                    assert(n_index_j >= 0 && n_index_j < 9 && "Column index not in valid range");
                    handle_number_in_cell(n, n_index_i, n_index_j);
                }
            }
        }
    }
}

/*
 * Return 0 if is not fully solved, >0 otherwise.
 */
int is_not_fully_solved(unsigned short int final_board[9][9]) {
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if (final_board[i][j] == 0) {
                return 0;
            }
        }
    }
    
    return 1;
}

void print_possible_values_table() {
    printf("┏━━━━━┯━━━━━┯━━━━━┳━━━━━┯━━━━━┯━━━━━┳━━━━━┯━━━━━┯━━━━━┓\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[0][0].values[0], possible_values[0][0].values[1], possible_values[0][0].values[2], possible_values[0][1].values[0], possible_values[0][1].values[1], possible_values[0][1].values[2], possible_values[0][2].values[0], possible_values[0][2].values[1], possible_values[0][2].values[2], possible_values[0][3].values[0], possible_values[0][3].values[1], possible_values[0][3].values[2], possible_values[0][4].values[0], possible_values[0][4].values[1], possible_values[0][4].values[2], possible_values[0][5].values[0], possible_values[0][5].values[1], possible_values[0][5].values[2], possible_values[0][6].values[0], possible_values[0][6].values[1], possible_values[0][6].values[2], possible_values[0][7].values[0], possible_values[0][7].values[1], possible_values[0][7].values[2], possible_values[0][8].values[0], possible_values[0][8].values[1], possible_values[0][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[0][0].values[3], possible_values[0][0].values[4], possible_values[0][0].values[5], possible_values[0][1].values[3], possible_values[0][1].values[4], possible_values[0][1].values[5], possible_values[0][2].values[3], possible_values[0][2].values[4], possible_values[0][2].values[5], possible_values[0][3].values[3], possible_values[0][3].values[4], possible_values[0][3].values[5], possible_values[0][4].values[3], possible_values[0][4].values[4], possible_values[0][4].values[5], possible_values[0][5].values[3], possible_values[0][5].values[4], possible_values[0][5].values[5], possible_values[0][6].values[3], possible_values[0][6].values[4], possible_values[0][6].values[5], possible_values[0][7].values[3], possible_values[0][7].values[4], possible_values[0][7].values[5], possible_values[0][8].values[3], possible_values[0][8].values[4], possible_values[0][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[0][0].values[6], possible_values[0][0].values[7], possible_values[0][0].values[8], possible_values[0][1].values[6], possible_values[0][1].values[7], possible_values[0][1].values[8], possible_values[0][2].values[6], possible_values[0][2].values[7], possible_values[0][2].values[8], possible_values[0][3].values[6], possible_values[0][3].values[7], possible_values[0][3].values[8], possible_values[0][4].values[6], possible_values[0][4].values[7], possible_values[0][4].values[8], possible_values[0][5].values[6], possible_values[0][5].values[7], possible_values[0][5].values[8], possible_values[0][6].values[6], possible_values[0][6].values[7], possible_values[0][6].values[8], possible_values[0][7].values[6], possible_values[0][7].values[7], possible_values[0][7].values[8], possible_values[0][8].values[6], possible_values[0][8].values[7], possible_values[0][8].values[8]);
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[1][0].values[0], possible_values[1][0].values[1], possible_values[1][0].values[2], possible_values[1][1].values[0], possible_values[1][1].values[1], possible_values[1][1].values[2], possible_values[1][2].values[0], possible_values[1][2].values[1], possible_values[1][2].values[2], possible_values[1][3].values[0], possible_values[1][3].values[1], possible_values[1][3].values[2], possible_values[1][4].values[0], possible_values[1][4].values[1], possible_values[1][4].values[2], possible_values[1][5].values[0], possible_values[1][5].values[1], possible_values[1][5].values[2], possible_values[1][6].values[0], possible_values[1][6].values[1], possible_values[1][6].values[2], possible_values[1][7].values[0], possible_values[1][7].values[1], possible_values[1][7].values[2], possible_values[1][8].values[0], possible_values[1][8].values[1], possible_values[1][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[1][0].values[3], possible_values[1][0].values[4], possible_values[1][0].values[5], possible_values[1][1].values[3], possible_values[1][1].values[4], possible_values[1][1].values[5], possible_values[1][2].values[3], possible_values[1][2].values[4], possible_values[1][2].values[5], possible_values[1][3].values[3], possible_values[1][3].values[4], possible_values[1][3].values[5], possible_values[1][4].values[3], possible_values[1][4].values[4], possible_values[1][4].values[5], possible_values[1][5].values[3], possible_values[1][5].values[4], possible_values[1][5].values[5], possible_values[1][6].values[3], possible_values[1][6].values[4], possible_values[1][6].values[5], possible_values[1][7].values[3], possible_values[1][7].values[4], possible_values[1][7].values[5], possible_values[1][8].values[3], possible_values[1][8].values[4], possible_values[1][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[1][0].values[6], possible_values[1][0].values[7], possible_values[1][0].values[8], possible_values[1][1].values[6], possible_values[1][1].values[7], possible_values[1][1].values[8], possible_values[1][2].values[6], possible_values[1][2].values[7], possible_values[1][2].values[8], possible_values[1][3].values[6], possible_values[1][3].values[7], possible_values[1][3].values[8], possible_values[1][4].values[6], possible_values[1][4].values[7], possible_values[1][4].values[8], possible_values[1][5].values[6], possible_values[1][5].values[7], possible_values[1][5].values[8], possible_values[1][6].values[6], possible_values[1][6].values[7], possible_values[1][6].values[8], possible_values[1][7].values[6], possible_values[1][7].values[7], possible_values[1][7].values[8], possible_values[1][8].values[6], possible_values[1][8].values[7], possible_values[1][8].values[8]);
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[2][0].values[0], possible_values[2][0].values[1], possible_values[2][0].values[2], possible_values[2][1].values[0], possible_values[2][1].values[1], possible_values[2][1].values[2], possible_values[2][2].values[0], possible_values[2][2].values[1], possible_values[2][2].values[2], possible_values[2][3].values[0], possible_values[2][3].values[1], possible_values[2][3].values[2], possible_values[2][4].values[0], possible_values[2][4].values[1], possible_values[2][4].values[2], possible_values[2][5].values[0], possible_values[2][5].values[1], possible_values[2][5].values[2], possible_values[2][6].values[0], possible_values[2][6].values[1], possible_values[2][6].values[2], possible_values[2][7].values[0], possible_values[2][7].values[1], possible_values[2][7].values[2], possible_values[2][8].values[0], possible_values[2][8].values[1], possible_values[2][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[2][0].values[3], possible_values[2][0].values[4], possible_values[2][0].values[5], possible_values[2][1].values[3], possible_values[2][1].values[4], possible_values[2][1].values[5], possible_values[2][2].values[3], possible_values[2][2].values[4], possible_values[2][2].values[5], possible_values[2][3].values[3], possible_values[2][3].values[4], possible_values[2][3].values[5], possible_values[2][4].values[3], possible_values[2][4].values[4], possible_values[2][4].values[5], possible_values[2][5].values[3], possible_values[2][5].values[4], possible_values[2][5].values[5], possible_values[2][6].values[3], possible_values[2][6].values[4], possible_values[2][6].values[5], possible_values[2][7].values[3], possible_values[2][7].values[4], possible_values[2][7].values[5], possible_values[2][8].values[3], possible_values[2][8].values[4], possible_values[2][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[2][0].values[6], possible_values[2][0].values[7], possible_values[2][0].values[8], possible_values[2][1].values[6], possible_values[2][1].values[7], possible_values[2][1].values[8], possible_values[2][2].values[6], possible_values[2][2].values[7], possible_values[2][2].values[8], possible_values[2][3].values[6], possible_values[2][3].values[7], possible_values[2][3].values[8], possible_values[2][4].values[6], possible_values[2][4].values[7], possible_values[2][4].values[8], possible_values[2][5].values[6], possible_values[2][5].values[7], possible_values[2][5].values[8], possible_values[2][6].values[6], possible_values[2][6].values[7], possible_values[2][6].values[8], possible_values[2][7].values[6], possible_values[2][7].values[7], possible_values[2][7].values[8], possible_values[2][8].values[6], possible_values[2][8].values[7], possible_values[2][8].values[8]);
    printf("┣━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━┫\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[3][0].values[0], possible_values[3][0].values[1], possible_values[3][0].values[2], possible_values[3][1].values[0], possible_values[3][1].values[1], possible_values[3][1].values[2], possible_values[3][2].values[0], possible_values[3][2].values[1], possible_values[3][2].values[2], possible_values[3][3].values[0], possible_values[3][3].values[1], possible_values[3][3].values[2], possible_values[3][4].values[0], possible_values[3][4].values[1], possible_values[3][4].values[2], possible_values[3][5].values[0], possible_values[3][5].values[1], possible_values[3][5].values[2], possible_values[3][6].values[0], possible_values[3][6].values[1], possible_values[3][6].values[2], possible_values[3][7].values[0], possible_values[3][7].values[1], possible_values[3][7].values[2], possible_values[3][8].values[0], possible_values[3][8].values[1], possible_values[3][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[3][0].values[3], possible_values[3][0].values[4], possible_values[3][0].values[5], possible_values[3][1].values[3], possible_values[3][1].values[4], possible_values[3][1].values[5], possible_values[3][2].values[3], possible_values[3][2].values[4], possible_values[3][2].values[5], possible_values[3][3].values[3], possible_values[3][3].values[4], possible_values[3][3].values[5], possible_values[3][4].values[3], possible_values[3][4].values[4], possible_values[3][4].values[5], possible_values[3][5].values[3], possible_values[3][5].values[4], possible_values[3][5].values[5], possible_values[3][6].values[3], possible_values[3][6].values[4], possible_values[3][6].values[5], possible_values[3][7].values[3], possible_values[3][7].values[4], possible_values[3][7].values[5], possible_values[3][8].values[3], possible_values[3][8].values[4], possible_values[3][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[3][0].values[6], possible_values[3][0].values[7], possible_values[3][0].values[8], possible_values[3][1].values[6], possible_values[3][1].values[7], possible_values[3][1].values[8], possible_values[3][2].values[6], possible_values[3][2].values[7], possible_values[3][2].values[8], possible_values[3][3].values[6], possible_values[3][3].values[7], possible_values[3][3].values[8], possible_values[3][4].values[6], possible_values[3][4].values[7], possible_values[3][4].values[8], possible_values[3][5].values[6], possible_values[3][5].values[7], possible_values[3][5].values[8], possible_values[3][6].values[6], possible_values[3][6].values[7], possible_values[3][6].values[8], possible_values[3][7].values[6], possible_values[3][7].values[7], possible_values[3][7].values[8], possible_values[3][8].values[6], possible_values[3][8].values[7], possible_values[3][8].values[8]);
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[4][0].values[0], possible_values[4][0].values[1], possible_values[4][0].values[2], possible_values[4][1].values[0], possible_values[4][1].values[1], possible_values[4][1].values[2], possible_values[4][2].values[0], possible_values[4][2].values[1], possible_values[4][2].values[2], possible_values[4][3].values[0], possible_values[4][3].values[1], possible_values[4][3].values[2], possible_values[4][4].values[0], possible_values[4][4].values[1], possible_values[4][4].values[2], possible_values[4][5].values[0], possible_values[4][5].values[1], possible_values[4][5].values[2], possible_values[4][6].values[0], possible_values[4][6].values[1], possible_values[4][6].values[2], possible_values[4][7].values[0], possible_values[4][7].values[1], possible_values[4][7].values[2], possible_values[4][8].values[0], possible_values[4][8].values[1], possible_values[4][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[4][0].values[3], possible_values[4][0].values[4], possible_values[4][0].values[5], possible_values[4][1].values[3], possible_values[4][1].values[4], possible_values[4][1].values[5], possible_values[4][2].values[3], possible_values[4][2].values[4], possible_values[4][2].values[5], possible_values[4][3].values[3], possible_values[4][3].values[4], possible_values[4][3].values[5], possible_values[4][4].values[3], possible_values[4][4].values[4], possible_values[4][4].values[5], possible_values[4][5].values[3], possible_values[4][5].values[4], possible_values[4][5].values[5], possible_values[4][6].values[3], possible_values[4][6].values[4], possible_values[4][6].values[5], possible_values[4][7].values[3], possible_values[4][7].values[4], possible_values[4][7].values[5], possible_values[4][8].values[3], possible_values[4][8].values[4], possible_values[4][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[4][0].values[6], possible_values[4][0].values[7], possible_values[4][0].values[8], possible_values[4][1].values[6], possible_values[4][1].values[7], possible_values[4][1].values[8], possible_values[4][2].values[6], possible_values[4][2].values[7], possible_values[4][2].values[8], possible_values[4][3].values[6], possible_values[4][3].values[7], possible_values[4][3].values[8], possible_values[4][4].values[6], possible_values[4][4].values[7], possible_values[4][4].values[8], possible_values[4][5].values[6], possible_values[4][5].values[7], possible_values[4][5].values[8], possible_values[4][6].values[6], possible_values[4][6].values[7], possible_values[4][6].values[8], possible_values[4][7].values[6], possible_values[4][7].values[7], possible_values[4][7].values[8], possible_values[4][8].values[6], possible_values[4][8].values[7], possible_values[4][8].values[8]);
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[5][0].values[0], possible_values[5][0].values[1], possible_values[5][0].values[2], possible_values[5][1].values[0], possible_values[5][1].values[1], possible_values[5][1].values[2], possible_values[5][2].values[0], possible_values[5][2].values[1], possible_values[5][2].values[2], possible_values[5][3].values[0], possible_values[5][3].values[1], possible_values[5][3].values[2], possible_values[5][4].values[0], possible_values[5][4].values[1], possible_values[5][4].values[2], possible_values[5][5].values[0], possible_values[5][5].values[1], possible_values[5][5].values[2], possible_values[5][6].values[0], possible_values[5][6].values[1], possible_values[5][6].values[2], possible_values[5][7].values[0], possible_values[5][7].values[1], possible_values[5][7].values[2], possible_values[5][8].values[0], possible_values[5][8].values[1], possible_values[5][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[5][0].values[3], possible_values[5][0].values[4], possible_values[5][0].values[5], possible_values[5][1].values[3], possible_values[5][1].values[4], possible_values[5][1].values[5], possible_values[5][2].values[3], possible_values[5][2].values[4], possible_values[5][2].values[5], possible_values[5][3].values[3], possible_values[5][3].values[4], possible_values[5][3].values[5], possible_values[5][4].values[3], possible_values[5][4].values[4], possible_values[5][4].values[5], possible_values[5][5].values[3], possible_values[5][5].values[4], possible_values[5][5].values[5], possible_values[5][6].values[3], possible_values[5][6].values[4], possible_values[5][6].values[5], possible_values[5][7].values[3], possible_values[5][7].values[4], possible_values[5][7].values[5], possible_values[5][8].values[3], possible_values[5][8].values[4], possible_values[5][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[5][0].values[6], possible_values[5][0].values[7], possible_values[5][0].values[8], possible_values[5][1].values[6], possible_values[5][1].values[7], possible_values[5][1].values[8], possible_values[5][2].values[6], possible_values[5][2].values[7], possible_values[5][2].values[8], possible_values[5][3].values[6], possible_values[5][3].values[7], possible_values[5][3].values[8], possible_values[5][4].values[6], possible_values[5][4].values[7], possible_values[5][4].values[8], possible_values[5][5].values[6], possible_values[5][5].values[7], possible_values[5][5].values[8], possible_values[5][6].values[6], possible_values[5][6].values[7], possible_values[5][6].values[8], possible_values[5][7].values[6], possible_values[5][7].values[7], possible_values[5][7].values[8], possible_values[5][8].values[6], possible_values[5][8].values[7], possible_values[5][8].values[8]);
    printf("┣━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━┫\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[6][0].values[0], possible_values[6][0].values[1], possible_values[6][0].values[2], possible_values[6][1].values[0], possible_values[6][1].values[1], possible_values[6][1].values[2], possible_values[6][2].values[0], possible_values[6][2].values[1], possible_values[6][2].values[2], possible_values[6][3].values[0], possible_values[6][3].values[1], possible_values[6][3].values[2], possible_values[6][4].values[0], possible_values[6][4].values[1], possible_values[6][4].values[2], possible_values[6][5].values[0], possible_values[6][5].values[1], possible_values[6][5].values[2], possible_values[6][6].values[0], possible_values[6][6].values[1], possible_values[6][6].values[2], possible_values[6][7].values[0], possible_values[6][7].values[1], possible_values[6][7].values[2], possible_values[6][8].values[0], possible_values[6][8].values[1], possible_values[6][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[6][0].values[3], possible_values[6][0].values[4], possible_values[6][0].values[5], possible_values[6][1].values[3], possible_values[6][1].values[4], possible_values[6][1].values[5], possible_values[6][2].values[3], possible_values[6][2].values[4], possible_values[6][2].values[5], possible_values[6][3].values[3], possible_values[6][3].values[4], possible_values[6][3].values[5], possible_values[6][4].values[3], possible_values[6][4].values[4], possible_values[6][4].values[5], possible_values[6][5].values[3], possible_values[6][5].values[4], possible_values[6][5].values[5], possible_values[6][6].values[3], possible_values[6][6].values[4], possible_values[6][6].values[5], possible_values[6][7].values[3], possible_values[6][7].values[4], possible_values[6][7].values[5], possible_values[6][8].values[3], possible_values[6][8].values[4], possible_values[6][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[6][0].values[6], possible_values[6][0].values[7], possible_values[6][0].values[8], possible_values[6][1].values[6], possible_values[6][1].values[7], possible_values[6][1].values[8], possible_values[6][2].values[6], possible_values[6][2].values[7], possible_values[6][2].values[8], possible_values[6][3].values[6], possible_values[6][3].values[7], possible_values[6][3].values[8], possible_values[6][4].values[6], possible_values[6][4].values[7], possible_values[6][4].values[8], possible_values[6][5].values[6], possible_values[6][5].values[7], possible_values[6][5].values[8], possible_values[6][6].values[6], possible_values[6][6].values[7], possible_values[6][6].values[8], possible_values[6][7].values[6], possible_values[6][7].values[7], possible_values[6][7].values[8], possible_values[6][8].values[6], possible_values[6][8].values[7], possible_values[6][8].values[8]);
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[7][0].values[0], possible_values[7][0].values[1], possible_values[7][0].values[2], possible_values[7][1].values[0], possible_values[7][1].values[1], possible_values[7][1].values[2], possible_values[7][2].values[0], possible_values[7][2].values[1], possible_values[7][2].values[2], possible_values[7][3].values[0], possible_values[7][3].values[1], possible_values[7][3].values[2], possible_values[7][4].values[0], possible_values[7][4].values[1], possible_values[7][4].values[2], possible_values[7][5].values[0], possible_values[7][5].values[1], possible_values[7][5].values[2], possible_values[7][6].values[0], possible_values[7][6].values[1], possible_values[7][6].values[2], possible_values[7][7].values[0], possible_values[7][7].values[1], possible_values[7][7].values[2], possible_values[7][8].values[0], possible_values[7][8].values[1], possible_values[7][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[7][0].values[3], possible_values[7][0].values[4], possible_values[7][0].values[5], possible_values[7][1].values[3], possible_values[7][1].values[4], possible_values[7][1].values[5], possible_values[7][2].values[3], possible_values[7][2].values[4], possible_values[7][2].values[5], possible_values[7][3].values[3], possible_values[7][3].values[4], possible_values[7][3].values[5], possible_values[7][4].values[3], possible_values[7][4].values[4], possible_values[7][4].values[5], possible_values[7][5].values[3], possible_values[7][5].values[4], possible_values[7][5].values[5], possible_values[7][6].values[3], possible_values[7][6].values[4], possible_values[7][6].values[5], possible_values[7][7].values[3], possible_values[7][7].values[4], possible_values[7][7].values[5], possible_values[7][8].values[3], possible_values[7][8].values[4], possible_values[7][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[7][0].values[6], possible_values[7][0].values[7], possible_values[7][0].values[8], possible_values[7][1].values[6], possible_values[7][1].values[7], possible_values[7][1].values[8], possible_values[7][2].values[6], possible_values[7][2].values[7], possible_values[7][2].values[8], possible_values[7][3].values[6], possible_values[7][3].values[7], possible_values[7][3].values[8], possible_values[7][4].values[6], possible_values[7][4].values[7], possible_values[7][4].values[8], possible_values[7][5].values[6], possible_values[7][5].values[7], possible_values[7][5].values[8], possible_values[7][6].values[6], possible_values[7][6].values[7], possible_values[7][6].values[8], possible_values[7][7].values[6], possible_values[7][7].values[7], possible_values[7][7].values[8], possible_values[7][8].values[6], possible_values[7][8].values[7], possible_values[7][8].values[8]);
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[8][0].values[0], possible_values[8][0].values[1], possible_values[8][0].values[2], possible_values[8][1].values[0], possible_values[8][1].values[1], possible_values[8][1].values[2], possible_values[8][2].values[0], possible_values[8][2].values[1], possible_values[8][2].values[2], possible_values[8][3].values[0], possible_values[8][3].values[1], possible_values[8][3].values[2], possible_values[8][4].values[0], possible_values[8][4].values[1], possible_values[8][4].values[2], possible_values[8][5].values[0], possible_values[8][5].values[1], possible_values[8][5].values[2], possible_values[8][6].values[0], possible_values[8][6].values[1], possible_values[8][6].values[2], possible_values[8][7].values[0], possible_values[8][7].values[1], possible_values[8][7].values[2], possible_values[8][8].values[0], possible_values[8][8].values[1], possible_values[8][8].values[2]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[8][0].values[3], possible_values[8][0].values[4], possible_values[8][0].values[5], possible_values[8][1].values[3], possible_values[8][1].values[4], possible_values[8][1].values[5], possible_values[8][2].values[3], possible_values[8][2].values[4], possible_values[8][2].values[5], possible_values[8][3].values[3], possible_values[8][3].values[4], possible_values[8][3].values[5], possible_values[8][4].values[3], possible_values[8][4].values[4], possible_values[8][4].values[5], possible_values[8][5].values[3], possible_values[8][5].values[4], possible_values[8][5].values[5], possible_values[8][6].values[3], possible_values[8][6].values[4], possible_values[8][6].values[5], possible_values[8][7].values[3], possible_values[8][7].values[4], possible_values[8][7].values[5], possible_values[8][8].values[3], possible_values[8][8].values[4], possible_values[8][8].values[5]);
    printf("┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃ %hu%hu%hu ┊ %hu%hu%hu ┊ %hu%hu%hu ┃\n", possible_values[8][0].values[6], possible_values[8][0].values[7], possible_values[8][0].values[8], possible_values[8][1].values[6], possible_values[8][1].values[7], possible_values[8][1].values[8], possible_values[8][2].values[6], possible_values[8][2].values[7], possible_values[8][2].values[8], possible_values[8][3].values[6], possible_values[8][3].values[7], possible_values[8][3].values[8], possible_values[8][4].values[6], possible_values[8][4].values[7], possible_values[8][4].values[8], possible_values[8][5].values[6], possible_values[8][5].values[7], possible_values[8][5].values[8], possible_values[8][6].values[6], possible_values[8][6].values[7], possible_values[8][6].values[8], possible_values[8][7].values[6], possible_values[8][7].values[7], possible_values[8][7].values[8], possible_values[8][8].values[6], possible_values[8][8].values[7], possible_values[8][8].values[8]);
    printf("┗━━━━━┷━━━━━┷━━━━━┻━━━━━┷━━━━━┷━━━━━┻━━━━━┷━━━━━┷━━━━━┛\n");
}

void solve_sudoku(unsigned short int input[9][9], unsigned short int output[9][9]) {
    init_possible_values();
    
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            output[i][j] = input[i][j];
        }
    }
    
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            unsigned short int n = input[i][j];
            if (n >= 1 && n <= 9)
            {
                handle_number_in_cell(n, i, j);
            }
        }
    }

    for (size_t x = 0; x < SUDOKU_MAX_ITERATIONS && is_not_fully_solved(output) == 0; x++)
    {
        for (size_t i = 0; i < 9; i++)
        {
            for (size_t j = 0; j < 9; j++)
            {
                unsigned short int only_possible_value = only_one_possible_value_in_cell(i, j);

                if (input[i][j] == 0 && only_possible_value != 0)
                {
                    output[i][j] = only_possible_value;
                    handle_number_in_cell(only_possible_value, i, j);
                }
            }
        }
        check_hidden_singles();
    }

    if(is_not_fully_solved(output) == 0) {
        printf("Sudoku not solved. Possible values so far:\n");
        print_possible_values_table();
    }
}
