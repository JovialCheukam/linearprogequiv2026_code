//
// Created by Admin on 2025-10-13.
//

#include "solutions.h"
#include "matrix_tool.h"
#include <iostream>
#include <cassert>
#include "printing_tool.h"


bool is_inside_set_of_strings(std::string str, std::vector<std::string> strings) {
    for (std::string word : strings) {
        if (str == word) return true;
    }
    return false;
}

Solutions::Solutions(){}

Solutions::Solutions(Eigen::SparseMatrix<double> A,Eigen::SparseMatrix<double> B,
                    std::tuple<bool,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
                    std::string> result,std::string algoName) {
    algoName_ = algoName;
    if (is_inside_set_of_strings(algoName_ , {"algo_scale","algo_scale_enhanced","algo_scale_enhanced2"})) {
        sparseMatrixA = std::get<3>(result);
        sparseMatrixB = std::get<4>(result);
    }
    else {
        sparseMatrixA = std::move(A);
        sparseMatrixB = std::move(B);
    }
    result_ = std::get<0>(result);
    permutMatrixRows = std::get<1>(result);
    permutMatrixCols = std::get<2>(result);
    outComme = std::get<5>(result);

}

Solutions::~Solutions() = default;

std::tuple<bool,
std::string, std::vector<double>, std::vector<double>,
std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>>  Solutions::get_elts_to_validate_solution(bool verbose) {


    // Print the sorted sparse matrix
    //std::cout << "permutRow = \n" << permutMatrixRows << std::endl;
    //std::cout << "permutCol = \n" << permutMatrixCols << std::endl;

    /*
     * Elements for asserting that sparseMatrixA and sparseMatrixB have the same set of coefficients
    */

    // get the sorted set of non zeros coefficient of sparseMatrixA and sparseMatrixB
    std::vector<double> nonZerosCoefA, nonZerosCoefB;

    for (int k = 0; k < sparseMatrixA.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(sparseMatrixA, k); it; ++it) {
            if (it.value() != 0)  nonZerosCoefA.push_back(it.value());
        }
    }

    for (int k = 0; k < sparseMatrixB.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(sparseMatrixB, k); it; ++it) {
            if (it.value() != 0) nonZerosCoefB.push_back(it.value());
        }
    }
    // Sort the extracted elements of nonZerosCoefA based on their values
    std::ranges::sort(nonZerosCoefA,
                      [](const double a, const double b) {
                          return a < b; // Sort by value in ascending order
                      });

    // Sort the extracted elements of nonZerosCoefB based on their values
    std::ranges::sort(nonZerosCoefB,
                      [](const double a, const double b) {
                          return a < b; // Sort by value in ascending order
                      });

    /*
     * Elements for asserting that permutMatrixRows is a permutation matrix
    */
    assert(permutMatrixRows.rows() == permutMatrixRows.cols());
    std::vector<int> sumRowsListOfPermutMatrixRows;
    std::vector<int> sumColsListOfPermutMatrixRows;

    for (unsigned i=0; i<permutMatrixRows.rows(); i++) {
        sumRowsListOfPermutMatrixRows.push_back(sparse_row_col(true, permutMatrixRows, i).sum());
        sumColsListOfPermutMatrixRows.push_back(sparse_row_col(false, permutMatrixRows, i).sum());
    }

    /*
     * Elements for asserting that permutMatrixCols is a permutation matrix
    */
    assert(permutMatrixCols.rows() == permutMatrixCols.cols());
    std::vector<int> sumRowsListOfPermutMatrixCols;
    std::vector<int> sumColsListOfPermutMatrixCols;

    for (unsigned i=0; i<permutMatrixCols.rows(); i++) {
        sumRowsListOfPermutMatrixCols.push_back(sparse_row_col(true, permutMatrixCols, i).sum());
        sumColsListOfPermutMatrixCols.push_back(sparse_row_col(false, permutMatrixCols, i).sum());
    }

    /*
     * Elements for asserting that permutMatrixRows * A * permutMatrixCols = B is a permutation matrix
    */
    if (verbose) {
        std::cout << "permutMatrixRows = " <<  permutMatrixRows << std::endl;
        std::cout << "permutMatrixCols = " <<  permutMatrixCols << std::endl;
    }

    Eigen::SparseMatrix<double> permutSparseMatrixA ;

    if (permutMatrixRows.rows() != 0 && permutMatrixCols.rows() != 0 && permutMatrixRows.cols() != 0 && permutMatrixCols.cols() != 0) {
        permutSparseMatrixA = permutMatrixRows * sparseMatrixA * permutMatrixCols;
    }

    return std::make_tuple(result_,algoName_,nonZerosCoefA,nonZerosCoefB,sumRowsListOfPermutMatrixRows,
        sumColsListOfPermutMatrixRows,sumRowsListOfPermutMatrixCols,sumColsListOfPermutMatrixCols,
        permutSparseMatrixA, sparseMatrixB);

}

bool Solutions::validate_a_solution() {

    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> test = this->get_elts_to_validate_solution();

    bool valid(true);

    if (std::get<0>(test)) {

        if(std::get<2>(test).size() != std::get<3>(test).size()) valid = false;

        if (valid) {// Test if the associated matrices A and B have the same set of coefficients"

            for (int i = 0; i < std::get<2>(test).size(); ++i) {

                if(std::get<2>(test)[i] != std::get<3>(test)[i]) {
                    valid = false;
                    break;
                }
            }
        }

        if (valid) {// Test if permutMatrixRows is a permutation matrix"

            for (int i = 0; i < std::get<4>(test).size(); ++i) {

                if(std::get<4>(test)[i] != 1) {
                        valid = false;
                        break;
                }
            }
        }

        if (valid) {
            for (int i = 0; i < std::get<5>(test).size(); ++i) {

                if(std::get<5>(test)[i] != 1){
                        valid = false;
                        break;
                }
            }
        }

        if (valid) { // "Test if permutMatrixCols is a permutation matrix"

            for (int i = 0; i < std::get<6>(test).size(); ++i) {

                if(std::get<6>(test)[i] != 1) {
                    valid = false;
                    break;
                }
            }
        }

        if (valid) {
            for (int i = 0; i < std::get<7>(test).size(); ++i) {

                if(std::get<7>(test)[i] != 1) {
                    valid = false;
                    break;
                }
            }
        }

        if (valid) {  // "Test if permutMatrixRows * A * permutMatrixCols = B ") {

            for (int i = 0; i < std::get<8>(test).rows(); ++i) {

                for (int j = 0; j < std::get<8>(test).cols(); ++j) {

                    if(std::get<8>(test).coeffRef(i,j) != std::get<9>(test).coeffRef(i,j)){
                        valid = false;
                        break;
                    }
                }
                if (!valid) break;
            }
        }
    }
    return valid;
}
