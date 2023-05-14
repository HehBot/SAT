#ifndef DPLL_H
#define DPLL_H

#include "sat_solver.h"

#include <map>
#include <vector>

class dpll : public sat_solver {
    void unit_propagate(std::vector<value>& m) const;
    bool is_sat(std::vector<value>& model) const;

public:
    dpll(char const* filename)
        : sat_solver(filename)
    {
    }
    dpll(std::set<std::set<literal>> s, std::size_t no_of_var)
        : sat_solver(s, no_of_var)
    {
    }
    virtual bool is_sat(std::map<std::string, bool>& model) const override;
};

#endif // DPLL_H
