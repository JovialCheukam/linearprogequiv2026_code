//
// Created by Admin on 2025-10-08.
//

#ifndef EQUIVLINPROG_PERMUTATION_H
#define EQUIVLINPROG_PERMUTATION_H
#include <vector>
#include <Eigen/Sparse>
#include "pairhash.h"

bool i_is_mobile_elt(std::vector<std::pair<int,int>> permut, int i);
bool mobile_elt_exist(std::vector<std::pair<int,int>> & permut);
std::pair<int,int> largest_mobile_elt(std::vector<std::pair<int,int>> & permut);
void swap_with_adjac_k_pointo_elt(int k, std::vector<std::pair<int,int>>& permut);
void reverse_dir_all_larg_than_k(int largest_mobile_elt, std::vector<std::pair<int,int>> & permut);
void gen_all_permutations(std::vector<std::pair<int,int>> & permut);
std::pair<bool, std::vector<int>> is_same_permut_for_elts(Eigen::SparseMatrix<double> row1,Eigen::SparseMatrix<double> row2_corr1,
                                                                        Eigen::SparseMatrix<double> row3,Eigen::SparseMatrix<double> row4_corr3);
bool permut_by_avoiding_wrong_learned_permutations(std::vector<int>& permut_vec, int i_fail, int m, std::vector<std::vector<int>>& Fail);

bool gen_good_permutation0(std::vector<int>& permut_vec,  bool succesPermut, int m, std::vector<std::vector<int>>& Fail,
                                                          std::vector<int>& permutationR, std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols,
                                                          int & number_of_failed_couple_permut,int & number_of_failed_perfect_matching, bool & restart);

bool gen_good_permutation_without_wrong_learned_permutation0(std::vector<int>& permut_vec,  bool succesPermut, int m, std::vector<std::vector<int>>& Fail,
                                                          const std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> & learned_wrong_scaling,
                                                          int & number_of_failed_couple_permut, int & number_of_failed_perfect_matching, bool & restart);
bool gen_permutation_without_incomp(std::vector<int>& permut_vec,  int m, std::vector<std::vector<int>>& Fail,
                                                          std::vector<int>& permutationR, std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols);
std::vector<int> get_permutation(Eigen::SparseMatrix<double> row1,Eigen::SparseMatrix<double> row2);
bool is_inside(int permut_vec_i_fail, std::vector<int>& Fail_i_fail);
bool vectors_are_equal(std::vector<int>& v1, std::vector<int>& v2);
bool gen_next_permut_greater_in_lexicographic_order(bool succesPermut, std::vector<int>& permut_vec);


#endif //EQUIVLINPROG_PERMUTATION_H