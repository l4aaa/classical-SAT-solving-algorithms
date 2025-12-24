# 🧠 Classical SAT Solving Algorithms

![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![Python](https://img.shields.io/badge/Scripting-Python%203-yellow.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-CMake-orange.svg)

A comprehensive benchmarking repository implementing four fundamental algorithms for the Boolean Satisfiability Problem (SAT).

Implemented in **modern C++17** for performance and memory safety, this project explores the evolution of SAT solvers from the basic Resolution method to the modern Conflict-Driven Clause Learning (CDCL) algorithm. 

This project is designed for scientific benchmarking and systems engineering exploration, demonstrating the immense performance gaps between naive and modern approaches.

---

## ✨ Key Features

### 🧩 The Solvers
1. **Resolution Method**: A naive approach using pure logical resolution steps.
   * *Characteristics*: Computationally expensive ($O(2^n)$ memory/time). Best for demonstrating theoretical proofs rather than solving.
2. **Davis–Putnam (DP)**: An early algorithm focused on variable elimination.
   * *Characteristics*: Foundational but suffers from memory explosion on non-trivial instances.
3. **DPLL (Davis–Putnam–Logemann–Loveland)**: A memory-efficient Depth-First Search (DFS) algorithm.
   * *Characteristics*: Uses **Unit Propagation**, **Pure Literal Elimination**, and **Backtracking**.
   * *Implementation*: Optimized with pass-by-reference and state restoration to minimize memory allocations.
4. **CDCL (Conflict-Driven Clause Learning)**: The state-of-the-art approach for industrial SAT solving.
   * *Characteristics*: Features **Non-Chronological Backtracking**, **1-UIP Conflict Analysis**, and **VSIDS-like Heuristics**.

### ⚙️ Utilities & Architecture
* **Standard Input**: Fully supports the [DIMACS](http://en.wikipedia.org/wiki/Conjunctive_normal_form) CNF file format.
* **CLI Interface**: Each solver is a standalone command-line tool usable in scripts or pipelines.
* **CNF Generator**: A Python script to generate random 2SAT/3SAT instances with customizable complexity.

---

## 🛠️ Tech Stack

* **Core Logic**: C++17 (optimized for speed, strict typing, and memory safety).
* **Build System**: CMake (Industry standard for cross-platform builds).
* **Scripting**: Python 3 (Random CNF Generation).
* **Data Format**: DIMACS CNF (Conjunctive Normal Form).

---

## 🚀 Getting Started

### 1. Prerequisites
* **C++ Compiler**: GCC, Clang, or MSVC supporting C++17.
* **CMake**: Version 3.10 or higher.
* **Python 3**: Optional, for generating new test cases.

### 2. Compilation
This project uses CMake for building all solvers simultaneously.

```bash
# 1. Create a build directory
mkdir build && cd build

# 2. Configure the project
cmake ..

# 3. Compile all solvers
make
```

### 3. Usage
Once compiled, you can run any solver by passing the path to a `.cnf` file as an argument.

**Running the CDCL Solver:**
```bash
./cdcl_solver ../cnf_files/samples/simple_v3_c2.cnf
```

**Running the DPLL Solver:**
```bash
./dpll_solver ../cnf_files/samples/quinn.cnf
```

**Output:**
The solver prints the result (`SAT` or `UNSAT`) and the execution time in milliseconds to standard output.
```text
SAT in 0.045 ms
```

#### 🎲 Generating Random Benchmarks
Use the included Python script to create custom benchmarks.
```bash
# Generate a random CNF with 50 variables and 100 clauses
python3 cnf_generator/generate_random_cnf.py --vars 50 --clauses 100 --lits 3 --out benchmark.cnf
```

---

## 📂 Project Structure

```text
l4aaa-classical-sat-solving-algorithms/
├── CMakeLists.txt             # Build configuration
├── cnf_files/                 # Directory for input files
│   └── samples/               # Standard .cnf test files
├── cnf_generator/
│   └── generate_random_cnf.py # Python script for creating benchmarks
├── results/                   # Pre-computed benchmark outcomes
├── sat_solvers/               # Source code
│   ├── cdcl/                  # Conflict-Driven Clause Learning
│   ├── dp/                    # Davis-Putnam
│   ├── dpll/                  # DPLL (Optimized Backtracking)
│   └── resolution/            # Resolution Method
├── LICENSE                    # MIT License
└── README.md                  # Documentation
```

---

## 📊 Benchmark Summary

Results found in the `results/` directory highlight the drastic performance differences:

| Algorithm | 2SAT Performance | 3SAT Performance | Note |
| :--- | :--- | :--- | :--- |
| **CDCL** | ⚡ Instant (< 1ms) | ⚡ Fast | Scales well with complexity; industry standard. |
| **DPLL** | 🚀 Fast | 🐢 Moderate | Good for smaller instances; memory efficient. |
| **DP** | 🐢 Slow | 🛑 Timeout | Memory intensive due to variable elimination. |
| **Resolution** | 🛑 Very Slow | 🛑 Timeout | Exponential complexity; theoretical use only. |

---

## 📝 License

Distributed under the MIT License. See `LICENSE` for more information.

> **Disclaimer**: The simpler methods (Resolution, DP) are implemented for scientific comparison and educational purposes; they are not intended for solving large industrial instances.