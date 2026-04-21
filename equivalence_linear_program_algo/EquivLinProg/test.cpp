//
// Created by Admin on 2025-10-10.
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "algo.h"
#include "algo_scale.h"
#include "solutions.h"
#include "matrix_tool.h"
#include <vector>
#include "algo_enhanced.h"
#include "algo_scale_enhanced.h"
#include "algo_scale_enhanced2.h"
#include <chrono>
#include <format> // Required for std::format
#include "permutation.h"
#include "printing_tool.h"


std::tuple<bool,
std::string, std::vector<double>, std::vector<double>,
std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> test_equiv_lp(const std::string& algoType) {

    std::string set_web, type_inst,  set_inst, set_inst_test, inst;

    if (std::string data("netlib"); data == "synthetic") {
        set_web = "synthetic/"; type_inst = "eq";  set_inst = "synth"; set_inst_test ="synth_npz/"; inst = "synth_10";
    }
    else {
        set_web = "netlib/"; type_inst = "eq";  set_inst = "lp"; set_inst_test = "lp_npz2/"; inst = "afiro";
    }

    auto [A, B,
          lowerBoundsA, lowerBoundsB,
          upperBoundsA, upperBoundsB ]  = get_sparse_matrice(set_web, type_inst,set_inst, set_inst_test, inst);

    std::cout << "A = " << std::endl <<  A << std::endl;
    std::cout << "B = " << std::endl <<  B << std::endl;

    std::tuple<bool,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
    std::string> result;

    // Initialize  timers
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Test of " << algoType << std::endl;

    // Initialize solution
    Solutions solution;

    if (algoType == "algo_after_all_permutations") {

        Algo_equal testor;
        // Record stating time
        start = std::chrono::high_resolution_clock::now();
        result = testor.check_equiv_between_asso_lp_matrices(A,B,
                                        lowerBoundsA, lowerBoundsB, std::chrono::microseconds(900000000));
        // Record ending time
        end = std::chrono::high_resolution_clock::now();
        solution = Solutions(A,B,result,"algo");

    }
    if (algoType == "algo_after_each_permutation") {

        Algo_equal testor;
        // Record stating time
        start = std::chrono::high_resolution_clock::now();
        result = testor.smart_permutation_check_equiv_between_asso_lp_matrices(A,B,
                                                                        lowerBoundsA, lowerBoundsB,
                                                                        upperBoundsA, upperBoundsB,std::chrono::microseconds(900000000));
        // Record ending time
        end = std::chrono::high_resolution_clock::now();
        solution = Solutions(A,B,result,"algo");
    }
    if (algoType == "algo_scale") {

        Algo_scale testor;
        // Record stating time
        start = std::chrono::high_resolution_clock::now();
        result = testor.johnsonTrotter_check_equiv_between_asso_lp_scaled_matrices(A,B,
                                                                        lowerBoundsA, lowerBoundsB, std::chrono::microseconds(900000000));
        // Record ending time
        end = std::chrono::high_resolution_clock::now();
        solution = Solutions(A,B,result,"algo_scale");
    }
    if (algoType == "algo_scale_enhanced") {

        Algo_scale_enhanced testor;
        // Record stating time
        start = std::chrono::high_resolution_clock::now();
        result = testor.enhanced_check_equiv_between_asso_lp_scaled_matrices(A,B,
                                                                        lowerBoundsA, lowerBoundsB,
                                                                        upperBoundsA, upperBoundsB, std::chrono::microseconds(900000000));
        // Record ending time
        end = std::chrono::high_resolution_clock::now();
        solution = Solutions(A,B,result,"algo_scale_enhanced");
    }
    if (algoType == "algo_scale_enhanced2") {

        Algo_scale_enhanced2 testor;
        // Record stating time
        start = std::chrono::high_resolution_clock::now();
        result = testor.enhanced2_check_equiv_between_asso_lp_scaled_matrices(A,B,
                                                                        lowerBoundsA, lowerBoundsB,
                                                                        upperBoundsA, upperBoundsB, std::chrono::microseconds(900000000));
        // Record ending time
        end = std::chrono::high_resolution_clock::now();
        solution = Solutions(A,B,result,"algo_scale_enhanced2");
    }
    if (algoType == "algo_enhanced") {

        Algo_enhanced testor;
        // Record stating time
        start = std::chrono::high_resolution_clock::now();
        result = testor.enhanced_check_equiv_between_asso_lp_matrices(algoType, A,B,
                                                                        lowerBoundsA, lowerBoundsB,
                                                                        upperBoundsA, upperBoundsB, std::chrono::microseconds(900000000));
        // Record ending time
        end = std::chrono::high_resolution_clock::now();
        solution = Solutions(A,B,result,"algo_enhanced");
    }
    // Compute the duration in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Time execution of " <<  algoType << " : "  << duration.count() << " microseconds" << std::endl;

    std::string model = algoType;
    std::string solver = "no_solver";
    std::string max_scale = "10";
    std::string timeout_or_not = std::get<5>(result);
    std::string final_result;
    std::string valid_or_not = "no-solution-to-validate";
    if (type_inst == "eq" || type_inst == "eq_pro") {
        if (std::get<0>(result) == true && std::get<5>(result) == "solved-without-timeout") {
            final_result = "equivalent";
            valid_or_not = "approximate-solution";
        }
        if (std::get<0>(result) == false  && std::get<5>(result) == "unknown-due-to-timeout") final_result = "unknown";
        if (std::get<0>(result) == false && std::get<5>(result) == "solved-without-timeout") final_result = "error:-should-be-equivalent";
    }
    if (type_inst == "not_eq" || type_inst == "not_eq_pro") {
        if (std::get<0>(result) == false && std::get<5>(result) == "solved-without-timeout") final_result = "not-equivalent";
        if (std::get<0>(result) == false  && std::get<5>(result) == "unknown-due-to-timeout") final_result = "unknown";
        if (std::get<0>(result) == true && std::get<5>(result) == "solved-without-timeout") final_result = "error:-should-be-not-equivalent";
    }

    std::chrono::seconds time_seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    int hours = static_cast<int>(time_seconds.count() / 3600);
    int minutes = static_cast<int>((time_seconds.count() % 3600)/60);
    int seconds = static_cast<int>((time_seconds.count() % 3600) % 60);
    std::string time_ex = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);

    std::string num_lines = std::to_string(A.rows());
    std::string num_columns = std::to_string(A.cols());


    solution.get_elts_to_validate_solution();
    if (solution.validate_a_solution()) valid_or_not = "valid-solution";


    record_solution_with_python(model, solver, max_scale, timeout_or_not,
                               type_inst, set_inst,set_inst_test,inst,set_web,
                               final_result, time_ex ,  valid_or_not , num_columns ,num_lines);

    return solution.get_elts_to_validate_solution(true);
}




