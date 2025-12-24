#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <filesystem>

using Literal = int;
using Clause = std::vector<Literal>;
using CNF = std::vector<Clause>;
enum Val { UNASSIGNED = -1, FALSE_VAL = 0, TRUE_VAL = 1 };
inline int var_idx(int lit) {
    return std::abs(lit) - 1;
}

CNF parse_cnf(const std::string& filename, int& num_vars) {
    std::ifstream file(filename);
    std::string line;
    CNF formula;
    num_vars = 0;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return formula;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue;
        
        if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string tmp;
            int num_clauses;
            iss >> tmp >> tmp >> num_vars >> num_clauses;
        } else {
            std::istringstream iss(line);
            int lit;
            Clause clause;
            while (iss >> lit && lit != 0) {
                clause.push_back(lit);
                if (std::abs(lit) > num_vars) num_vars = std::abs(lit);
            }
            if (!clause.empty()) formula.push_back(clause);
        }
    }
    return formula;
}

Val evaluate_clause(const Clause& clause, const std::vector<int>& assignments) {
    bool is_unresolved = false;
    for (int lit : clause) {
        int idx = var_idx(lit);
        int val = assignments[idx];
        
        if (val != UNASSIGNED) {
            if ((lit > 0 && val == TRUE_VAL) || (lit < 0 && val == FALSE_VAL)) {
                return TRUE_VAL;
            }
        } else {
            is_unresolved = true;
        }
    }

    if (is_unresolved) return UNASSIGNED;

    return FALSE_VAL;
}
bool unit_propagate(const CNF& cnf, std::vector<int>& assignments, std::vector<int>& changes) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& clause : cnf) {
            Val status = evaluate_clause(clause, assignments);
            
            if (status == TRUE_VAL) continue;
            if (status == FALSE_VAL) return false;
            int unassigned_lit = 0;
            int unassigned_count = 0;
            
            for (int lit : clause) {
                if (assignments[var_idx(lit)] == UNASSIGNED) {
                    unassigned_lit = lit;
                    unassigned_count++;
                }
            }

            if (unassigned_count == 0) return false; 

            if (unassigned_count == 1) {
                int idx = var_idx(unassigned_lit);
                int required_val = (unassigned_lit > 0) ? TRUE_VAL : FALSE_VAL;
                
                if (assignments[idx] != UNASSIGNED && assignments[idx] != required_val) {
                    return false;
                }
                
                if (assignments[idx] == UNASSIGNED) {
                    assignments[idx] = required_val;
                    changes.push_back(idx);
                    changed = true;
                }
            }
        }
    }
    return true;
}

bool dpll_solve(const CNF& cnf, std::vector<int>& assignments) {
    std::vector<int> changes;
    if (!unit_propagate(cnf, assignments, changes)) {
        for (int idx : changes) assignments[idx] = UNASSIGNED;
        return false;
    }
    bool all_satisfied = true;
    for (const auto& clause : cnf) {
        Val status = evaluate_clause(clause, assignments);
        if (status == FALSE_VAL) {
            for (int idx : changes) assignments[idx] = UNASSIGNED;
            return false;
        }
        if (status == UNASSIGNED) {
            all_satisfied = false;
        }
    }
    if (all_satisfied) return true;
    int var_to_pick = -1;
    for (size_t i = 0; i < assignments.size(); ++i) {
        if (assignments[i] == UNASSIGNED) {
            var_to_pick = i;
            break;
        }
    }
    assignments[var_to_pick] = TRUE_VAL;
    if (dpll_solve(cnf, assignments)) return true;
    
    assignments[var_to_pick] = FALSE_VAL;
    if (dpll_solve(cnf, assignments)) return true;
    
    assignments[var_to_pick] = UNASSIGNED;

    for (int idx : changes) assignments[idx] = UNASSIGNED;

    return false;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./dpll_solver <input_file.cnf>\n";
        return 1;
    }
    std::string input_path = argv[1];

    int num_vars;
    CNF cnf = parse_cnf(input_path, num_vars);
    
    if (cnf.empty() && num_vars == 0) {
        std::cerr << "Error: Empty or invalid CNF file.\n";
        return 1;
    }

    std::vector<int> assignments(num_vars, UNASSIGNED);

    auto start = std::chrono::high_resolution_clock::now();
    bool sat = dpll_solve(cnf, assignments);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::string outcome = (sat ? "SAT" : "UNSAT");
    std::cout << outcome << " in " << ms << " ms\n";

    // Optional: Write to file if needed (legacy support)
    // std::ofstream out("results_DPLL.txt", std::ios::app);
    // out << input_path << ": " << outcome << " in " << ms << " ms\n";

    return 0;
}