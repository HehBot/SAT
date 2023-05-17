#ifndef IMPL_H
#define IMPL_H

#include "sat_solver.h"

#include <map>
#include <set>
#include <string>
#include <vector>

class impl {
    using literal = sat_solver::literal;
    using value = sat_solver::value;

    std::vector<bool> is_valid;
    std::vector<literal> literals;
    std::map<literal, std::size_t> indices;
    std::vector<std::set<std::size_t>> adj, back_adj;

    std::size_t get_free_slot() const;
    void get_roots_helper(std::size_t n, std::vector<bool>& visited, std::set<size_t>& ans) const;
    void prune_helper(std::size_t n, std::vector<bool>& stays);

#ifdef DEBUG
    std::vector<std::string> const var_name;
    std::string& prefix;
#endif

public:
#ifndef DEBUG
    impl() = default;
#else
    impl(std::vector<std::string> const& var_name, std::string& prefix);
    void write_to_file(char const* filename) const;
#endif
    void add_vertex(literal const& n);
    void add_edge(literal const& n1, literal const& n2);

    void get_roots(std::set<literal> const& erring_clause, std::set<literal>& conf_clause) const;
    void prune(literal const& l, std::vector<value>& M);
};

#endif // IMPL_H