/*
TEST_CASE( "Equivalence of linear programs: algo_after_all_permutations", "[Algo_after_all_permutation]" ) {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> testEquiv = test_equiv_lp("algo_after_all_permutations");

    SECTION( "Test of effective equivalence" ) {

        if (std::get<0>(testEquiv)) {

            REQUIRE(std::get<2>(testEquiv).size() == std::get<3>(testEquiv).size());

            SECTION( "Test if the associated matrices A and B have the same set of coefficients" ) {

                for (int i = 0; i < std::get<2>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<2>(testEquiv)[i] == std::get<3>(testEquiv)[i]);
                }
            }

            SECTION( "Test if permutMatrixRows is a permutation matrix") {

                for (int i = 0; i < std::get<4>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<4>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<5>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<5>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixCols is a permutation matrix") {

                for (int i = 0; i < std::get<6>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<6>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<7>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<7>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixRows * A * permutMatrixCols = B ") {

                for (int i = 0; i < std::get<8>(testEquiv).rows(); ++i) {

                    for (int j = 0; j < std::get<8>(testEquiv).cols(); ++j) {

                        REQUIRE(std::get<8>(testEquiv).coeffRef(i,j) == std::get<9>(testEquiv).coeffRef(i,j));
                    }
                }
            }
        }
    }
}


TEST_CASE( "Equivalence of linear programs : algo_after_each_permutation", "[Algo_after_each_permutation]" ) {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> testEquiv = test_equiv_lp("algo_after_each_permutation");

    SECTION( "Test of effective equivalence" ) {

        if (std::get<0>(testEquiv)) {

            REQUIRE(std::get<2>(testEquiv).size() == std::get<3>(testEquiv).size());

            SECTION( "Test if the associated matrices A and B have the same set of coefficients" ) {

                for (int i = 0; i < std::get<2>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<2>(testEquiv)[i] == std::get<3>(testEquiv)[i]);
                }
            }

            SECTION( "Test if permutMatrixRows is a permutation matrix") {

                for (int i = 0; i < std::get<4>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<4>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<5>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<5>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixCols is a permutation matrix") {

                for (int i = 0; i < std::get<6>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<6>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<7>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<7>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixRows * A * permutMatrixCols = B ") {

                for (int i = 0; i < std::get<8>(testEquiv).rows(); ++i) {

                    for (int j = 0; j < std::get<8>(testEquiv).cols(); ++j) {

                        REQUIRE(std::get<8>(testEquiv).coeffRef(i,j) == std::get<9>(testEquiv).coeffRef(i,j));
                    }
                }
            }
        }
        if (!std::get<0>(testEquiv)) {REQUIRE(std::get<0>(testEquiv) == false );}

    }
}


TEST_CASE( "Equivalence of linear programs : algo_scale", "[Algo_scale]" ) {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> testEquiv = test_equiv_lp("algo_scale");

    SECTION( "Test of effective equivalence" ) {

        if (std::get<0>(testEquiv)) {

            REQUIRE(std::get<2>(testEquiv).size() == std::get<3>(testEquiv).size());

            SECTION( "Test if the associated matrices A and B have the same set of coefficients" ) {

                for (int i = 0; i < std::get<2>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<2>(testEquiv)[i] == std::get<3>(testEquiv)[i]);
                }
            }

            SECTION( "Test if permutMatrixRows is a permutation matrix") {

                for (int i = 0; i < std::get<4>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<4>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<5>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<5>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixCols is a permutation matrix") {

                for (int i = 0; i < std::get<6>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<6>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<7>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<7>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixRows * A * permutMatrixCols = B ") {

                for (int i = 0; i < std::get<8>(testEquiv).rows(); ++i) {

                    for (int j = 0; j < std::get<8>(testEquiv).cols(); ++j) {

                        REQUIRE(std::get<8>(testEquiv).coeffRef(i,j) == std::get<9>(testEquiv).coeffRef(i,j));
                    }
                }
            }
        }
    }
}



TEST_CASE( "Equivalence of linear programs : algo_scale_enhanced", "[Algo_scale_enhanced]" ) {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> testEquiv = test_equiv_lp("algo_scale_enhanced");

    SECTION( "Test of  effective equivalence" ) {

        if (std::get<0>(testEquiv)) {

            REQUIRE(std::get<2>(testEquiv).size() == std::get<3>(testEquiv).size());

            SECTION( "Test if the associated matrices A and B have the same set of coefficients" ) {

                for (int i = 0; i < std::get<2>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<2>(testEquiv)[i] == std::get<3>(testEquiv)[i]);
                }
            }

            SECTION( "Test if permutMatrixRows is a permutation matrix") {

                for (int i = 0; i < std::get<4>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<4>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<5>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<5>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixCols is a permutation matrix") {

                for (int i = 0; i < std::get<6>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<6>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<7>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<7>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixRows * A * permutMatrixCols = B ") {

                for (int i = 0; i < std::get<8>(testEquiv).rows(); ++i) {

                    for (int j = 0; j < std::get<8>(testEquiv).cols(); ++j) {

                        REQUIRE(std::get<8>(testEquiv).coeffRef(i,j) == std::get<9>(testEquiv).coeffRef(i,j));
                    }
                }
            }
        }
    }
}



TEST_CASE( "Equivalence of linear programs : algo_scale_enhanced2", "[Algo_scale_enhanced2]" ) {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> testEquiv = test_equiv_lp("algo_scale_enhanced2");

    SECTION( "Test of effective equivalence" ) {

        if (std::get<0>(testEquiv)) {

            REQUIRE(std::get<2>(testEquiv).size() == std::get<3>(testEquiv).size());

            SECTION( "Test if the associated matrices A and B have the same set of coefficients" ) {

                for (int i = 0; i < std::get<2>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<2>(testEquiv)[i] == std::get<3>(testEquiv)[i]);
                }
            }

            SECTION( "Test if permutMatrixRows is a permutation matrix") {

                for (int i = 0; i < std::get<4>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<4>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<5>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<5>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixCols is a permutation matrix") {

                for (int i = 0; i < std::get<6>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<6>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<7>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<7>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixRows * A * permutMatrixCols = B ") {

                for (int i = 0; i < std::get<8>(testEquiv).rows(); ++i) {

                    for (int j = 0; j < std::get<8>(testEquiv).cols(); ++j) {

                        REQUIRE(std::get<8>(testEquiv).coeffRef(i,j) == std::get<9>(testEquiv).coeffRef(i,j));
                    }
                }
            }
        }
    }
}

*/


