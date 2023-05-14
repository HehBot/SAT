#include "sudoku.h"

#include <cdcl.h>
#include <cmath>
#include <dpll.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using literal = sat_solver::literal;

static std::size_t next_var = 0;

static void sum_equals_one(std::set<std::set<literal>>& s, std::vector<std::size_t> const& vars)
{
    literal si(next_var++);
    literal siprev;
    s.insert(std::set({ literal(vars[0]).neg(), si }));

    std::set<literal> at_least_one_true({ literal(vars[0]) });

    for (std::size_t i = 1; i < vars.size() - 1; ++i) {
        siprev = si;
        si.i = next_var++;

        std::set<std::set<literal>> X({ { literal(vars[i]).neg(), si }, { literal(siprev).neg(), si }, { siprev.neg(), literal(vars[i]).neg() } });
        s.insert(X.begin(), X.end());

        at_least_one_true.insert(literal(vars[i]));
    }
    at_least_one_true.insert(literal(vars[vars.size() - 1]));

    s.insert(at_least_one_true);
    s.insert(std::set({ si.neg(), literal(vars[vars.size() - 1]).neg() }));
}

inline std::size_t v(std::size_t i, std::size_t j, std::size_t k, std::size_t N)
{
    return k + N * (j + N * i);
}

bool sudoku_solver(std::vector<std::vector<std::size_t>>& board, sat_solver* solver)
{
    std::size_t const n2 = board.size();
    std::size_t const n = std::sqrt(n2);

    std::set<std::set<literal>> s;

    std::size_t i = 0, j;
    for (std::vector<std::size_t> const& row : board) {
        j = 0;
        for (std::size_t k : row) {
            if (k != 0)
                s.insert(std::set { literal(v(i, j, k - 1, n2)) });
            ++j;
        }
        ++i;
    }

    std::vector<std::size_t> v1, v2, v3;
    next_var = n2 * n2 * n2;

    for (std::size_t i = 0; i < n2; ++i) {
        for (std::size_t j = 0; j < n2; ++j) {
            v1.clear();
            v2.clear();
            v3.clear();
            for (std::size_t k = 0; k < n2; ++k) {
                v1.push_back(v(k, i, j, n2));
                v2.push_back(v(j, k, i, n2));
                v3.push_back(v(i, j, k, n2));
            }
            sum_equals_one(s, v1);
            sum_equals_one(s, v2);
            sum_equals_one(s, v3);
        }
    }

    for (std::size_t k = 0; k < n2; ++k) {
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                v1.clear();
                for (std::size_t s = 0; s < n; ++s)
                    for (std::size_t r = 0; r < n; ++r)
                        v1.push_back(v(n * i + r, n * j + s, k, n2));
                sum_equals_one(s, v1);
            }
        }
    }

    solver->add_clauses(s);
    std::map<std::string, bool> ans;
    bool sat = solver->is_sat(ans);

    if (sat) {
        for (std::size_t i = 0; i < n2; ++i) {
            for (std::size_t j = 0; j < n2; ++j) {
                for (std::size_t k = 0; k < n2; ++k) {
                    if (ans[std::to_string(v(i, j, k, n2))]) {
                        board[i][j] = k + 1;
                        continue;
                    }
                }
            }
        }
    }

    return sat;
}

void print_board(std::vector<std::vector<std::size_t>> const& board)
{
    for (auto const& row : board) {
        for (std::size_t k : row) {
            if (k != 0)
                std::cout << k << ' ';
            else
                std::cout << ". ";
        }
        std::cout << '\n';
    }
}

static std::size_t char_to_num(char c)
{
    if (c <= '9' && c >= '0')
        return c - '0';
    else if (c <= 'z' && c >= 'a')
        return (c - 'a') + 10;
    throw std::invalid_argument("Bad sudoku file");
}

std::vector<std::vector<std::size_t>> read_board(char const* s)
{
    std::vector<std::size_t> buf;
    std::ifstream f(s);
    std::size_t z;

    while (f >> z)
        buf.push_back(z);

    std::size_t const n4 = buf.size();
    std::size_t n2 = std::sqrt(n4);

    std::vector<std::vector<std::size_t>> board(n2);
    for (std::size_t i = 0; i < n2; ++i)
        for (std::size_t j = 0; j < n2; ++j)
            board[i].push_back(buf[n2 * i + j]);
    return board;
}
