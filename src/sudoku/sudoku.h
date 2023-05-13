#ifndef SUDOKU_H
#define SUDOKU_H

#include <sat_solver.h>
#include <vector>

bool sudoku_solver(std::vector<std::vector<std::size_t>>& board);
void print_board(std::vector<std::vector<std::size_t>> const& board);
std::vector<std::vector<std::size_t>> read_board(char const* s);

#endif // SUDOKU_H
