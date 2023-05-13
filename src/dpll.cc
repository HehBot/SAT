#include "dpll.h"

#ifdef DEBUG
    #include <iostream>
#endif
#include <map>
#include <random>
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
std::size_t dpll::choose(std::vector<value>& m)
{
    std::size_t i;
    std::vector<std::size_t> candidates;
    for (i = 0; i < m.size(); ++i) {
        if (m[i] == value::u)
            candidates.push_back(i);
    }
    if (candidates.size() == 0)
        return m.size();
    static std::random_device rd;
    static std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, candidates.size() - 1);
    return candidates[distr(eng)];
}

bool dpll::is_sat(std::vector<value>& model) const
{
    {
        value V = evaluate(s, model);
#ifdef DEBUG
        std::cout << prefix << "Evaluate: " << (V == value::u ? "unassigned" : V == value::t ? "1"
                                                                                             : "0")
                  << '\n';
#endif
        if (V != value::u)
            return (V == value::t);
    }

    unit_propagate(model);

    std::size_t v = choose(model);
    if (v == model.size()) {
        value V = evaluate(s, model);
#ifdef DEBUG
        std::cout << prefix << "Evaluate: " << (V == value::u ? "unassigned" : V == value::t ? "1"
                                                                                             : "0")
                  << '\n';
#endif
        return (V == value::t);
    }

    std::vector<value> m = model;

    static std::random_device rd;
    static std::mt19937 eng(rd());
    static std::uniform_int_distribution<> distr(0, 1);
    bool original = distr(eng);
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
    ;
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
