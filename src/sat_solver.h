#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <map>
#include <set>
#include <string>
#include <vector>

class sat_solver {
public:
    struct literal {
        std::size_t i;
        bool is_neg;
        literal() = default;
        literal(std::size_t i)
            : i(i), is_neg(false)
        {
        }
        literal(std::size_t i, bool is_neg)
            : i(i), is_neg(is_neg)
        {
        }
        literal neg() const
        {
            return literal(i, !is_neg);
        }
        bool operator==(literal const& l) const
        {
            return (i == l.i) && (is_neg == l.is_neg);
        }
        bool operator<(literal const& l) const
        {
            if (i != l.i)
                return i < l.i;
            return !is_neg && l.is_neg;
        }
    };
    enum class value {
        t,
        f,
        u
    };

protected:
    static std::size_t choose(std::vector<value>& m);
    static bool rand_bool();
    value evaluate(std::vector<value>& m) const;

    std::vector<std::string> var_name;
    mutable std::set<std::set<literal>> s;

public:
    sat_solver(char const* filename);
    sat_solver(std::set<std::set<literal>> s, std::size_t no_of_var);
    virtual bool is_sat(std::map<std::string, bool>& model) const = 0;
    void print() const;
    void print_clause(std::set<literal> clause) const;
    void write_to_file(char const* filename) const;
};

#endif // SAT_SOLVER_H
