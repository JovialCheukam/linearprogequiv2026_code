//
// Created by Admin on 2025-10-14.
//

#ifndef EQUIVLINPROG_ALGO_ENHANCED_H
#define EQUIVLINPROG_ALGO_ENHANCED_H
#include <Eigen/Sparse>
#include <chrono>
#include <vector>
#include "pairhash.h"

class Algo_equal {
private:
    /* data */
public:
    Algo_equal(/* args */);
    ~Algo_equal();
    bool chek_equal_set(bool row, Eigen::SparseMatrix<double> sortedVec1, Eigen::SparseMatrix<double> sortedVec2);
    std::vector<std::vector<int>> get_set_indexes_corr(bool row, int numRowsCols, Eigen::SparseMatrix<double>& sortedMatrixA,
                                                          Eigen::SparseMatrix<double>& sortedMatrixB);
    std::vector<std::vector<int>> get_set_indexes_corr_cols_by_bounds_matrices(int numCols, Eigen::SparseMatrix<double>& matrixBoundsA,
                                                          Eigen::SparseMatrix<double>& matrixBoundsB);
};

class Algo_enhanced {

public:
    Algo_enhanced(/* args */);
    ~Algo_enhanced();

    std::tuple<bool,
    Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
    std::string> enhanced_check_equiv_between_asso_lp_matrices_learning(bool luby, const std::string& algoType, Eigen::SparseMatrix<double>& matrixA,Eigen::SparseMatrix<double>& matrixB,
                                                                                                            Eigen::SparseMatrix<double>& matrixLowerBoundsA, Eigen::SparseMatrix<double>& matrixLowerBoundsB,
                                                                                                            Eigen::SparseMatrix<double>& matrixUpperBoundsA, Eigen::SparseMatrix<double>& matrixUpperBoundsB,
                                                                                                                std::chrono::microseconds& timeout,
                                                                                                                std::vector<std::vector<int>>&  FailRows,
                                                                                                                std::vector<std::vector<int>>&  FailCols,
                                                                                                                std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash>& learned_wrong_row_corr,
                                                                                                                std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols,
                                                                                                                int& number_of_failed_perfect_matching);

    std::tuple<bool,
    Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
std::string> luby_learning(int intial_num_failed, const std::string& algoType, Eigen::SparseMatrix<double>& matrixA,Eigen::SparseMatrix<double>& matrixB,
                                                    Eigen::SparseMatrix<double>& matrixLowerBoundsA, Eigen::SparseMatrix<double>& matrixLowerBoundsB,
                                                    Eigen::SparseMatrix<double>& matrixUpperBoundsA, Eigen::SparseMatrix<double>& matrixUpperBoundsB,
                                                                                                            std::chrono::microseconds timeout);
};


#endif //EQUIVLINPROG_ALGO_ENHANCED_H
