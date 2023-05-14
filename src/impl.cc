#include "impl.h"

#ifdef DEBUG
    #include <fstream>
    #include <iostream>
#endif
#include <set>

#ifdef DEBUG
extern std::string prefix;

impl::impl(std::vector<std::string> const& var_name)
    : var_name(var_name)
{
}

void impl::write_to_file(char const* filename) const
{
    std::ofstream f(filename);
    f << "digraph {\n";
    for (std::size_t i = 0; i < adj.size(); ++i) {
        literal l = literals[i];
        f << "    \"" << (l.is_neg ? "~" : "") << var_name[l.i] << "\" -> { ";
        for (auto j : adj[i]) {
            literal lj = literals[j];
            f << "\"" << (lj.is_neg ? "~" : "") << var_name[lj.i] << "\" ";
        }
        f << "}\n";
    }
    f << "}";
}
#endif

void impl::add_vertex(literal const& n)
{
    if (!indices.contains(n)) {
        indices[n] = literals.size();
        literals.push_back(n);
        adj.push_back(std::set<std::size_t>());
        back_adj.push_back(std::set<std::size_t>());
    }
}

void impl::add_edge(literal const& n1, literal const& n2)
{
    std::size_t k1, k2;
    if (indices.contains(n1))
        k1 = indices[n1];
    else
        return;
    if (!indices.contains(n2)) {
        k2 = indices[n2] = literals.size();
        literals.push_back(n2);
        adj.push_back(std::set<std::size_t>());
        back_adj.push_back(std::set<std::size_t>());
    } else
        k2 = indices[n2];
    adj[k1].insert(k2);
    back_adj[k2].insert(k1);
}

void impl::get_roots_helper(std::size_t n, std::vector<bool>& visited, std::set<size_t>& ans) const
{
    if (visited[n])
        return;
    visited[n] = true;
    if (back_adj[n].empty())
        ans.insert(n);
    else {
        for (std::size_t n_back : back_adj[n])
            get_roots_helper(n_back, visited, ans);
    }
}

void impl::get_roots(std::set<literal> const& erring_clause, std::set<literal>& conf_clause) const
{
    std::vector<bool> visited(literals.size(), false);

    std::set<size_t> ans;

    for (auto n : erring_clause) {
        n.is_neg = !n.is_neg;
        if (indices.contains(n))
            get_roots_helper(indices.at(n), visited, ans);
    }

    conf_clause.clear();
    for (auto z : ans) {
        literal l = literals[z];
        l.is_neg = !l.is_neg;
        conf_clause.insert(l);
    }
}

void impl::prune_helper(std::size_t n, std::vector<bool>& stays)
{
    stays[n] = false;
    for (std::size_t parent : back_adj[n])
        adj[parent].erase(n);
    for (std::size_t child : adj[n]) {
        back_adj[child].erase(n);
        prune_helper(child, stays);
    }
}

void impl::prune(literal const& l, std::vector<value>& M)
{
    std::vector<bool> stays(literals.size(), true);
    prune_helper(indices[l], stays);
    std::size_t i = 0;
    indices.clear();
    while (i != stays.size()) {
        if (!stays[i]) {
#ifdef DEBUG
            std::cout << prefix << "    " << var_name[literals[i].i] << " = unassigned\n";
#endif
            M[literals[i].i] = value::u;
            adj.erase(adj.begin() + i);
            back_adj.erase(back_adj.begin() + i);
            literals.erase(literals.begin() + i);
            stays.erase(stays.begin() + i);
        } else {
            indices[literals[i]] = i;
            ++i;
        }
    }
}
