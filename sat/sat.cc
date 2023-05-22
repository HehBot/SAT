#include <cdcl.h>
#include <dpll.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 3 && argc != 4) {
        std::cerr << "Usage: " << argv[0] << " dpll <SAT file>\n";
        std::cerr << "       " << argv[0] << " cdcl <SAT file> [SAT file for learned clauses]\n";
        return -1;
    }

    sat_solver* solver;
    bool write_learned_to_file = false;
    bool read_learned_from_file = false;
    if (std::string(argv[1]) == "dpll") {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " dpll <SAT file>\n";
            return -1;
        }
        solver = new dpll;
    } else if (std::string(argv[1]) == "cdcl") {
        solver = new cdcl;
        if (argc == 4) {
            write_learned_to_file = true;
            std::ifstream f(argv[3]);
            if (f.good()) {
                f.close();
                solver->add_clauses_from_file(argv[3]);
            }
        }
    } else {
        std::cerr << "Unknown algorithm " << argv[1] << '\n';
        std::cerr << "Usage: " << argv[0] << " dpll <SAT file>\n";
        std::cerr << "       " << argv[0] << " cdcl <SAT file> [SAT file for learned clauses]\n";
        return -1;
    }

    solver->add_clauses_from_file(argv[2]);
    std::map<std::string, bool> m;

    bool is_sat = solver->is_sat(m);

    if (is_sat) {
        std::cout << "SAT\n";
        for (auto i : m)
            std::cout << i.first << " = " << i.second << '\n';
    } else
        std::cout << "UNSAT\n";

    if (write_learned_to_file)
        solver->write_to_file(argv[3]);
    delete solver;

    return (is_sat ? 0 : 1);
}
