#include "cdcl.h"
#include "dpll.h"
#include "sudoku/sudoku.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " (sat|sudoku) (dpll|cdcl) <SAT file/sudoku file>\n";
        return 1;
    }

    sat_solver* solver;
    if (std::string(argv[2]) == "dpll")
        solver = new dpll;
    else if (std::string(argv[2]) == "cdcl")
        solver = new cdcl;
    else {
        std::cerr << "Unknown algorithm " << argv[2] << '\n';
        return 1;
    }

    if (std::string(argv[1]) == "sudoku") {
        std::vector<std::vector<std::size_t>> board = read_board(argv[3]);
        print_board(board);
        if (sudoku_solver(board, solver)) {
            std::cout << "Solution:\n";
            print_board(board);
        } else
            std::cout << "Unsolvable board\n";
    } else if (std::string(argv[1]) == "sat") {
        solver->add_clauses_from_file(argv[3]);
        std::map<std::string, bool> m;
        if (solver->is_sat(m)) {
            std::cout << "SAT\n";
            for (auto i : m) {
                std::cout << i.first << " = " << i.second << '\n';
            }
        } else
            std::cout << "UNSAT\n";
    }

    delete solver;

    return 0;
}
