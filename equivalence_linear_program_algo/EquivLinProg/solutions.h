//
// Created by Admin on 2025-10-13.
//

#ifndef EQUIVLINPROG_SOLUTIONS_H
#define EQUIVLINPROG_SOLUTIONS_H
#include <Eigen/Sparse>

class Solutions {
private:
    std::string algoName_;
    Eigen::SparseMatrix<double> sparseMatrixA;
    Eigen::SparseMatrix<double> sparseMatrixB;
    bool result_;
    Eigen::SparseMatrix<double> permutMatrixRows;
    Eigen::SparseMatrix<double> permutMatrixCols;
    std::string outComme;

public:
    Solutions();
    Solutions(Eigen::SparseMatrix<double> A,Eigen::SparseMatrix<double> B,
        std::tuple<bool,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
        std::string> result,std::string algoName);
    ~Solutions();
    std::tuple<bool,
    std::string, std::vector<double>, std::vector<double>,
    std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
    Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> get_elts_to_validate_solution(bool verbose = false);

    bool validate_a_solution();

};



#endif //EQUIVLINPROG_SOLUTIONS_H