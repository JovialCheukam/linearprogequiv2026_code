cmake_minimum_required(VERSION 4.0)
project(EquivLinProg)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)

add_compile_options("-O3")
add_compile_options("-march=native")

set(EIGEN_DIR "< Home Directory >/equivalence_linear_program_algo/header_only_lib")

include_directories(${EIGEN_DIR})

#find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
#add_executable(test test.cpp)
#target_link_libraries(test PRIVATE Catch2::Catch2WithMain)



add_executable(EquivLinProg #main.cpp
        matrix_tool.h
        matrix_tool.cpp
        printing_tool.h
        printing_tool.cpp
        permutation.cpp
        permutation.h
        #test.cpp
        solutions.cpp
        solutions.h
        algo_enhanced.cpp
        algo_enhanced.h
        thread_for_equiv.cpp
        matrix_tool.h
        pairhash.h
)
#target_link_libraries(EquivLinProg Catch2::Catch2WithMain)
