# 🧠 Classical SAT Solving Algorithms

![C++](https://img.shields.io/badge/Language-C%2B%2B11-blue.svg)
![Python](https://img.shields.io/badge/Scripting-Python%203-yellow.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A comprehensive benchmarking repository implementing four fundamental algorithms for the Boolean Satisfiability Problem (SAT). Implemented in **C++11** for performance, this project explores the evolution of SAT solvers from the basic Resolution method to the modern Conflict-Driven Clause Learning (CDCL) algorithm.

This project is designed for scientific benchmarking and educational exploration, demonstrating the immense performance gaps between naive and modern approaches.

---

## ✨ Key Features

### 🧩 The Solvers
* **Resolution Method**: A naive approach using pure logical resolution steps. Known for being computationally expensive and slow on non-trivial problems.
* **Davis–Putnam (DP)**: An early algorithm focused on variable elimination. While foundational, it struggles with memory explosion on large instances.
* **DPLL (Davis–Putnam–Logemann–Loveland)**: A memory-efficient depth-first search algorithm utilizing unit propagation, pure literal elimination, and the splitting rule.
* **CDCL (Conflict-Driven Clause Learning)**: The state-of-the-art approach for industrial SAT solving. Features unit propagation, 1-UIP conflict analysis, non-chronological backtracking, and dynamic clause learning.

### ⚙️ Utilities & Architecture
* **Standard Input**: Fully supports the [DIMACS](http://en.wikipedia.org/wiki/Conjunctive_normal_form) CNF file format.
* **Automated Benchmarking**: Each solver automatically processes batches of `.cnf` files and logs execution time in milliseconds.
* **CNF Generator**: A Python script to generate random 2SAT/3SAT instances with customizable variables and clauses.
* **Performance Comparison**: Pre-computed results comparing execution times across all four methods for 2SAT and 3SAT problems.

---

## 🛠️ Tech Stack

* **Core Logic**: C++11 (chosen for speed and strict typing).
* **Scripting**: Python 3 (Random CNF Generation).
* **Data Format**: DIMACS CNF (Conjunctive Normal Form).

---

## 🚀 Getting Started

### 1. Prerequisites
* **C++ Compiler**: A compiler supporting C++11 (e.g., `g++`, `clang`).
* **Python 3**: For generating new test cases (optional).

### 2. Compilation
Each solver is self-contained. You can compile them individually using your terminal:

**Compile CDCL Solver:**
```bash
g++ -std=c++11 sat_solvers/cdcl/sat_solver.cpp -o cdcl_solver
```

**Compile DPLL Solver:**
```bash
g++ -std=c++11 sat_solvers/dpll/sat_solver.cpp -o dpll_solver
```

*(Repeat for `dp` and `resolution` directories as needed)*

### 3. Usage

#### 🔬 Running a Solver
The solvers are configured to read `.cnf` files from the `cnf_files/samples/` directory by default.

1.  Place your DIMACS `.cnf` files into `cnf_files/samples/`.
2.  Run the executable:
    ```bash
    ./cdcl_solver
    ```
3.  Results are printed to the console and saved to `results_<method>.txt`.

#### 🎲 Generating Random CNF
Use the included Python script to create custom benchmarks.

```bash
# Generate a random CNF with 30 variables and 30 clauses
python3 cnf_generator/generate_random_cnf.py --vars 30 --clauses 30 --lits 3 --out random.cnf
```

---

## 📂 Project Structure

```bash
l4aaa-classical-sat-solving-algorithms/
├── cnf_files/                 # Directory for input files
│   └── samples/               # Drop your .cnf test files here
├── cnf_generator/
│   ├── generate_random_cnf.py # Python script for creating random benchmarks
│   └── README.md              # Documentation for the generator
├── results/                   # Benchmark outcomes
│   ├── 2sat/                  # Results for 2-Satisfiability problems
│   └── 3sat/                  # Results for 3-Satisfiability problems
├── sat_solvers/               # Source code for algorithms
│   ├── cdcl/                  # Conflict-Driven Clause Learning implementation
│   ├── dp/                    # Davis-Putnam implementation
│   ├── dpll/                  # DPLL implementation
│   └── resolution/            # Resolution Method implementation
├── LICENSE                    # MIT License
└── README.md                  # Project Documentation
```

---

## 📊 Benchmark Summary

Results found in the `results/` directory highlight the drastic performance differences:

| Algorithm | 2SAT Performance | 3SAT Performance | Note |
| :--- | :--- | :--- | :--- |
| **CDCL** | ⚡ Instant (< 1ms) | ⚡ Fast | Scales well with complexity. |
| **DPLL** | 🚀 Fast | 🐢 Moderate | Good for smaller instances. |
| **DP** | 🐢 Slow | 🛑 Timeout | Memory intensive. |
| **Resolution** | 🛑 Very Slow | 🛑 Timeout | Exponential complexity. |

---

## 📝 License

Distributed under the MIT License. See `LICENSE` for more information.

> **Disclaimer**: The simpler methods (Resolution, DP) are implemented for scientific comparison and may not solve large instances efficiently.
