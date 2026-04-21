//
// Created by Admin on 2025-10-08.
//

#ifndef EQUIVLINPROG_MATRIX_TOOL_H
#define EQUIVLINPROG_MATRIX_TOOL_H
#include <Eigen/Sparse>

Eigen::SparseMatrix<double> gen_matrix_with_given_non_zeros(int rows, int cols, std::vector<std::vector<int>> setOfSetOfTriplets);
Eigen::SparseMatrix<double> randomly_generate_sparse_matrix(int numRows, int numCols);
Eigen::SparseMatrix<double> generate_sorted_rows_cols_based_value_sparse_matrix(bool row, Eigen::SparseMatrix<double> & matrix);
Eigen::SparseMatrix<double> sparse_row_col(bool row, Eigen::SparseMatrix<double> & matrix, int rank);
Eigen::Triplet<double> exchange_rows_or_col(bool row, std::vector<std::pair<int, int>> pairsExchangIndexes, Eigen::Triplet<double> triplet);
Eigen::SparseMatrix<double> permut_rows_or_cols_matrix(bool row, Eigen::SparseMatrix<double> & matrix, const std::vector<std::pair<int, int>>& pairsExchangIndexes);
std::vector<std::vector<int>>  initialize_struct(int numRowsCols);
std::vector<std::pair<int,int>> get_one_seq_corr(int k, std::vector<std::tuple<int, int, int>> tree);
bool is_not_inside(std::tuple<int, int, int> tuple, std::vector<std::pair<int,int>> &  seq_corr);
std::vector<std::tuple<int, int, int>> gen_tree_of_corr(std::vector<std::vector<int>> corrRowColA);
std::vector<std::vector<std::tuple<int, int, int>>> get_children_per_node(std::vector<std::tuple<int, int, int>> tree);
std::vector<std::vector<int>> depth_first_search(std::vector<std::vector<std::tuple<int, int, int>>> allChildren);
void build_permutation_matrix(std::vector<Eigen::Triplet<double>>& tripletListRows, Eigen::Triplet<double> triplet);
std::tuple<Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
           Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
           Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>> get_sparse_matrice(const std::string& set_web, const std::string& type_inst,const std::string& set_inst,
                                                                                        const std::string& set_inst_test,const std::string& inst);
void initialize_failing_set(std::vector<std::vector<int>>& Fail, std::vector<std::vector<int>>& corrRowColA);
void gen_random_synthetic_insts(const int& num_lines, const int& num_cols, const int& maxNonZeros, const int& maxNumModifiedCoefs,
                    const double& minCoef, const double& maxCoef,
                    const std::vector<std::vector<std::string>>& list_set_inst, const std::vector<std::string>& list_type_inst, const int& num_insts);
bool pair_is_not_inside(std::pair<int,int> pair, std::vector<std::pair<int,int>> incom_couples_rows);
bool tuple_is_not_inside(std::tuple<int,int,int,int> tuple, std::vector<std::tuple<int,int,int,int>> incom_couples_rows_cols);
bool set_is_not_inside(std::vector<std::pair<int,int>> set_of_incom_couples_rows, std::vector<std::vector<std::pair<int,int>>> incom_couples_rows);
std::tuple<bool, Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> reduce_size_of_matrices(Eigen::SparseMatrix<double>& matrixA, Eigen::SparseMatrix<double>& matrixB);
int get_min_index(std::vector<size_t> vector);
void record_solution_with_python(const std::string& model, const std::string& solver, const std::string& max_scale, const std::string& timeout_or_not,
    const std::string& type_inst, const std::string& set_inst,const std::string& set_inst_test,const std::string& inst,const std::string& set_web,
    const std::string& result,const std::string& time_ex , const std::string&  valid_or_not ,const std::string& num_columns , const std::string& num_lines,
    const std::string& luby_cst,  const std::string& seed);

std::vector<std::pair<int, int>> gen_pairs_exchang_indexes(int numRowsOrCols, int notPermutedRowOrCol);

#endif //EQUIVLINPROG_MATRIX_TOOL_H