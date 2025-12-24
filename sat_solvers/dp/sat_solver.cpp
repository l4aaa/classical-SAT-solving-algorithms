#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

using Clause = std::set<int>;
using CNF = std::vector<Clause>;

CNF parse_cnf(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    CNF formula;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return formula;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') continue;

        std::istringstream iss(line);
        int lit;
        Clause clause;
        while (iss >> lit && lit != 0) clause.insert(lit);
        
        if (!clause.empty()) formula.push_back(clause);
    }
    return formula;
}

bool contains_empty_clause(const CNF &cnf) {
    for (const auto &clause : cnf) {
        if (clause.empty()) return true;
    }
    return false;
}

void remove_clauses_with_literal(CNF &cnf, int lit) {
    cnf.erase(std::remove_if(cnf.begin(), cnf.end(),
        [lit](const Clause &c) { return c.count(lit); }), cnf.end());
}

void remove_literal_from_clauses(CNF &cnf, int lit) {
    for (auto &clause : cnf) {
        clause.erase(lit);
    }
}

bool unit_propagate(CNF &cnf, std::set<int> &assignments) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it = cnf.begin(); it != cnf.end(); ) {
            if (it->size() == 1) {
                int unit = *it->begin();
                assignments.insert(unit);
                remove_clauses_with_literal(cnf, unit);
                remove_literal_from_clauses(cnf, -unit);
                changed = true;
                it = cnf.begin();
            } else {
                ++it;
            }
        }
    }
    return !contains_empty_clause(cnf);
}

void eliminate_pure_literals(CNF &cnf, std::set<int> &assignments) {
    std::map<int, int> literal_count;
    for (const auto &clause : cnf) {
        for (int lit : clause) literal_count[lit]++;
    }

    for (const auto &[lit, count] : literal_count) {
        if (literal_count.find(-lit) == literal_count.end()) {
            assignments.insert(lit);
            remove_clauses_with_literal(cnf, lit);
        }
    }
}

CNF resolve_on_variable(const CNF &cnf, int var) {
    CNF pos_clauses, neg_clauses, rest;
    for (const auto &clause : cnf) {
        if (clause.count(var)) pos_clauses.push_back(clause);
        else if (clause.count(-var)) neg_clauses.push_back(clause);
        else rest.push_back(clause);
    }

    CNF resolvents = rest;
    for (const auto &c1 : pos_clauses) {
        for (const auto &c2 : neg_clauses) {
            Clause resolvent;
            for (int l : c1) if (l != var) resolvent.insert(l);
            for (int l : c2) if (l != -var) resolvent.insert(l);
            resolvents.push_back(resolvent);
        }
    }
    return resolvents;
}

bool davis_putnam(CNF cnf, std::set<int> &assignments) {
    if (contains_empty_clause(cnf)) return false;
    if (cnf.empty()) return true;

    if (!unit_propagate(cnf, assignments)) return false;
    eliminate_pure_literals(cnf, assignments);
    if (cnf.empty()) return true;
    if (contains_empty_clause(cnf)) return false;

    std::set<int> vars;
    for (const auto &clause : cnf) {
        for (int lit : clause) vars.insert(std::abs(lit));
    }
    
    if (vars.empty()) return true;

    int var = *vars.begin();
    CNF reduced = resolve_on_variable(cnf, var);
    return davis_putnam(reduced, assignments);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./dp_solver <input_file.cnf>\n";
        return 1;
    }
    std::string filename = argv[1];

    CNF cnf = parse_cnf(filename);
    std::set<int> assignments;

    auto start = std::chrono::high_resolution_clock::now();
    bool sat = davis_putnam(cnf, assignments);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::string outcome = (sat ? "SAT" : "UNSAT");
    
    std::cout << outcome << " in " << ms << " ms\n";
    return 0;
}