TEST_CASE( "Equivalence of linear programs: algo_enhanced", "[Algo_enhanced]" ) {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> testEquiv = test_equiv_lp("algo_enhanced");

    SECTION( "Test of if effective equivalence" ) {

        if (std::get<0>(testEquiv)) {

            REQUIRE(std::get<2>(testEquiv).size() == std::get<3>(testEquiv).size());

            SECTION( "Test if the associated matrices A and B have the same set of coefficients" ) {

                for (int i = 0; i < std::get<2>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<2>(testEquiv)[i] == std::get<3>(testEquiv)[i]);
                }
            }

            SECTION( "Test if permutMatrixRows is a permutation matrix") {

                for (int i = 0; i < std::get<4>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<4>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<5>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<5>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixCols is a permutation matrix") {

                for (int i = 0; i < std::get<6>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<6>(testEquiv)[i] == 1);
                }

                for (int i = 0; i < std::get<7>(testEquiv).size(); ++i) {

                    REQUIRE(std::get<7>(testEquiv)[i] == 1);
                }
            }

            SECTION( "Test if permutMatrixRows * A * permutMatrixCols = B ") {

                for (int i = 0; i < std::get<8>(testEquiv).rows(); ++i) {

                    for (int j = 0; j < std::get<8>(testEquiv).cols(); ++j) {

                        REQUIRE(std::get<8>(testEquiv).coeffRef(i,j) == std::get<9>(testEquiv).coeffRef(i,j));
                    }
                }
            }
        }
    }
}

