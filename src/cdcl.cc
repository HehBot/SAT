#include "cdcl.h"

#include "impl.h"

#ifdef DEBUG
    #include <iostream>
#endif
#include <string>
#include <vector>

#ifdef DEBUG
std::string prefix = "";
#endif

// bool unit_propagate(std::vector<value>& m, impl& i, std::set<literal>& c)
//      Unit propagates and updates i
//      Returns
//          value::f if unit propagation causes contradiction, c is set to erring clause
//          value::t if m satisfies the formula
//          value::u if formula is unassigned

sat_solver::value cdcl::unit_propagate(std::vector<value>& m, impl& graph, std::set<literal>& erring_clause) const
{
    // unit propagating maximally
    bool changed;
    do {
        changed = false;
        for (auto const& c : s) {
            literal unit_lit;
            bool has_unit_lit = false, found_unassigned_lit = false;
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
                m[unit_lit.i] = (unit_lit.is_neg ? value::f : value::t);
                for (literal const& l : c) {
                    if (l == unit_lit)
                        continue;
                    graph.add_edge(literal(l.i, !l.is_neg), unit_lit);
                }
#ifdef DEBUG
                std::cout << prefix << "UnitProp: " << var_name[unit_lit.i] << " = " << !unit_lit.is_neg << ": ";
                print_clause(c);
                std::cout << '\n';
#endif
                changed = true;
            }
        }
    } while (changed);

    // evaluating
    bool found_unassigned_clause = false;
    for (auto const& c : s) {
        bool found_true_lit = false;
        bool found_unassigned_lit = false;
        for (literal l : c) {
            if (m[l.i] == value::u)
                found_unassigned_lit = true;
            else if ((m[l.i] == value::f) == l.is_neg) {
                found_true_lit = true;
                break;
            }
        }
        if (!found_true_lit) {
            if (found_unassigned_lit)
                found_unassigned_clause = true;
            else {
                erring_clause = c;
#ifdef DEBUG
                std::cout << prefix << "Evaluate: 0: ";
                print_clause(c);
                std::cout << '\n';
#endif
                return value::f;
            }
        }
    }
    if (found_unassigned_clause) {
#ifdef DEBUG
        std::cout << prefix << "Evaluate: unassigned\n";
#endif
        return value::u;
    }
#ifdef DEBUG
    std::cout << prefix << "Evaluate: 1\n";
#endif
    return value::t;
}

bool cdcl::is_sat(std::vector<value>& model) const
{
    std::vector<literal> decisions;
#ifndef DEBUG
    impl graph;
#else
    impl graph(var_name);
#endif
    std::set<literal> erring_clause;
    std::vector<value> M = model;

    std::string last_prefix;

    while (true) {
        erring_clause.clear();
        value V = unit_propagate(M, graph, erring_clause);
        if (V == value::f) {
            if (decisions.empty())
                return false;
            std::set<literal> conf_clause;
            graph.get_roots(erring_clause, conf_clause);

            if (conf_clause.empty())
                return false;

            s.insert(conf_clause);

            literal last_decision;
            bool found_backtracking_decision = false;
            for (auto it = decisions.rbegin(); it != decisions.rend(); ++it) {
                literal l = *it;
                l.is_neg = !l.is_neg;
                if (conf_clause.contains(l)) {
                    decisions.erase((++it).base());
                    l.is_neg = !l.is_neg;
                    last_decision = l;
                    found_backtracking_decision = true;
                    break;
                }
            }
            if (!found_backtracking_decision)
                return false;
#ifdef DEBUG
            prefix = last_prefix;
            std::cout << prefix << "Backtracking on " << var_name[last_decision.i] << ", clause added: ";
            print_clause(conf_clause);
            std::cout << '\n';
#endif
            graph.prune(last_decision, M);
            graph.add_vertex(literal(last_decision.i, !last_decision.is_neg));
        } else if (V == value::u) {
            std::size_t z = choose(M);
            bool new_value = rand_bool();
            M[z] = (new_value ? value::t : value::f);
            decisions.emplace_back(z, !new_value);
            graph.add_vertex(literal(z, !new_value));
#ifdef DEBUG
            std::cout << prefix << "Branch: " << var_name[z] << " = " << new_value << '\n';
            last_prefix = prefix;
            prefix += "    ";
#endif
        } else {
            model = M;
            return true;
        }
    }
}

bool cdcl::is_sat(std::map<std::string, bool>& model) const
{
    std::vector<value> v(var_name.size(), value::u);
    if (!is_sat(v))
        return false;
    model.clear();
    for (std::size_t i = 0; i < v.size(); ++i)
        model[var_name[i]] = (v[i] == value::t);
    return true;
}
