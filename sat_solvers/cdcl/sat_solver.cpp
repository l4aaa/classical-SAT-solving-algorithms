#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>

enum RetVal {
    r_satisfied,
    r_unsatisfied,
    r_normal
};

class SATSolverCDCL {
private:
    std::vector<int> literals;
    std::vector<std::vector<int>> literal_list_per_clause;
    
    std::vector<int> literal_frequency;
    std::vector<int> literal_polarity;
    std::vector<int> original_literal_frequency;
    
    std::vector<int> literal_decision_level;
    std::vector<int> literal_antecedent;
    
    int literal_count = 0;
    int clause_count = 0;
    int assigned_literal_count = 0;
    int kappa_antecedent = -1;
    int pick_counter = 0;
    bool already_unsatisfied = false;

    std::random_device random_generator;
    std::mt19937 generator;

    int unit_propagate(int decision_level);
    void assign_literal(int variable, int decision_level, int antecedent);
    void unassign_literal(int literal_index);
    int literal_to_variable_index(int variable);
    int conflict_analysis_and_backtrack(int decision_level);
    std::vector<int>& resolve(std::vector<int>& input_clause, int literal);
    int pick_branching_variable();
    bool all_variables_assigned();

public:
    SATSolverCDCL() : generator(random_generator()) {}
    
    void initialize(const std::string& filename);
    
    int CDCL();
    int solve();
};

void SATSolverCDCL::initialize(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        exit(1);
    }

    std::string line;
    already_unsatisfied = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp >> temp >> literal_count >> clause_count;
            break;
        }
    }

    assigned_literal_count = 0;
    kappa_antecedent = -1;
    pick_counter = 0;

    literals.assign(literal_count, -1);
    literal_frequency.assign(literal_count, 0);
    literal_polarity.assign(literal_count, 0);
    literal_list_per_clause.resize(clause_count);
    literal_antecedent.assign(literal_count, -1);
    literal_decision_level.assign(literal_count, -1);

    int literal;
    for (int i = 0; i < clause_count; ++i) {
        int literal_count_in_clause = 0;
        while (file >> literal && literal != 0) {
            literal_list_per_clause[i].push_back(literal);
            
            int lit_idx = (literal > 0) ? (literal - 1) : (-literal - 1);
            if (lit_idx < literal_count) {
                literal_frequency[lit_idx]++;
                if (literal > 0) literal_polarity[lit_idx]++;
                else literal_polarity[lit_idx]--;
            }
            literal_count_in_clause++;
        }
        
        if (literal_count_in_clause == 0 && !file.eof()) {
            already_unsatisfied = true;
        }
    }
    
    original_literal_frequency = literal_frequency;
    file.close();
}

int SATSolverCDCL::unit_propagate(int decision_level) {
    bool unit_clause_found = false;
    int false_count = 0, unset_count = 0;
    int last_unset_literal_idx = -1;
    bool satisfied_flag = false;

    do {
        unit_clause_found = false;
        for (int i = 0; i < literal_list_per_clause.size() && !unit_clause_found; i++) {
            false_count = 0;
            unset_count = 0;
            satisfied_flag = false;

            for (size_t j = 0; j < literal_list_per_clause[i].size(); j++) {
                int variable = literal_list_per_clause[i][j];
                int literal_index = literal_to_variable_index(variable);

                if (literals[literal_index] == -1) {
                    unset_count++;
                    last_unset_literal_idx = j;
                } else if ((literals[literal_index] == 0 && variable > 0) ||
                           (literals[literal_index] == 1 && variable < 0)) {
                    false_count++;
                } else {
                    satisfied_flag = true;
                    break;
                }
            }

            if (satisfied_flag) continue;

            if (unset_count == 1) {
                assign_literal(literal_list_per_clause[i][last_unset_literal_idx], decision_level, i);
                unit_clause_found = true;
                break;
            } else if (false_count == literal_list_per_clause[i].size()) {
                kappa_antecedent = i;
                return RetVal::r_unsatisfied;
            }
        }
    } while (unit_clause_found);

    kappa_antecedent = -1;
    return RetVal::r_normal;
}

void SATSolverCDCL::assign_literal(int variable, int decision_level, int antecedent) {
    int literal_index = literal_to_variable_index(variable);
    int value = (variable > 0) ? 1 : 0;
    
    literals[literal_index] = value;
    literal_decision_level[literal_index] = decision_level;
    literal_antecedent[literal_index] = antecedent;
    
    literal_frequency[literal_index] = -1; 
    assigned_literal_count++;
}

void SATSolverCDCL::unassign_literal(int literal_index) {
    literals[literal_index] = -1;
    literal_decision_level[literal_index] = -1;
    literal_antecedent[literal_index] = -1;
    literal_frequency[literal_index] = original_literal_frequency[literal_index];
    assigned_literal_count--;
}

int SATSolverCDCL::literal_to_variable_index(int variable) {
    return (variable > 0) ? variable - 1 : -variable - 1;
}

