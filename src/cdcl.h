#ifndef CDCL_H
#define CDCL_H

#include "impl.h"
#include "sat_solver.h"

#include <map>
#include <string>

class cdcl : public sat_solver {
    value unit_propagate(std::vector<value>& m, impl& graph, std::set<literal>& erring_clause) const;

    bool is_sat(std::vector<value>& model) const;

public:
    cdcl(char const* filename)
        : sat_solver(filename)
    {
    }
    cdcl(std::set<std::set<literal>> s, std::size_t no_of_var)
        : sat_solver(s, no_of_var)
    {
    }
    virtual bool is_sat(std::map<std::string, bool>& model) const override;
};

#endif // CDCL_H
