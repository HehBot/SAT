#ifndef SUDOKU_H
#define SUDOKU_H

#include "../sat_solver.h"

#include <vector>

bool sudoku_solver(std::vector<std::vector<std::size_t>>& board);
void print_board(std::vector<std::vector<std::size_t>> const& board);
std::vector<std::vector<std::size_t>> create_board(char const* s);

void sum_equals_one(std::set<std::set<sat_solver::literal>>& s, std::vector<std::size_t> const& vars);

#endif // SUDOKU_H
