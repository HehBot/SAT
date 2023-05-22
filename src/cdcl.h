#ifndef CDCL_H
#define CDCL_H

#include "impl.h"
#include "sat_solver.h"

#include <map>
#include <string>

class cdcl : public sat_solver {
    void unit_propagate(std::vector<value>& m, impl& graph) const;
    bool is_sat(std::vector<value>& model) const;

public:
    cdcl() = default;
    cdcl(std::set<std::set<literal>> const& s)
        : sat_solver(s)
    {
    }
    virtual bool is_sat(std::map<std::string, bool>& model) const override;
};

#endif // CDCL_H
