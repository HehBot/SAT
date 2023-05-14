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

sat_solver::value sat_solver::evaluate(std::vector<value>& m) const
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
            else
                return value::f;
        }
    }
    if (found_unassigned_clause)
        return value::u;
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
sat_solver::sat_solver(char const* filename)
{
    std::ifstream f(filename);
    std::unordered_map<std::string, std::size_t> names;

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
            if (!names.contains(n)) {
                names[n] = var_name.size();
                var_name.push_back(n);
            }
            l.i = names[n];

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
sat_solver::sat_solver(std::set<std::set<literal>> s, std::size_t no_of_var)
    : s(s)
{
    for (auto const& c : s)
        for (literal const& l : c)
            if (l.i >= no_of_var)
                throw std::invalid_argument("Bad formula");
    var_name.reserve(no_of_var);
    for (std::size_t i = 0; i < no_of_var; ++i)
        var_name.push_back(std::to_string(i));
}

void sat_solver::print() const
{
    std::set<std::set<literal>>::iterator it;
    for (it = s.begin(); std::next(it) != s.end(); ++it) {
        std::cout << '(';
        std::set<literal>::iterator it2;
        for (it2 = it->begin(); std::next(it2) != it->end(); ++it2) {
            if (it2->is_neg)
                std::cout << '~';
            std::cout << var_name[it2->i] << " \u2228 ";
        }
        if (it2->is_neg)
            std::cout << '~';
        std::cout << var_name[it2->i] << ") \u2227 ";
    }
    std::cout << '(';
    std::set<literal>::iterator it2;
    for (it2 = it->begin(); std::next(it2) != it->end(); ++it2) {
        if (it2->is_neg)
            std::cout << '~';
        std::cout << var_name[it2->i] << " \u2228 ";
    }
    if (it2->is_neg)
        std::cout << '~';
    std::cout << var_name[it2->i] << ")\n";
}

void sat_solver::print_clause(std::set<literal> cl) const
{
    auto it = cl.begin();
    std::cout << '(';
    for (; std::next(it) != cl.end(); ++it) {
        if (it->is_neg)
            std::cout << '~';
        std::cout << var_name[it->i] << " \u2228 ";
    }
    if (it->is_neg)
        std::cout << '~';
    std::cout << var_name[it->i] << ')';
}

void sat_solver::write_to_file(char const* filename) const
{
    std::ofstream f(filename);
    for (auto const& c : s) {
        for (auto const& l : c) {
            if (l.is_neg)
                f << '~';
            f << var_name[l.i] << ' ';
        }
        f << '\n';
    }
}
