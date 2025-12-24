cmake_minimum_required(VERSION 3.10)
project(SatSolvers VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -O3)
endif()


add_executable(cdcl_solver sat_solvers/cdcl/sat_solver.cpp)

add_executable(dpll_solver sat_solvers/dpll/sat_solver.cpp)

add_executable(dp_solver sat_solvers/dp/sat_solver.cpp)

add_executable(resolution_solver sat_solvers/resolution/sat_solver.cpp)