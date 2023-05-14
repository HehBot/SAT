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
    if (std::string(argv[1]) == "sudoku") {
        std::vector<std::vector<std::size_t>> board = read_board(argv[3]);
        print_board(board);

        if (sudoku_solver(board)) {
            std::cout << "Solution:\n";
            print_board(board);
        } else
            std::cout << "Unsolvable board\n";
    } else if (std::string(argv[1]) == "sat") {
        cdcl solver(argv[3]);
        solver.print();
        std::map<std::string, bool> m;
        if (solver.is_sat(m)) {
            std::cout << "SAT\n";
            for (auto i : m) {
                std::cout << i.first << " = " << i.second << '\n';
            }
        } else {
            std::cout << "UNSAT\n";
        }
    }

    return 0;
}
