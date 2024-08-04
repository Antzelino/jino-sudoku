#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "sudoku_solver.h"

#define SUDOKU_MAX_ITERATIONS 10

/*
 * Notes for a single cell
 * A cell starts by having 9 possible values (1-9), until only 1 is left, which becomes the number of the cell
 * If `values[n]` is 0 then n+1 is not a possible value in this cell. Otherwise `values[n]` is equal to n+1
 */
typedef struct Notes {
    unsigned short int values[9];
} Notes;

// All candidates for all cells of the board (9x9 board), by using one `Notes` per cell of the board
Notes candidates[9][9];

void init_candidates() {
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            for (size_t k = 0; k < 9; k++)
            {
                candidates[i][j].values[k] = k + 1;
            }
        }
    }
}

void set_candidate_single_value(unsigned short int n, unsigned short int i, unsigned short int j) {
    for (unsigned short int n_ = 0; n_ < 9; n_++)
    {
        if (n_ == n-1)
        {
            assert(candidates[i][j].values[n_] != 0 && "Cell should have value, not 0");
            assert(candidates[i][j].values[n_] == n && "Cell has wrong value in index");
            continue;
        }
        candidates[i][j].values[n_] = 0;
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
                assert(candidates[i][j].values[n-1] != 0 && "Cell should have value, not 0");
                assert(candidates[i][j].values[n-1] == n && "Cell has wrong value in index");
                continue;
            }
            candidates[i_][j_].values[n-1] = 0;
        }
    }
}

void clear_number_from_column(unsigned short int n, unsigned short int i, unsigned short int j) {
    for (size_t i_ = 0; i_ < 9; i_ ++)
    {
        if (i_ == i)
        {
            assert(candidates[i][j].values[n-1] != 0 && "Cell should have value, not 0");
            assert(candidates[i][j].values[n-1] == n && "Cell has wrong value in index");
            continue;
        }
        candidates[i_][j].values[n-1] = 0;
    }
}

void clear_number_from_row(unsigned short int n, unsigned short int i, unsigned short int j) {
    for (size_t j_ = 0; j_ < 9; j_ ++)
    {
        if (j_ == j)
        {
            assert(candidates[i][j].values[n-1] != 0 && "Cell should have value, not 0");
            assert(candidates[i][j].values[n-1] == n && "Cell has wrong value in index");
            continue;
        }
        candidates[i][j_].values[n-1] = 0;
    }
}

void handle_number_in_cell(unsigned short int n, unsigned short int i, unsigned short int j) {
    assert(n != 0 && "Must not set number in cell to 0");
    clear_number_from_row(n, i, j);
    clear_number_from_column(n, i, j);
    clear_number_from_3x3(n, i, j);
    set_candidate_single_value(n, i, j);
}

