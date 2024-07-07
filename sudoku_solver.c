#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "sudoku_solver.h"

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

void clear_number_from_9box(unsigned short int n, unsigned short int i, unsigned short int j) {
    unsigned short int i_start, i_end, j_start, j_end;
    i_start = (i / 3) * 3;
    i_end = i_start + 2;
    j_start = (j / 3) * 3;
    j_end = j_start + 2;
    
    assert(i_start == 0 || i_start == 3 || i_start == 6 && "Calculation for indices in 9box are wrong");
    assert(j_start == 0 || j_start == 3 || j_start == 6 && "Calculation for indices in 9box are wrong");
    assert(i_end == 2 || i_end == 5 || i_end == 8 && "Calculation for indices in 9box are wrong");
    assert(j_end == 2 || j_end == 5 || j_end == 8 && "Calculation for indices in 9box are wrong");

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
    clear_number_from_9box(n, i, j);
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

    for (size_t x = 0; x < 10; x++)
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
    }
}
