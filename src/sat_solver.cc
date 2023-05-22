#include "sat_solver.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

static std::random_device rd;
static std::mt19937 eng(rd());

bool sat_solver::rand_bool()
{
    static std::uniform_int_distribution<> distr(0, 1);
    return distr(eng);
}

std::size_t sat_solver::choose(std::vector<value>& m)
{
    std::size_t i;
    std::vector<std::size_t> candidates;
    for (i = 0; i < m.size(); ++i) {
        if (m[i] == value::u)
            candidates.push_back(i);
    }
    if (candidates.size() == 0)
        return m.size();
    std::uniform_int_distribution<> distr(0, candidates.size() - 1);
    return candidates[distr(eng)];
}

sat_solver::value sat_solver::evaluate(std::vector<value> const& m, std::set<literal>& erring_clause) const
{
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

static bool isalnum(char t)
{
    return ((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || (t >= '0' && t <= '9'));
}
static std::string get_alphanum(std::ifstream& f)
{
    std::string s;
    char z = f.get();
    while (isalnum(z)) {
        s += z;
        z = f.get();
    }
    f.unget();
    return s;
}

sat_solver::sat_solver(std::set<std::set<literal>> const& clauses)
    : s(clauses)
{
    for (auto const& c : clauses)
        for (literal const& l : c) {
            if (l.i >= var_name.size())
                var_name.resize(l.i + 1, "");
            if (var_name[l.i].length() == 0) {
                var_name[l.i] = std::to_string(l.i);
                var_name_indices[std::to_string(l.i)] = l.i;
            }
        }
}

void sat_solver::add_clauses(std::set<std::set<literal>> const& clauses)
{
    for (auto const& c : clauses)
        for (literal const& l : c) {
            if (l.i >= var_name.size())
                var_name.resize(l.i + 1, "");
            if (var_name[l.i].length() == 0) {
                var_name[l.i] = std::to_string(l.i);
                var_name_indices[std::to_string(l.i)] = l.i;
            }
        }
    s.insert(clauses.begin(), clauses.end());
}
void sat_solver::add_clauses_from_file(char const* filename)
{
    std::ifstream f(filename);

    if (f.fail()) {
        std::cerr << "Failed to add clauses from file " << filename << '\n';
        return;
    }

    char z = f.get();
    while (!f.eof()) {
        std::set<literal> clause;
        bool useful = true;
        while (z != '\n') {
            literal l;
            if (z == '~')
                l.is_neg = true;
            else if (isalnum(z)) {
                l.is_neg = false;
                f.unget();
            } else {
                z = f.get();
                continue;
            }

            std::string n = get_alphanum(f);
            if (!var_name_indices.contains(n)) {
                var_name_indices[n] = var_name.size();
                var_name.push_back(n);
            }
            l.i = var_name_indices[n];

            if (!clause.contains(literal(l.i, !l.is_neg)))
                clause.insert(l);
            else {
                useful = false;
                break;
            }

            z = f.get();
        }
        if (useful)
            s.insert(clause);
        else {
            constexpr auto m = std::numeric_limits<std::streamsize>::max();
            f.ignore(m, '\n');
        }
        z = f.get();
    }
}

#ifdef DEBUG
void sat_solver::print() const
{
    std::cout << "Formula(";
    if (s.empty()) {
        std::cout << ')';
        return;
    }
    auto it = s.begin();
    for (it = s.begin(); std::next(it) != s.end(); ++it) {
        print_clause(*it);
        std::cout << " \u2227 ";
    }
    print_clause(*it);
    std::cout << ')';
}

void sat_solver::print_clause(std::set<literal> const& cl) const
{
    std::cout << '(';
    if (cl.empty()) {
        std::cout << ')';
        return;
    }
    auto it = cl.begin();
    for (; std::next(it) != cl.end(); ++it) {
        if (it->is_neg)
            std::cout << '~';
        std::cout << var_name[it->i] << " \u2228 ";
    }
    if (it->is_neg)
        std::cout << '~';
    std::cout << var_name[it->i] << ')';
}
#endif

void sat_solver::write_to_file(char const* filename) const
{
    std::ofstream f(filename);
    for (auto const& c : s) {
        auto it = c.begin();
        for (; std::next(it) != c.end(); ++it) {
            if (it->is_neg)
                f << '~';
            f << var_name[it->i] << ' ';
        }
        if (it->is_neg)
            f << '~';
        f << var_name[it->i] << '\n';
    }
}
