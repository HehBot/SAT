#include "dpll.h"

#ifdef DEBUG
    #include <iostream>
#endif
#include <map>
#include <set>
#include <vector>

#ifdef DEBUG
static std::string prefix = "";
#endif

void dpll::unit_propagate(std::vector<value>& m) const
{
    bool changed;
    do {
        changed = false;
        for (auto const& c : s) {
            bool has_unit_lit = false, found_unassigned_lit = false;
            literal unit_lit;
            for (literal const& l : c) {
                if (m[l.i] == value::u) {
                    if (!has_unit_lit && !found_unassigned_lit) {
                        unit_lit = l;
                        has_unit_lit = true;
                        found_unassigned_lit = true;
                    } else if (found_unassigned_lit)
                        has_unit_lit = false;
                } else if ((m[l.i] == value::f) == l.is_neg) {
                    has_unit_lit = false;
                    break;
                }
            }

            if (has_unit_lit) {
                changed = true;
                m[unit_lit.i] = (unit_lit.is_neg ? value::f : value::t);
#ifdef DEBUG
                std::cout << prefix << "UnitProp: " << var_name[unit_lit.i] << " = " << !unit_lit.is_neg << '\n';
#endif
            }
        }
    } while (changed);
}

bool dpll::is_sat(std::vector<value>& model) const
{
    unit_propagate(model);

    std::size_t v = choose(model);
    std::set<literal> erring_clause;
    if (v == model.size()) {
        value V = evaluate(model, erring_clause);
        return (V == value::t);
    }

    std::vector<value> m = model;
    bool original = rand_bool();
    m[v] = (original ? value::t : value::f);

#ifdef DEBUG
    std::string curr_prefix = prefix;
    std::cout << prefix << "Branch: " << var_name[v] << " = " << (original ? 1 : 0) << '\n';
    prefix += "    ";
#endif

    if (is_sat(m)) {
        model = m;
        return true;
    }

#ifdef DEBUG
    std::cout << curr_prefix << "Backtrack: " << var_name[v] << " = " << (!original ? 1 : 0) << '\n';
    prefix = curr_prefix + "    ";
#endif

    model[v] = (!original ? value::t : value::f);
    return is_sat(model);
}

bool dpll::is_sat(std::map<std::string, bool>& model) const
{
    std::vector<value> v(var_name.size(), value::u);
    if (!is_sat(v))
        return false;
    model.clear();
    for (std::size_t i = 0; i < v.size(); ++i)
        model[var_name[i]] = (v[i] == value::t);
    return true;
}