int SATSolverCDCL::conflict_analysis_and_backtrack(int decision_level) {
    std::vector<int> learnt_clause = literal_list_per_clause[kappa_antecedent];
    int conflict_decision_level = decision_level;
    int this_level_count = 0;
    int resolver_literal_idx = 0;

    do {
        this_level_count = 0;
        for (int variable : learnt_clause) {
            int lit_idx = literal_to_variable_index(variable);
            if (literal_decision_level[lit_idx] == conflict_decision_level) {
                this_level_count++;
            }
            if (literal_decision_level[lit_idx] == conflict_decision_level &&
                literal_antecedent[lit_idx] != -1) {
                resolver_literal_idx = lit_idx;
            }
        }
        
        if (this_level_count == 1) break;
        
        learnt_clause = resolve(learnt_clause, resolver_literal_idx);
    } while (true);

    literal_list_per_clause.push_back(learnt_clause);
    
    for (int variable : learnt_clause) {
        int lit_idx = literal_to_variable_index(variable);
        int update = (variable > 0) ? 1 : -1;
        literal_polarity[lit_idx] += update;
        
        if (literal_frequency[lit_idx] != -1) {
            literal_frequency[lit_idx]++;
        }
        original_literal_frequency[lit_idx]++;
    }
    
    clause_count++;

    int backtracked_decision_level = 0;
    for (int variable : learnt_clause) {
        int lit_idx = literal_to_variable_index(variable);
        int level_here = literal_decision_level[lit_idx];
        
        if (level_here != conflict_decision_level && level_here > backtracked_decision_level) {
            backtracked_decision_level = level_here;
        }
    }

    for (int i = 0; i < literals.size(); i++) {
        if (literal_decision_level[i] > backtracked_decision_level) {
            unassign_literal(i);
        }
    }

    return backtracked_decision_level;
}

std::vector<int>& SATSolverCDCL::resolve(std::vector<int>& input_clause, int literal_idx) {
    std::vector<int> second_input = literal_list_per_clause[literal_antecedent[literal_idx]];
    
    input_clause.insert(input_clause.end(), second_input.begin(), second_input.end());
    
    int var_num = literal_idx + 1;
    
    for (int i = 0; i < input_clause.size(); i++) {
        if (input_clause[i] == var_num || input_clause[i] == -var_num) {
            input_clause.erase(input_clause.begin() + i);
            i--;
        }
    }
    
    std::sort(input_clause.begin(), input_clause.end());
    input_clause.erase(std::unique(input_clause.begin(), input_clause.end()), input_clause.end());
    
    return input_clause;
}

int SATSolverCDCL::pick_branching_variable() {
    std::uniform_int_distribution<int> choose_branch(1, 10);
    std::uniform_int_distribution<int> choose_literal(0, literal_count - 1);
    
    if (pick_counter > 20 * literal_count) {
        for (int i = 0; i < literals.size(); i++) {
            original_literal_frequency[i] /= 2;
            if (literal_frequency[i] != -1) literal_frequency[i] /= 2;
        }
        pick_counter = 0;
    }
    
    int variable_idx = -1;
    int max_freq = -1;
    
    for (int i = 0; i < literal_count; i++) {
        if (literal_frequency[i] > max_freq) {
            max_freq = literal_frequency[i];
            variable_idx = i;
        }
    }
    
    if (variable_idx != -1) {
        pick_counter++;
        return (literal_polarity[variable_idx] >= 0) ? variable_idx + 1 : -variable_idx - 1;
    }

    bool too_many_attempts = false;
    int attempt_counter = 0;
    do {
        int idx = choose_literal(generator);
        if (literal_frequency[idx] != -1) {
             return (literal_polarity[idx] >= 0) ? idx + 1 : -idx - 1;
        }
        attempt_counter++;
        if (attempt_counter > 10 * literal_count) too_many_attempts = true;
    } while (!too_many_attempts);

    return 1;
}

bool SATSolverCDCL::all_variables_assigned() {
    return literal_count == assigned_literal_count;
}

int SATSolverCDCL::CDCL() {
    int decision_level = 0;
    if (already_unsatisfied) return RetVal::r_unsatisfied;
    
    int unit_propagate_result = unit_propagate(decision_level);
    if (unit_propagate_result == RetVal::r_unsatisfied) return unit_propagate_result;

    while (!all_variables_assigned()) {
        int picked_variable = pick_branching_variable();
        decision_level++;
        assign_literal(picked_variable, decision_level, -1);
        
        while (true) {
            unit_propagate_result = unit_propagate(decision_level);
            if (unit_propagate_result == RetVal::r_unsatisfied) {
                if (decision_level == 0) return RetVal::r_unsatisfied;
                decision_level = conflict_analysis_and_backtrack(decision_level);
            } else {
                break;
            }
        }
    }
    return RetVal::r_satisfied;
}

int SATSolverCDCL::solve() {
    return CDCL();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./cdcl_solver <input_file.cnf>\n";
        return 1;
    }
    std::string filename = argv[1];

    SATSolverCDCL solver;
    solver.initialize(filename);

    auto start = std::chrono::high_resolution_clock::now();
    int result = solver.solve();
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::string outcome = (result == RetVal::r_satisfied ? "SAT" : "UNSAT");
    
    std::cout << outcome << " in " << ms << " ms\n";
    
    return 0;
}