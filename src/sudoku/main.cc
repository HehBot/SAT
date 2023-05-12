#include "dpll.h"
#include "sat_solver.h"
#include "sudoku.h"

#include <iostream>
#include <map>
#include <vector>

extern std::size_t next_var;

int main(int argc, char* argv[])
{
    if (argc == 2) {
        dpll S(argv[1]);
        S.print();
        std::map<std::string, bool> M;
        if (S.is_sat(M)) {
            std::cout << "SAT\n";
            std::map<std::string, bool>::iterator it;
            for (it = M.begin(); it != M.end(); ++it)
                std::cout << it->first << " = " << it->second << '\n';
        } else
            std::cout << "UNSAT\n";
    } else if (argc == 1) {
        std::vector<std::vector<std::size_t>> board = create_board(
            R"BOARD(
000020040
008005000
000070602
031040070
200000000
000501203
049000730
000000010
800004000
)BOARD");
        print_board(board);

        if (sudoku_solver(board)) {
            std::cout << "Solution:\n";
            print_board(board);
        } else
            std::cout << "Unsolvable board\n";
    } else {
        std::cerr << "Usage: " << argv[0] << " <SAT file>\n";
        return 1;
    }

    return 0;
}