int only_one_possible_value_in_cell(unsigned short int i, unsigned short int j) {
    unsigned short int possible_value = 0;
    for (size_t n_ = 0; n_ < 9; n_++)
    {
        if (possible_value == 0)
        {
            possible_value = candidates[i][j].values[n_];
        }
        else if (candidates[i][j].values[n_] != 0)
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
                if (candidates[i][j].values[n-1] != 0)
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
                if (candidates[i][j].values[n-1] != 0)
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
                        if (candidates[i_][j_].values[n-1] != 0)
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

#define PV(x,y,n) ('0' + (possible_values[x][y].values[n]))

void print_possible_values_table() {
    printf("┏━━━━━┯━━━━━┯━━━━━┳━━━━━┯━━━━━┯━━━━━┳━━━━━┯━━━━━┯━━━━━┓\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(0,0,0) == '0' ? ' ' : PV(0,0,0), PV(0,0,1) == '0' ? ' ' : PV(0,0,1), PV(0,0,2) == '0' ? ' ' : PV(0,0,2), PV(0,1,0) == '0' ? ' ' : PV(0,1,0), PV(0,1,1) == '0' ? ' ' : PV(0,1,1), PV(0,1,2) == '0' ? ' ' : PV(0,1,2), PV(0,2,0) == '0' ? ' ' : PV(0,2,0), PV(0,2,1) == '0' ? ' ' : PV(0,2,1), PV(0,2,2) == '0' ? ' ' : PV(0,2,2), PV(0,3,0) == '0' ? ' ' : PV(0,3,0), PV(0,3,1) == '0' ? ' ' : PV(0,3,1), PV(0,3,2) == '0' ? ' ' : PV(0,3,2), PV(0,4,0) == '0' ? ' ' : PV(0,4,0), PV(0,4,1) == '0' ? ' ' : PV(0,4,1), PV(0,4,2) == '0' ? ' ' : PV(0,4,2), PV(0,5,0) == '0' ? ' ' : PV(0,5,0), PV(0,5,1) == '0' ? ' ' : PV(0,5,1), PV(0,5,2) == '0' ? ' ' : PV(0,5,2), PV(0,6,0) == '0' ? ' ' : PV(0,6,0), PV(0,6,1) == '0' ? ' ' : PV(0,6,1), PV(0,6,2) == '0' ? ' ' : PV(0,6,2), PV(0,7,0) == '0' ? ' ' : PV(0,7,0), PV(0,7,1) == '0' ? ' ' : PV(0,7,1), PV(0,7,2) == '0' ? ' ' : PV(0,7,2), PV(0,8,0) == '0' ? ' ' : PV(0,8,0), PV(0,8,1) == '0' ? ' ' : PV(0,8,1), PV(0,8,2) == '0' ? ' ' : PV(0,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(0,0,3) == '0' ? ' ' : PV(0,0,3), PV(0,0,4) == '0' ? ' ' : PV(0,0,4), PV(0,0,5) == '0' ? ' ' : PV(0,0,5), PV(0,1,3) == '0' ? ' ' : PV(0,1,3), PV(0,1,4) == '0' ? ' ' : PV(0,1,4), PV(0,1,5) == '0' ? ' ' : PV(0,1,5), PV(0,2,3) == '0' ? ' ' : PV(0,2,3), PV(0,2,4) == '0' ? ' ' : PV(0,2,4), PV(0,2,5) == '0' ? ' ' : PV(0,2,5), PV(0,3,3) == '0' ? ' ' : PV(0,3,3), PV(0,3,4) == '0' ? ' ' : PV(0,3,4), PV(0,3,5) == '0' ? ' ' : PV(0,3,5), PV(0,4,3) == '0' ? ' ' : PV(0,4,3), PV(0,4,4) == '0' ? ' ' : PV(0,4,4), PV(0,4,5) == '0' ? ' ' : PV(0,4,5), PV(0,5,3) == '0' ? ' ' : PV(0,5,3), PV(0,5,4) == '0' ? ' ' : PV(0,5,4), PV(0,5,5) == '0' ? ' ' : PV(0,5,5), PV(0,6,3) == '0' ? ' ' : PV(0,6,3), PV(0,6,4) == '0' ? ' ' : PV(0,6,4), PV(0,6,5) == '0' ? ' ' : PV(0,6,5), PV(0,7,3) == '0' ? ' ' : PV(0,7,3), PV(0,7,4) == '0' ? ' ' : PV(0,7,4), PV(0,7,5) == '0' ? ' ' : PV(0,7,5), PV(0,8,3) == '0' ? ' ' : PV(0,8,3), PV(0,8,4) == '0' ? ' ' : PV(0,8,4), PV(0,8,5) == '0' ? ' ' : PV(0,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(0,0,6) == '0' ? ' ' : PV(0,0,6), PV(0,0,7) == '0' ? ' ' : PV(0,0,7), PV(0,0,8) == '0' ? ' ' : PV(0,0,8), PV(0,1,6) == '0' ? ' ' : PV(0,1,6), PV(0,1,7) == '0' ? ' ' : PV(0,1,7), PV(0,1,8) == '0' ? ' ' : PV(0,1,8), PV(0,2,6) == '0' ? ' ' : PV(0,2,6), PV(0,2,7) == '0' ? ' ' : PV(0,2,7), PV(0,2,8) == '0' ? ' ' : PV(0,2,8), PV(0,3,6) == '0' ? ' ' : PV(0,3,6), PV(0,3,7) == '0' ? ' ' : PV(0,3,7), PV(0,3,8) == '0' ? ' ' : PV(0,3,8), PV(0,4,6) == '0' ? ' ' : PV(0,4,6), PV(0,4,7) == '0' ? ' ' : PV(0,4,7), PV(0,4,8) == '0' ? ' ' : PV(0,4,8), PV(0,5,6) == '0' ? ' ' : PV(0,5,6), PV(0,5,7) == '0' ? ' ' : PV(0,5,7), PV(0,5,8) == '0' ? ' ' : PV(0,5,8), PV(0,6,6) == '0' ? ' ' : PV(0,6,6), PV(0,6,7) == '0' ? ' ' : PV(0,6,7), PV(0,6,8) == '0' ? ' ' : PV(0,6,8), PV(0,7,6) == '0' ? ' ' : PV(0,7,6), PV(0,7,7) == '0' ? ' ' : PV(0,7,7), PV(0,7,8) == '0' ? ' ' : PV(0,7,8), PV(0,8,6) == '0' ? ' ' : PV(0,8,6), PV(0,8,7) == '0' ? ' ' : PV(0,8,7), PV(0,8,8) == '0' ? ' ' : PV(0,8,8));
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(1,0,0) == '0' ? ' ' : PV(1,0,0), PV(1,0,1) == '0' ? ' ' : PV(1,0,1), PV(1,0,2) == '0' ? ' ' : PV(1,0,2), PV(1,1,0) == '0' ? ' ' : PV(1,1,0), PV(1,1,1) == '0' ? ' ' : PV(1,1,1), PV(1,1,2) == '0' ? ' ' : PV(1,1,2), PV(1,2,0) == '0' ? ' ' : PV(1,2,0), PV(1,2,1) == '0' ? ' ' : PV(1,2,1), PV(1,2,2) == '0' ? ' ' : PV(1,2,2), PV(1,3,0) == '0' ? ' ' : PV(1,3,0), PV(1,3,1) == '0' ? ' ' : PV(1,3,1), PV(1,3,2) == '0' ? ' ' : PV(1,3,2), PV(1,4,0) == '0' ? ' ' : PV(1,4,0), PV(1,4,1) == '0' ? ' ' : PV(1,4,1), PV(1,4,2) == '0' ? ' ' : PV(1,4,2), PV(1,5,0) == '0' ? ' ' : PV(1,5,0), PV(1,5,1) == '0' ? ' ' : PV(1,5,1), PV(1,5,2) == '0' ? ' ' : PV(1,5,2), PV(1,6,0) == '0' ? ' ' : PV(1,6,0), PV(1,6,1) == '0' ? ' ' : PV(1,6,1), PV(1,6,2) == '0' ? ' ' : PV(1,6,2), PV(1,7,0) == '0' ? ' ' : PV(1,7,0), PV(1,7,1) == '0' ? ' ' : PV(1,7,1), PV(1,7,2) == '0' ? ' ' : PV(1,7,2), PV(1,8,0) == '0' ? ' ' : PV(1,8,0), PV(1,8,1) == '0' ? ' ' : PV(1,8,1), PV(1,8,2) == '0' ? ' ' : PV(1,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(1,0,3) == '0' ? ' ' : PV(1,0,3), PV(1,0,4) == '0' ? ' ' : PV(1,0,4), PV(1,0,5) == '0' ? ' ' : PV(1,0,5), PV(1,1,3) == '0' ? ' ' : PV(1,1,3), PV(1,1,4) == '0' ? ' ' : PV(1,1,4), PV(1,1,5) == '0' ? ' ' : PV(1,1,5), PV(1,2,3) == '0' ? ' ' : PV(1,2,3), PV(1,2,4) == '0' ? ' ' : PV(1,2,4), PV(1,2,5) == '0' ? ' ' : PV(1,2,5), PV(1,3,3) == '0' ? ' ' : PV(1,3,3), PV(1,3,4) == '0' ? ' ' : PV(1,3,4), PV(1,3,5) == '0' ? ' ' : PV(1,3,5), PV(1,4,3) == '0' ? ' ' : PV(1,4,3), PV(1,4,4) == '0' ? ' ' : PV(1,4,4), PV(1,4,5) == '0' ? ' ' : PV(1,4,5), PV(1,5,3) == '0' ? ' ' : PV(1,5,3), PV(1,5,4) == '0' ? ' ' : PV(1,5,4), PV(1,5,5) == '0' ? ' ' : PV(1,5,5), PV(1,6,3) == '0' ? ' ' : PV(1,6,3), PV(1,6,4) == '0' ? ' ' : PV(1,6,4), PV(1,6,5) == '0' ? ' ' : PV(1,6,5), PV(1,7,3) == '0' ? ' ' : PV(1,7,3), PV(1,7,4) == '0' ? ' ' : PV(1,7,4), PV(1,7,5) == '0' ? ' ' : PV(1,7,5), PV(1,8,3) == '0' ? ' ' : PV(1,8,3), PV(1,8,4) == '0' ? ' ' : PV(1,8,4), PV(1,8,5) == '0' ? ' ' : PV(1,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(1,0,6) == '0' ? ' ' : PV(1,0,6), PV(1,0,7) == '0' ? ' ' : PV(1,0,7), PV(1,0,8) == '0' ? ' ' : PV(1,0,8), PV(1,1,6) == '0' ? ' ' : PV(1,1,6), PV(1,1,7) == '0' ? ' ' : PV(1,1,7), PV(1,1,8) == '0' ? ' ' : PV(1,1,8), PV(1,2,6) == '0' ? ' ' : PV(1,2,6), PV(1,2,7) == '0' ? ' ' : PV(1,2,7), PV(1,2,8) == '0' ? ' ' : PV(1,2,8), PV(1,3,6) == '0' ? ' ' : PV(1,3,6), PV(1,3,7) == '0' ? ' ' : PV(1,3,7), PV(1,3,8) == '0' ? ' ' : PV(1,3,8), PV(1,4,6) == '0' ? ' ' : PV(1,4,6), PV(1,4,7) == '0' ? ' ' : PV(1,4,7), PV(1,4,8) == '0' ? ' ' : PV(1,4,8), PV(1,5,6) == '0' ? ' ' : PV(1,5,6), PV(1,5,7) == '0' ? ' ' : PV(1,5,7), PV(1,5,8) == '0' ? ' ' : PV(1,5,8), PV(1,6,6) == '0' ? ' ' : PV(1,6,6), PV(1,6,7) == '0' ? ' ' : PV(1,6,7), PV(1,6,8) == '0' ? ' ' : PV(1,6,8), PV(1,7,6) == '0' ? ' ' : PV(1,7,6), PV(1,7,7) == '0' ? ' ' : PV(1,7,7), PV(1,7,8) == '0' ? ' ' : PV(1,7,8), PV(1,8,6) == '0' ? ' ' : PV(1,8,6), PV(1,8,7) == '0' ? ' ' : PV(1,8,7), PV(1,8,8) == '0' ? ' ' : PV(1,8,8));
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(2,0,0) == '0' ? ' ' : PV(2,0,0), PV(2,0,1) == '0' ? ' ' : PV(2,0,1), PV(2,0,2) == '0' ? ' ' : PV(2,0,2), PV(2,1,0) == '0' ? ' ' : PV(2,1,0), PV(2,1,1) == '0' ? ' ' : PV(2,1,1), PV(2,1,2) == '0' ? ' ' : PV(2,1,2), PV(2,2,0) == '0' ? ' ' : PV(2,2,0), PV(2,2,1) == '0' ? ' ' : PV(2,2,1), PV(2,2,2) == '0' ? ' ' : PV(2,2,2), PV(2,3,0) == '0' ? ' ' : PV(2,3,0), PV(2,3,1) == '0' ? ' ' : PV(2,3,1), PV(2,3,2) == '0' ? ' ' : PV(2,3,2), PV(2,4,0) == '0' ? ' ' : PV(2,4,0), PV(2,4,1) == '0' ? ' ' : PV(2,4,1), PV(2,4,2) == '0' ? ' ' : PV(2,4,2), PV(2,5,0) == '0' ? ' ' : PV(2,5,0), PV(2,5,1) == '0' ? ' ' : PV(2,5,1), PV(2,5,2) == '0' ? ' ' : PV(2,5,2), PV(2,6,0) == '0' ? ' ' : PV(2,6,0), PV(2,6,1) == '0' ? ' ' : PV(2,6,1), PV(2,6,2) == '0' ? ' ' : PV(2,6,2), PV(2,7,0) == '0' ? ' ' : PV(2,7,0), PV(2,7,1) == '0' ? ' ' : PV(2,7,1), PV(2,7,2) == '0' ? ' ' : PV(2,7,2), PV(2,8,0) == '0' ? ' ' : PV(2,8,0), PV(2,8,1) == '0' ? ' ' : PV(2,8,1), PV(2,8,2) == '0' ? ' ' : PV(2,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(2,0,3) == '0' ? ' ' : PV(2,0,3), PV(2,0,4) == '0' ? ' ' : PV(2,0,4), PV(2,0,5) == '0' ? ' ' : PV(2,0,5), PV(2,1,3) == '0' ? ' ' : PV(2,1,3), PV(2,1,4) == '0' ? ' ' : PV(2,1,4), PV(2,1,5) == '0' ? ' ' : PV(2,1,5), PV(2,2,3) == '0' ? ' ' : PV(2,2,3), PV(2,2,4) == '0' ? ' ' : PV(2,2,4), PV(2,2,5) == '0' ? ' ' : PV(2,2,5), PV(2,3,3) == '0' ? ' ' : PV(2,3,3), PV(2,3,4) == '0' ? ' ' : PV(2,3,4), PV(2,3,5) == '0' ? ' ' : PV(2,3,5), PV(2,4,3) == '0' ? ' ' : PV(2,4,3), PV(2,4,4) == '0' ? ' ' : PV(2,4,4), PV(2,4,5) == '0' ? ' ' : PV(2,4,5), PV(2,5,3) == '0' ? ' ' : PV(2,5,3), PV(2,5,4) == '0' ? ' ' : PV(2,5,4), PV(2,5,5) == '0' ? ' ' : PV(2,5,5), PV(2,6,3) == '0' ? ' ' : PV(2,6,3), PV(2,6,4) == '0' ? ' ' : PV(2,6,4), PV(2,6,5) == '0' ? ' ' : PV(2,6,5), PV(2,7,3) == '0' ? ' ' : PV(2,7,3), PV(2,7,4) == '0' ? ' ' : PV(2,7,4), PV(2,7,5) == '0' ? ' ' : PV(2,7,5), PV(2,8,3) == '0' ? ' ' : PV(2,8,3), PV(2,8,4) == '0' ? ' ' : PV(2,8,4), PV(2,8,5) == '0' ? ' ' : PV(2,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(2,0,6) == '0' ? ' ' : PV(2,0,6), PV(2,0,7) == '0' ? ' ' : PV(2,0,7), PV(2,0,8) == '0' ? ' ' : PV(2,0,8), PV(2,1,6) == '0' ? ' ' : PV(2,1,6), PV(2,1,7) == '0' ? ' ' : PV(2,1,7), PV(2,1,8) == '0' ? ' ' : PV(2,1,8), PV(2,2,6) == '0' ? ' ' : PV(2,2,6), PV(2,2,7) == '0' ? ' ' : PV(2,2,7), PV(2,2,8) == '0' ? ' ' : PV(2,2,8), PV(2,3,6) == '0' ? ' ' : PV(2,3,6), PV(2,3,7) == '0' ? ' ' : PV(2,3,7), PV(2,3,8) == '0' ? ' ' : PV(2,3,8), PV(2,4,6) == '0' ? ' ' : PV(2,4,6), PV(2,4,7) == '0' ? ' ' : PV(2,4,7), PV(2,4,8) == '0' ? ' ' : PV(2,4,8), PV(2,5,6) == '0' ? ' ' : PV(2,5,6), PV(2,5,7) == '0' ? ' ' : PV(2,5,7), PV(2,5,8) == '0' ? ' ' : PV(2,5,8), PV(2,6,6) == '0' ? ' ' : PV(2,6,6), PV(2,6,7) == '0' ? ' ' : PV(2,6,7), PV(2,6,8) == '0' ? ' ' : PV(2,6,8), PV(2,7,6) == '0' ? ' ' : PV(2,7,6), PV(2,7,7) == '0' ? ' ' : PV(2,7,7), PV(2,7,8) == '0' ? ' ' : PV(2,7,8), PV(2,8,6) == '0' ? ' ' : PV(2,8,6), PV(2,8,7) == '0' ? ' ' : PV(2,8,7), PV(2,8,8) == '0' ? ' ' : PV(2,8,8));
    printf("┣━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━┫\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(3,0,0) == '0' ? ' ' : PV(3,0,0), PV(3,0,1) == '0' ? ' ' : PV(3,0,1), PV(3,0,2) == '0' ? ' ' : PV(3,0,2), PV(3,1,0) == '0' ? ' ' : PV(3,1,0), PV(3,1,1) == '0' ? ' ' : PV(3,1,1), PV(3,1,2) == '0' ? ' ' : PV(3,1,2), PV(3,2,0) == '0' ? ' ' : PV(3,2,0), PV(3,2,1) == '0' ? ' ' : PV(3,2,1), PV(3,2,2) == '0' ? ' ' : PV(3,2,2), PV(3,3,0) == '0' ? ' ' : PV(3,3,0), PV(3,3,1) == '0' ? ' ' : PV(3,3,1), PV(3,3,2) == '0' ? ' ' : PV(3,3,2), PV(3,4,0) == '0' ? ' ' : PV(3,4,0), PV(3,4,1) == '0' ? ' ' : PV(3,4,1), PV(3,4,2) == '0' ? ' ' : PV(3,4,2), PV(3,5,0) == '0' ? ' ' : PV(3,5,0), PV(3,5,1) == '0' ? ' ' : PV(3,5,1), PV(3,5,2) == '0' ? ' ' : PV(3,5,2), PV(3,6,0) == '0' ? ' ' : PV(3,6,0), PV(3,6,1) == '0' ? ' ' : PV(3,6,1), PV(3,6,2) == '0' ? ' ' : PV(3,6,2), PV(3,7,0) == '0' ? ' ' : PV(3,7,0), PV(3,7,1) == '0' ? ' ' : PV(3,7,1), PV(3,7,2) == '0' ? ' ' : PV(3,7,2), PV(3,8,0) == '0' ? ' ' : PV(3,8,0), PV(3,8,1) == '0' ? ' ' : PV(3,8,1), PV(3,8,2) == '0' ? ' ' : PV(3,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(3,0,3) == '0' ? ' ' : PV(3,0,3), PV(3,0,4) == '0' ? ' ' : PV(3,0,4), PV(3,0,5) == '0' ? ' ' : PV(3,0,5), PV(3,1,3) == '0' ? ' ' : PV(3,1,3), PV(3,1,4) == '0' ? ' ' : PV(3,1,4), PV(3,1,5) == '0' ? ' ' : PV(3,1,5), PV(3,2,3) == '0' ? ' ' : PV(3,2,3), PV(3,2,4) == '0' ? ' ' : PV(3,2,4), PV(3,2,5) == '0' ? ' ' : PV(3,2,5), PV(3,3,3) == '0' ? ' ' : PV(3,3,3), PV(3,3,4) == '0' ? ' ' : PV(3,3,4), PV(3,3,5) == '0' ? ' ' : PV(3,3,5), PV(3,4,3) == '0' ? ' ' : PV(3,4,3), PV(3,4,4) == '0' ? ' ' : PV(3,4,4), PV(3,4,5) == '0' ? ' ' : PV(3,4,5), PV(3,5,3) == '0' ? ' ' : PV(3,5,3), PV(3,5,4) == '0' ? ' ' : PV(3,5,4), PV(3,5,5) == '0' ? ' ' : PV(3,5,5), PV(3,6,3) == '0' ? ' ' : PV(3,6,3), PV(3,6,4) == '0' ? ' ' : PV(3,6,4), PV(3,6,5) == '0' ? ' ' : PV(3,6,5), PV(3,7,3) == '0' ? ' ' : PV(3,7,3), PV(3,7,4) == '0' ? ' ' : PV(3,7,4), PV(3,7,5) == '0' ? ' ' : PV(3,7,5), PV(3,8,3) == '0' ? ' ' : PV(3,8,3), PV(3,8,4) == '0' ? ' ' : PV(3,8,4), PV(3,8,5) == '0' ? ' ' : PV(3,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(3,0,6) == '0' ? ' ' : PV(3,0,6), PV(3,0,7) == '0' ? ' ' : PV(3,0,7), PV(3,0,8) == '0' ? ' ' : PV(3,0,8), PV(3,1,6) == '0' ? ' ' : PV(3,1,6), PV(3,1,7) == '0' ? ' ' : PV(3,1,7), PV(3,1,8) == '0' ? ' ' : PV(3,1,8), PV(3,2,6) == '0' ? ' ' : PV(3,2,6), PV(3,2,7) == '0' ? ' ' : PV(3,2,7), PV(3,2,8) == '0' ? ' ' : PV(3,2,8), PV(3,3,6) == '0' ? ' ' : PV(3,3,6), PV(3,3,7) == '0' ? ' ' : PV(3,3,7), PV(3,3,8) == '0' ? ' ' : PV(3,3,8), PV(3,4,6) == '0' ? ' ' : PV(3,4,6), PV(3,4,7) == '0' ? ' ' : PV(3,4,7), PV(3,4,8) == '0' ? ' ' : PV(3,4,8), PV(3,5,6) == '0' ? ' ' : PV(3,5,6), PV(3,5,7) == '0' ? ' ' : PV(3,5,7), PV(3,5,8) == '0' ? ' ' : PV(3,5,8), PV(3,6,6) == '0' ? ' ' : PV(3,6,6), PV(3,6,7) == '0' ? ' ' : PV(3,6,7), PV(3,6,8) == '0' ? ' ' : PV(3,6,8), PV(3,7,6) == '0' ? ' ' : PV(3,7,6), PV(3,7,7) == '0' ? ' ' : PV(3,7,7), PV(3,7,8) == '0' ? ' ' : PV(3,7,8), PV(3,8,6) == '0' ? ' ' : PV(3,8,6), PV(3,8,7) == '0' ? ' ' : PV(3,8,7), PV(3,8,8) == '0' ? ' ' : PV(3,8,8));
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(4,0,0) == '0' ? ' ' : PV(4,0,0), PV(4,0,1) == '0' ? ' ' : PV(4,0,1), PV(4,0,2) == '0' ? ' ' : PV(4,0,2), PV(4,1,0) == '0' ? ' ' : PV(4,1,0), PV(4,1,1) == '0' ? ' ' : PV(4,1,1), PV(4,1,2) == '0' ? ' ' : PV(4,1,2), PV(4,2,0) == '0' ? ' ' : PV(4,2,0), PV(4,2,1) == '0' ? ' ' : PV(4,2,1), PV(4,2,2) == '0' ? ' ' : PV(4,2,2), PV(4,3,0) == '0' ? ' ' : PV(4,3,0), PV(4,3,1) == '0' ? ' ' : PV(4,3,1), PV(4,3,2) == '0' ? ' ' : PV(4,3,2), PV(4,4,0) == '0' ? ' ' : PV(4,4,0), PV(4,4,1) == '0' ? ' ' : PV(4,4,1), PV(4,4,2) == '0' ? ' ' : PV(4,4,2), PV(4,5,0) == '0' ? ' ' : PV(4,5,0), PV(4,5,1) == '0' ? ' ' : PV(4,5,1), PV(4,5,2) == '0' ? ' ' : PV(4,5,2), PV(4,6,0) == '0' ? ' ' : PV(4,6,0), PV(4,6,1) == '0' ? ' ' : PV(4,6,1), PV(4,6,2) == '0' ? ' ' : PV(4,6,2), PV(4,7,0) == '0' ? ' ' : PV(4,7,0), PV(4,7,1) == '0' ? ' ' : PV(4,7,1), PV(4,7,2) == '0' ? ' ' : PV(4,7,2), PV(4,8,0) == '0' ? ' ' : PV(4,8,0), PV(4,8,1) == '0' ? ' ' : PV(4,8,1), PV(4,8,2) == '0' ? ' ' : PV(4,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(4,0,3) == '0' ? ' ' : PV(4,0,3), PV(4,0,4) == '0' ? ' ' : PV(4,0,4), PV(4,0,5) == '0' ? ' ' : PV(4,0,5), PV(4,1,3) == '0' ? ' ' : PV(4,1,3), PV(4,1,4) == '0' ? ' ' : PV(4,1,4), PV(4,1,5) == '0' ? ' ' : PV(4,1,5), PV(4,2,3) == '0' ? ' ' : PV(4,2,3), PV(4,2,4) == '0' ? ' ' : PV(4,2,4), PV(4,2,5) == '0' ? ' ' : PV(4,2,5), PV(4,3,3) == '0' ? ' ' : PV(4,3,3), PV(4,3,4) == '0' ? ' ' : PV(4,3,4), PV(4,3,5) == '0' ? ' ' : PV(4,3,5), PV(4,4,3) == '0' ? ' ' : PV(4,4,3), PV(4,4,4) == '0' ? ' ' : PV(4,4,4), PV(4,4,5) == '0' ? ' ' : PV(4,4,5), PV(4,5,3) == '0' ? ' ' : PV(4,5,3), PV(4,5,4) == '0' ? ' ' : PV(4,5,4), PV(4,5,5) == '0' ? ' ' : PV(4,5,5), PV(4,6,3) == '0' ? ' ' : PV(4,6,3), PV(4,6,4) == '0' ? ' ' : PV(4,6,4), PV(4,6,5) == '0' ? ' ' : PV(4,6,5), PV(4,7,3) == '0' ? ' ' : PV(4,7,3), PV(4,7,4) == '0' ? ' ' : PV(4,7,4), PV(4,7,5) == '0' ? ' ' : PV(4,7,5), PV(4,8,3) == '0' ? ' ' : PV(4,8,3), PV(4,8,4) == '0' ? ' ' : PV(4,8,4), PV(4,8,5) == '0' ? ' ' : PV(4,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(4,0,6) == '0' ? ' ' : PV(4,0,6), PV(4,0,7) == '0' ? ' ' : PV(4,0,7), PV(4,0,8) == '0' ? ' ' : PV(4,0,8), PV(4,1,6) == '0' ? ' ' : PV(4,1,6), PV(4,1,7) == '0' ? ' ' : PV(4,1,7), PV(4,1,8) == '0' ? ' ' : PV(4,1,8), PV(4,2,6) == '0' ? ' ' : PV(4,2,6), PV(4,2,7) == '0' ? ' ' : PV(4,2,7), PV(4,2,8) == '0' ? ' ' : PV(4,2,8), PV(4,3,6) == '0' ? ' ' : PV(4,3,6), PV(4,3,7) == '0' ? ' ' : PV(4,3,7), PV(4,3,8) == '0' ? ' ' : PV(4,3,8), PV(4,4,6) == '0' ? ' ' : PV(4,4,6), PV(4,4,7) == '0' ? ' ' : PV(4,4,7), PV(4,4,8) == '0' ? ' ' : PV(4,4,8), PV(4,5,6) == '0' ? ' ' : PV(4,5,6), PV(4,5,7) == '0' ? ' ' : PV(4,5,7), PV(4,5,8) == '0' ? ' ' : PV(4,5,8), PV(4,6,6) == '0' ? ' ' : PV(4,6,6), PV(4,6,7) == '0' ? ' ' : PV(4,6,7), PV(4,6,8) == '0' ? ' ' : PV(4,6,8), PV(4,7,6) == '0' ? ' ' : PV(4,7,6), PV(4,7,7) == '0' ? ' ' : PV(4,7,7), PV(4,7,8) == '0' ? ' ' : PV(4,7,8), PV(4,8,6) == '0' ? ' ' : PV(4,8,6), PV(4,8,7) == '0' ? ' ' : PV(4,8,7), PV(4,8,8) == '0' ? ' ' : PV(4,8,8));
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(5,0,0) == '0' ? ' ' : PV(5,0,0), PV(5,0,1) == '0' ? ' ' : PV(5,0,1), PV(5,0,2) == '0' ? ' ' : PV(5,0,2), PV(5,1,0) == '0' ? ' ' : PV(5,1,0), PV(5,1,1) == '0' ? ' ' : PV(5,1,1), PV(5,1,2) == '0' ? ' ' : PV(5,1,2), PV(5,2,0) == '0' ? ' ' : PV(5,2,0), PV(5,2,1) == '0' ? ' ' : PV(5,2,1), PV(5,2,2) == '0' ? ' ' : PV(5,2,2), PV(5,3,0) == '0' ? ' ' : PV(5,3,0), PV(5,3,1) == '0' ? ' ' : PV(5,3,1), PV(5,3,2) == '0' ? ' ' : PV(5,3,2), PV(5,4,0) == '0' ? ' ' : PV(5,4,0), PV(5,4,1) == '0' ? ' ' : PV(5,4,1), PV(5,4,2) == '0' ? ' ' : PV(5,4,2), PV(5,5,0) == '0' ? ' ' : PV(5,5,0), PV(5,5,1) == '0' ? ' ' : PV(5,5,1), PV(5,5,2) == '0' ? ' ' : PV(5,5,2), PV(5,6,0) == '0' ? ' ' : PV(5,6,0), PV(5,6,1) == '0' ? ' ' : PV(5,6,1), PV(5,6,2) == '0' ? ' ' : PV(5,6,2), PV(5,7,0) == '0' ? ' ' : PV(5,7,0), PV(5,7,1) == '0' ? ' ' : PV(5,7,1), PV(5,7,2) == '0' ? ' ' : PV(5,7,2), PV(5,8,0) == '0' ? ' ' : PV(5,8,0), PV(5,8,1) == '0' ? ' ' : PV(5,8,1), PV(5,8,2) == '0' ? ' ' : PV(5,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(5,0,3) == '0' ? ' ' : PV(5,0,3), PV(5,0,4) == '0' ? ' ' : PV(5,0,4), PV(5,0,5) == '0' ? ' ' : PV(5,0,5), PV(5,1,3) == '0' ? ' ' : PV(5,1,3), PV(5,1,4) == '0' ? ' ' : PV(5,1,4), PV(5,1,5) == '0' ? ' ' : PV(5,1,5), PV(5,2,3) == '0' ? ' ' : PV(5,2,3), PV(5,2,4) == '0' ? ' ' : PV(5,2,4), PV(5,2,5) == '0' ? ' ' : PV(5,2,5), PV(5,3,3) == '0' ? ' ' : PV(5,3,3), PV(5,3,4) == '0' ? ' ' : PV(5,3,4), PV(5,3,5) == '0' ? ' ' : PV(5,3,5), PV(5,4,3) == '0' ? ' ' : PV(5,4,3), PV(5,4,4) == '0' ? ' ' : PV(5,4,4), PV(5,4,5) == '0' ? ' ' : PV(5,4,5), PV(5,5,3) == '0' ? ' ' : PV(5,5,3), PV(5,5,4) == '0' ? ' ' : PV(5,5,4), PV(5,5,5) == '0' ? ' ' : PV(5,5,5), PV(5,6,3) == '0' ? ' ' : PV(5,6,3), PV(5,6,4) == '0' ? ' ' : PV(5,6,4), PV(5,6,5) == '0' ? ' ' : PV(5,6,5), PV(5,7,3) == '0' ? ' ' : PV(5,7,3), PV(5,7,4) == '0' ? ' ' : PV(5,7,4), PV(5,7,5) == '0' ? ' ' : PV(5,7,5), PV(5,8,3) == '0' ? ' ' : PV(5,8,3), PV(5,8,4) == '0' ? ' ' : PV(5,8,4), PV(5,8,5) == '0' ? ' ' : PV(5,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(5,0,6) == '0' ? ' ' : PV(5,0,6), PV(5,0,7) == '0' ? ' ' : PV(5,0,7), PV(5,0,8) == '0' ? ' ' : PV(5,0,8), PV(5,1,6) == '0' ? ' ' : PV(5,1,6), PV(5,1,7) == '0' ? ' ' : PV(5,1,7), PV(5,1,8) == '0' ? ' ' : PV(5,1,8), PV(5,2,6) == '0' ? ' ' : PV(5,2,6), PV(5,2,7) == '0' ? ' ' : PV(5,2,7), PV(5,2,8) == '0' ? ' ' : PV(5,2,8), PV(5,3,6) == '0' ? ' ' : PV(5,3,6), PV(5,3,7) == '0' ? ' ' : PV(5,3,7), PV(5,3,8) == '0' ? ' ' : PV(5,3,8), PV(5,4,6) == '0' ? ' ' : PV(5,4,6), PV(5,4,7) == '0' ? ' ' : PV(5,4,7), PV(5,4,8) == '0' ? ' ' : PV(5,4,8), PV(5,5,6) == '0' ? ' ' : PV(5,5,6), PV(5,5,7) == '0' ? ' ' : PV(5,5,7), PV(5,5,8) == '0' ? ' ' : PV(5,5,8), PV(5,6,6) == '0' ? ' ' : PV(5,6,6), PV(5,6,7) == '0' ? ' ' : PV(5,6,7), PV(5,6,8) == '0' ? ' ' : PV(5,6,8), PV(5,7,6) == '0' ? ' ' : PV(5,7,6), PV(5,7,7) == '0' ? ' ' : PV(5,7,7), PV(5,7,8) == '0' ? ' ' : PV(5,7,8), PV(5,8,6) == '0' ? ' ' : PV(5,8,6), PV(5,8,7) == '0' ? ' ' : PV(5,8,7), PV(5,8,8) == '0' ? ' ' : PV(5,8,8));
    printf("┣━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━╋━━━━━┿━━━━━┿━━━━━┫\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(6,0,0) == '0' ? ' ' : PV(6,0,0), PV(6,0,1) == '0' ? ' ' : PV(6,0,1), PV(6,0,2) == '0' ? ' ' : PV(6,0,2), PV(6,1,0) == '0' ? ' ' : PV(6,1,0), PV(6,1,1) == '0' ? ' ' : PV(6,1,1), PV(6,1,2) == '0' ? ' ' : PV(6,1,2), PV(6,2,0) == '0' ? ' ' : PV(6,2,0), PV(6,2,1) == '0' ? ' ' : PV(6,2,1), PV(6,2,2) == '0' ? ' ' : PV(6,2,2), PV(6,3,0) == '0' ? ' ' : PV(6,3,0), PV(6,3,1) == '0' ? ' ' : PV(6,3,1), PV(6,3,2) == '0' ? ' ' : PV(6,3,2), PV(6,4,0) == '0' ? ' ' : PV(6,4,0), PV(6,4,1) == '0' ? ' ' : PV(6,4,1), PV(6,4,2) == '0' ? ' ' : PV(6,4,2), PV(6,5,0) == '0' ? ' ' : PV(6,5,0), PV(6,5,1) == '0' ? ' ' : PV(6,5,1), PV(6,5,2) == '0' ? ' ' : PV(6,5,2), PV(6,6,0) == '0' ? ' ' : PV(6,6,0), PV(6,6,1) == '0' ? ' ' : PV(6,6,1), PV(6,6,2) == '0' ? ' ' : PV(6,6,2), PV(6,7,0) == '0' ? ' ' : PV(6,7,0), PV(6,7,1) == '0' ? ' ' : PV(6,7,1), PV(6,7,2) == '0' ? ' ' : PV(6,7,2), PV(6,8,0) == '0' ? ' ' : PV(6,8,0), PV(6,8,1) == '0' ? ' ' : PV(6,8,1), PV(6,8,2) == '0' ? ' ' : PV(6,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(6,0,3) == '0' ? ' ' : PV(6,0,3), PV(6,0,4) == '0' ? ' ' : PV(6,0,4), PV(6,0,5) == '0' ? ' ' : PV(6,0,5), PV(6,1,3) == '0' ? ' ' : PV(6,1,3), PV(6,1,4) == '0' ? ' ' : PV(6,1,4), PV(6,1,5) == '0' ? ' ' : PV(6,1,5), PV(6,2,3) == '0' ? ' ' : PV(6,2,3), PV(6,2,4) == '0' ? ' ' : PV(6,2,4), PV(6,2,5) == '0' ? ' ' : PV(6,2,5), PV(6,3,3) == '0' ? ' ' : PV(6,3,3), PV(6,3,4) == '0' ? ' ' : PV(6,3,4), PV(6,3,5) == '0' ? ' ' : PV(6,3,5), PV(6,4,3) == '0' ? ' ' : PV(6,4,3), PV(6,4,4) == '0' ? ' ' : PV(6,4,4), PV(6,4,5) == '0' ? ' ' : PV(6,4,5), PV(6,5,3) == '0' ? ' ' : PV(6,5,3), PV(6,5,4) == '0' ? ' ' : PV(6,5,4), PV(6,5,5) == '0' ? ' ' : PV(6,5,5), PV(6,6,3) == '0' ? ' ' : PV(6,6,3), PV(6,6,4) == '0' ? ' ' : PV(6,6,4), PV(6,6,5) == '0' ? ' ' : PV(6,6,5), PV(6,7,3) == '0' ? ' ' : PV(6,7,3), PV(6,7,4) == '0' ? ' ' : PV(6,7,4), PV(6,7,5) == '0' ? ' ' : PV(6,7,5), PV(6,8,3) == '0' ? ' ' : PV(6,8,3), PV(6,8,4) == '0' ? ' ' : PV(6,8,4), PV(6,8,5) == '0' ? ' ' : PV(6,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(6,0,6) == '0' ? ' ' : PV(6,0,6), PV(6,0,7) == '0' ? ' ' : PV(6,0,7), PV(6,0,8) == '0' ? ' ' : PV(6,0,8), PV(6,1,6) == '0' ? ' ' : PV(6,1,6), PV(6,1,7) == '0' ? ' ' : PV(6,1,7), PV(6,1,8) == '0' ? ' ' : PV(6,1,8), PV(6,2,6) == '0' ? ' ' : PV(6,2,6), PV(6,2,7) == '0' ? ' ' : PV(6,2,7), PV(6,2,8) == '0' ? ' ' : PV(6,2,8), PV(6,3,6) == '0' ? ' ' : PV(6,3,6), PV(6,3,7) == '0' ? ' ' : PV(6,3,7), PV(6,3,8) == '0' ? ' ' : PV(6,3,8), PV(6,4,6) == '0' ? ' ' : PV(6,4,6), PV(6,4,7) == '0' ? ' ' : PV(6,4,7), PV(6,4,8) == '0' ? ' ' : PV(6,4,8), PV(6,5,6) == '0' ? ' ' : PV(6,5,6), PV(6,5,7) == '0' ? ' ' : PV(6,5,7), PV(6,5,8) == '0' ? ' ' : PV(6,5,8), PV(6,6,6) == '0' ? ' ' : PV(6,6,6), PV(6,6,7) == '0' ? ' ' : PV(6,6,7), PV(6,6,8) == '0' ? ' ' : PV(6,6,8), PV(6,7,6) == '0' ? ' ' : PV(6,7,6), PV(6,7,7) == '0' ? ' ' : PV(6,7,7), PV(6,7,8) == '0' ? ' ' : PV(6,7,8), PV(6,8,6) == '0' ? ' ' : PV(6,8,6), PV(6,8,7) == '0' ? ' ' : PV(6,8,7), PV(6,8,8) == '0' ? ' ' : PV(6,8,8));
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(7,0,0) == '0' ? ' ' : PV(7,0,0), PV(7,0,1) == '0' ? ' ' : PV(7,0,1), PV(7,0,2) == '0' ? ' ' : PV(7,0,2), PV(7,1,0) == '0' ? ' ' : PV(7,1,0), PV(7,1,1) == '0' ? ' ' : PV(7,1,1), PV(7,1,2) == '0' ? ' ' : PV(7,1,2), PV(7,2,0) == '0' ? ' ' : PV(7,2,0), PV(7,2,1) == '0' ? ' ' : PV(7,2,1), PV(7,2,2) == '0' ? ' ' : PV(7,2,2), PV(7,3,0) == '0' ? ' ' : PV(7,3,0), PV(7,3,1) == '0' ? ' ' : PV(7,3,1), PV(7,3,2) == '0' ? ' ' : PV(7,3,2), PV(7,4,0) == '0' ? ' ' : PV(7,4,0), PV(7,4,1) == '0' ? ' ' : PV(7,4,1), PV(7,4,2) == '0' ? ' ' : PV(7,4,2), PV(7,5,0) == '0' ? ' ' : PV(7,5,0), PV(7,5,1) == '0' ? ' ' : PV(7,5,1), PV(7,5,2) == '0' ? ' ' : PV(7,5,2), PV(7,6,0) == '0' ? ' ' : PV(7,6,0), PV(7,6,1) == '0' ? ' ' : PV(7,6,1), PV(7,6,2) == '0' ? ' ' : PV(7,6,2), PV(7,7,0) == '0' ? ' ' : PV(7,7,0), PV(7,7,1) == '0' ? ' ' : PV(7,7,1), PV(7,7,2) == '0' ? ' ' : PV(7,7,2), PV(7,8,0) == '0' ? ' ' : PV(7,8,0), PV(7,8,1) == '0' ? ' ' : PV(7,8,1), PV(7,8,2) == '0' ? ' ' : PV(7,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(7,0,3) == '0' ? ' ' : PV(7,0,3), PV(7,0,4) == '0' ? ' ' : PV(7,0,4), PV(7,0,5) == '0' ? ' ' : PV(7,0,5), PV(7,1,3) == '0' ? ' ' : PV(7,1,3), PV(7,1,4) == '0' ? ' ' : PV(7,1,4), PV(7,1,5) == '0' ? ' ' : PV(7,1,5), PV(7,2,3) == '0' ? ' ' : PV(7,2,3), PV(7,2,4) == '0' ? ' ' : PV(7,2,4), PV(7,2,5) == '0' ? ' ' : PV(7,2,5), PV(7,3,3) == '0' ? ' ' : PV(7,3,3), PV(7,3,4) == '0' ? ' ' : PV(7,3,4), PV(7,3,5) == '0' ? ' ' : PV(7,3,5), PV(7,4,3) == '0' ? ' ' : PV(7,4,3), PV(7,4,4) == '0' ? ' ' : PV(7,4,4), PV(7,4,5) == '0' ? ' ' : PV(7,4,5), PV(7,5,3) == '0' ? ' ' : PV(7,5,3), PV(7,5,4) == '0' ? ' ' : PV(7,5,4), PV(7,5,5) == '0' ? ' ' : PV(7,5,5), PV(7,6,3) == '0' ? ' ' : PV(7,6,3), PV(7,6,4) == '0' ? ' ' : PV(7,6,4), PV(7,6,5) == '0' ? ' ' : PV(7,6,5), PV(7,7,3) == '0' ? ' ' : PV(7,7,3), PV(7,7,4) == '0' ? ' ' : PV(7,7,4), PV(7,7,5) == '0' ? ' ' : PV(7,7,5), PV(7,8,3) == '0' ? ' ' : PV(7,8,3), PV(7,8,4) == '0' ? ' ' : PV(7,8,4), PV(7,8,5) == '0' ? ' ' : PV(7,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(7,0,6) == '0' ? ' ' : PV(7,0,6), PV(7,0,7) == '0' ? ' ' : PV(7,0,7), PV(7,0,8) == '0' ? ' ' : PV(7,0,8), PV(7,1,6) == '0' ? ' ' : PV(7,1,6), PV(7,1,7) == '0' ? ' ' : PV(7,1,7), PV(7,1,8) == '0' ? ' ' : PV(7,1,8), PV(7,2,6) == '0' ? ' ' : PV(7,2,6), PV(7,2,7) == '0' ? ' ' : PV(7,2,7), PV(7,2,8) == '0' ? ' ' : PV(7,2,8), PV(7,3,6) == '0' ? ' ' : PV(7,3,6), PV(7,3,7) == '0' ? ' ' : PV(7,3,7), PV(7,3,8) == '0' ? ' ' : PV(7,3,8), PV(7,4,6) == '0' ? ' ' : PV(7,4,6), PV(7,4,7) == '0' ? ' ' : PV(7,4,7), PV(7,4,8) == '0' ? ' ' : PV(7,4,8), PV(7,5,6) == '0' ? ' ' : PV(7,5,6), PV(7,5,7) == '0' ? ' ' : PV(7,5,7), PV(7,5,8) == '0' ? ' ' : PV(7,5,8), PV(7,6,6) == '0' ? ' ' : PV(7,6,6), PV(7,6,7) == '0' ? ' ' : PV(7,6,7), PV(7,6,8) == '0' ? ' ' : PV(7,6,8), PV(7,7,6) == '0' ? ' ' : PV(7,7,6), PV(7,7,7) == '0' ? ' ' : PV(7,7,7), PV(7,7,8) == '0' ? ' ' : PV(7,7,8), PV(7,8,6) == '0' ? ' ' : PV(7,8,6), PV(7,8,7) == '0' ? ' ' : PV(7,8,7), PV(7,8,8) == '0' ? ' ' : PV(7,8,8));
    printf("┠┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈╂┈┈┈┈┈┼┈┈┈┈┈┼┈┈┈┈┈┨\n");
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(8,0,0) == '0' ? ' ' : PV(8,0,0), PV(8,0,1) == '0' ? ' ' : PV(8,0,1), PV(8,0,2) == '0' ? ' ' : PV(8,0,2), PV(8,1,0) == '0' ? ' ' : PV(8,1,0), PV(8,1,1) == '0' ? ' ' : PV(8,1,1), PV(8,1,2) == '0' ? ' ' : PV(8,1,2), PV(8,2,0) == '0' ? ' ' : PV(8,2,0), PV(8,2,1) == '0' ? ' ' : PV(8,2,1), PV(8,2,2) == '0' ? ' ' : PV(8,2,2), PV(8,3,0) == '0' ? ' ' : PV(8,3,0), PV(8,3,1) == '0' ? ' ' : PV(8,3,1), PV(8,3,2) == '0' ? ' ' : PV(8,3,2), PV(8,4,0) == '0' ? ' ' : PV(8,4,0), PV(8,4,1) == '0' ? ' ' : PV(8,4,1), PV(8,4,2) == '0' ? ' ' : PV(8,4,2), PV(8,5,0) == '0' ? ' ' : PV(8,5,0), PV(8,5,1) == '0' ? ' ' : PV(8,5,1), PV(8,5,2) == '0' ? ' ' : PV(8,5,2), PV(8,6,0) == '0' ? ' ' : PV(8,6,0), PV(8,6,1) == '0' ? ' ' : PV(8,6,1), PV(8,6,2) == '0' ? ' ' : PV(8,6,2), PV(8,7,0) == '0' ? ' ' : PV(8,7,0), PV(8,7,1) == '0' ? ' ' : PV(8,7,1), PV(8,7,2) == '0' ? ' ' : PV(8,7,2), PV(8,8,0) == '0' ? ' ' : PV(8,8,0), PV(8,8,1) == '0' ? ' ' : PV(8,8,1), PV(8,8,2) == '0' ? ' ' : PV(8,8,2));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(8,0,3) == '0' ? ' ' : PV(8,0,3), PV(8,0,4) == '0' ? ' ' : PV(8,0,4), PV(8,0,5) == '0' ? ' ' : PV(8,0,5), PV(8,1,3) == '0' ? ' ' : PV(8,1,3), PV(8,1,4) == '0' ? ' ' : PV(8,1,4), PV(8,1,5) == '0' ? ' ' : PV(8,1,5), PV(8,2,3) == '0' ? ' ' : PV(8,2,3), PV(8,2,4) == '0' ? ' ' : PV(8,2,4), PV(8,2,5) == '0' ? ' ' : PV(8,2,5), PV(8,3,3) == '0' ? ' ' : PV(8,3,3), PV(8,3,4) == '0' ? ' ' : PV(8,3,4), PV(8,3,5) == '0' ? ' ' : PV(8,3,5), PV(8,4,3) == '0' ? ' ' : PV(8,4,3), PV(8,4,4) == '0' ? ' ' : PV(8,4,4), PV(8,4,5) == '0' ? ' ' : PV(8,4,5), PV(8,5,3) == '0' ? ' ' : PV(8,5,3), PV(8,5,4) == '0' ? ' ' : PV(8,5,4), PV(8,5,5) == '0' ? ' ' : PV(8,5,5), PV(8,6,3) == '0' ? ' ' : PV(8,6,3), PV(8,6,4) == '0' ? ' ' : PV(8,6,4), PV(8,6,5) == '0' ? ' ' : PV(8,6,5), PV(8,7,3) == '0' ? ' ' : PV(8,7,3), PV(8,7,4) == '0' ? ' ' : PV(8,7,4), PV(8,7,5) == '0' ? ' ' : PV(8,7,5), PV(8,8,3) == '0' ? ' ' : PV(8,8,3), PV(8,8,4) == '0' ? ' ' : PV(8,8,4), PV(8,8,5) == '0' ? ' ' : PV(8,8,5));
    printf("┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃ %c%c%c ┊ %c%c%c ┊ %c%c%c ┃\n", PV(8,0,6) == '0' ? ' ' : PV(8,0,6), PV(8,0,7) == '0' ? ' ' : PV(8,0,7), PV(8,0,8) == '0' ? ' ' : PV(8,0,8), PV(8,1,6) == '0' ? ' ' : PV(8,1,6), PV(8,1,7) == '0' ? ' ' : PV(8,1,7), PV(8,1,8) == '0' ? ' ' : PV(8,1,8), PV(8,2,6) == '0' ? ' ' : PV(8,2,6), PV(8,2,7) == '0' ? ' ' : PV(8,2,7), PV(8,2,8) == '0' ? ' ' : PV(8,2,8), PV(8,3,6) == '0' ? ' ' : PV(8,3,6), PV(8,3,7) == '0' ? ' ' : PV(8,3,7), PV(8,3,8) == '0' ? ' ' : PV(8,3,8), PV(8,4,6) == '0' ? ' ' : PV(8,4,6), PV(8,4,7) == '0' ? ' ' : PV(8,4,7), PV(8,4,8) == '0' ? ' ' : PV(8,4,8), PV(8,5,6) == '0' ? ' ' : PV(8,5,6), PV(8,5,7) == '0' ? ' ' : PV(8,5,7), PV(8,5,8) == '0' ? ' ' : PV(8,5,8), PV(8,6,6) == '0' ? ' ' : PV(8,6,6), PV(8,6,7) == '0' ? ' ' : PV(8,6,7), PV(8,6,8) == '0' ? ' ' : PV(8,6,8), PV(8,7,6) == '0' ? ' ' : PV(8,7,6), PV(8,7,7) == '0' ? ' ' : PV(8,7,7), PV(8,7,8) == '0' ? ' ' : PV(8,7,8), PV(8,8,6) == '0' ? ' ' : PV(8,8,6), PV(8,8,7) == '0' ? ' ' : PV(8,8,7), PV(8,8,8) == '0' ? ' ' : PV(8,8,8));
    printf("┗━━━━━┷━━━━━┷━━━━━┻━━━━━┷━━━━━┷━━━━━┻━━━━━┷━━━━━┷━━━━━┛\n");
}

void solve_sudoku(unsigned short int input[9][9], unsigned short int output[9][9]) {
    init_candidates();
    
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
