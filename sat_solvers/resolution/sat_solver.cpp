#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
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
        formula.push_back(clause);
    }
    return formula;
}

bool resolve_clauses(const Clause &c1, const Clause &c2, Clause &resolvent) {
    for (int lit : c1) {
        if (c2.count(-lit)) {
            resolvent.clear();
            for (int l : c1) if (l != lit) resolvent.insert(l);
            for (int l : c2) if (l != -lit) resolvent.insert(l);
            return true;
        }
    }
    return false;
}

bool resolution_algorithm(CNF formula) {
    std::set<Clause> clauses(formula.begin(), formula.end());
    bool added = true;
    
    while (added) {
        added = false;
        std::vector<Clause> new_clauses;
        
        for (auto it1 = clauses.begin(); it1 != clauses.end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != clauses.end(); ++it2) {
                Clause resolvent;
                if (resolve_clauses(*it1, *it2, resolvent)) {
                    if (resolvent.empty()) return false;
                    
                    if (clauses.find(resolvent) == clauses.end()) {
                        new_clauses.push_back(resolvent);
                    }
                }
            }
        }
        
        for (const auto &c : new_clauses) {
            if (clauses.insert(c).second) {
                added = true;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./resolution_solver <input_file.cnf>\n";
        return 1;
    }
    std::string filename = argv[1];

    CNF cnf = parse_cnf(filename);

    auto start = std::chrono::high_resolution_clock::now();
    bool sat = resolution_algorithm(cnf);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::string outcome = (sat ? "SAT" : "UNSAT");
    
    std::cout << outcome << " in " << ms << " ms\n";
    return 0;
}