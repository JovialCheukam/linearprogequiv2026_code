#include <cassert>
#include "permutation.h"
#include <iostream>
#include <Eigen/Sparse>
#include <unordered_map>
#include "printing_tool.h"

bool i_is_mobile_elt(std::vector<std::pair<int,int>> permut, int i) {
    if (permut.size() > 1) {
        if (i == 0) {
            if  (permut[0].second > 0 && permut[0].first > permut[1].first) {
                return true;
            }
            else {
                return false;
            }
        }
        else if (i <= (permut.size() - 2)) {
             if (permut[i].second < 0 && permut[i].first > permut[i-1].first) return true;
             if (permut[i].second > 0 && permut[i].first > permut[i+1].first) return true;
             else return false;
            }
        else {
                if (permut[permut.size()-1].second < 0 && permut[permut.size()-1].first > permut[permut.size()-2].first) return true;
                else return false;
        }

    }
    else return false;
}

bool mobile_elt_exist(std::vector<std::pair<int,int>> & permut) {
    for (unsigned i=0; i<permut.size(); ++i) {
        if (i_is_mobile_elt(permut,i)) return true;
    }
    return false;
}

std::pair<int,int> largest_mobile_elt(std::vector<std::pair<int,int>> & permut) {
    int largest_mobile = -1 ;
    int ind_largest_mobile = -1;
    for (unsigned i=0; i<permut.size(); ++i) {
        if (i_is_mobile_elt(permut,i)) {
            if (permut[i].first > largest_mobile) {
                largest_mobile = permut[i].first;
                ind_largest_mobile = i;
            }
        }
    }
    return std::make_pair(ind_largest_mobile,largest_mobile);
}

void swap_with_adjac_k_pointo_elt(int k, std::vector<std::pair<int,int>>& permut) {

    if (permut[k].second < 0) {
        std::pair<int,int> keep = permut[k - 1];
        permut[k - 1] = permut[k];
        permut[k] = keep;
    }
    else {
        std::pair<int,int> keep = permut[k + 1];
        permut[k + 1] = permut[k];
        permut[k] = keep;
    }
}

void reverse_dir_all_larg_than_k(int largest_mobile_elt, std::vector<std::pair<int,int>> & permut) {

    for (unsigned i=0; i<permut.size(); ++i) {
        if (permut[i].first > largest_mobile_elt) {
            if (permut[i].second > 0) {
                permut[i].second = -1;
            }
            else permut[i].second = 1;
        }
    }
}

void gen_all_permutations(std::vector<std::pair<int,int>> & permut) {

    while (mobile_elt_exist(permut)) {
        std::pair<int,int> k_and_largest_mobile_elt = largest_mobile_elt(permut);
        swap_with_adjac_k_pointo_elt(k_and_largest_mobile_elt.first, permut);
        reverse_dir_all_larg_than_k(k_and_largest_mobile_elt.second, permut);

        std::cout << "[" ;
        for (unsigned i=0; i<permut.size(); ++i) {
            std::cout << permut[i].first;
            if (i < permut.size() - 1) std::cout << ", ";
        }
        std::cout << "] , ";

        std::cout << "[" ;
        for (unsigned i=0; i<permut.size(); ++i) {
            std::cout << permut[i].second;
            if (i < permut.size() - 1) std::cout << ", ";
        }
        std::cout << "] , mobile_elt_exist = " << mobile_elt_exist(permut) <<  std::endl;


    }
}

std::pair<int,std::vector<int>> exchange_with_smallest_greater_than(std::vector<int> permut_vec) {
    int k_e = -1;
    const int m = permut_vec.size();
    std::vector<int> permut_vec_final;
    for (int j=1; j<m; ++j) {
        if (permut_vec[j] > permut_vec[0]) {

            if (k_e == -1) {
                k_e = j;

            }
            else {
                if (permut_vec[k_e] > permut_vec[j]) {
                    k_e = j;
                }
            }
        }
    }
    if (k_e != -1) {

        int k = permut_vec[0];
        permut_vec[0] = permut_vec[k_e];
        permut_vec[k_e] = k;

        permut_vec_final.push_back(permut_vec[0]);
        std::vector<int> end_permut_vec;
        for (unsigned i=1; i<permut_vec.size(); ++i) {
            end_permut_vec.push_back(permut_vec[i]);
        }
        std::sort(end_permut_vec.begin(), end_permut_vec.end());
        for (unsigned i=0; i<end_permut_vec.size(); ++i) {
            permut_vec_final.push_back(end_permut_vec[i]);
        }
    }

    return std::make_pair(k_e, permut_vec_final);
}

bool is_inside(int permut_vec_i_fail, std::vector<int>& Fail_i_fail) {
    for (unsigned i=0; i<Fail_i_fail.size(); ++i) {
        if (permut_vec_i_fail == Fail_i_fail[i]) return true;
    }
    return false;
}

std::pair<bool,int> is_inside_all(std::vector<int>& permut_vec, std::vector<std::vector<int>>& Fail) {

    for (unsigned i=0; i<permut_vec.size(); ++i) {
        if (is_inside(permut_vec[i], Fail[i])) return std::make_pair(true, i);
    }
    return std::make_pair(false, -1);
}

bool vectors_are_equal(std::vector<int>& v1, std::vector<int>& v2) {

    for (int i=0; i<v1.size(); ++i) {
        if (v1[i] != v2[i]) return false;
    }
    return true;
}

bool permut_by_avoiding_wrong_learned_permutations0(std::vector<int>& permut_vec, int i_fail, int m) {

    if (i_fail == -1) {

        return true;
    }

    std::vector<int> permut_vec_i_fail;
    for (unsigned i=i_fail; i<permut_vec.size(); ++i) {
        int u = permut_vec[i];
        permut_vec_i_fail.push_back(u);
    }

    if (i_fail < m - 1) {
        std::pair<int,std::vector<int>> k_e_permut_vec_i_fail = exchange_with_smallest_greater_than(permut_vec_i_fail);
        int size_permut_vec_i_fail(k_e_permut_vec_i_fail.second.size());
        for (unsigned i=0; i<size_permut_vec_i_fail; ++i) {
            permut_vec[m - 1 - i] = k_e_permut_vec_i_fail.second[size_permut_vec_i_fail - 1 - i];
        }

        if (k_e_permut_vec_i_fail.first != -1) {

            return true;
        }
    }

    if (i_fail > 0) {
        for (unsigned i=0; i<i_fail; ++i) {

            if (permut_vec[i_fail - i- 1] < permut_vec[i_fail - i]) {



                std::vector<int> permut_vec_i_fail_i_1;
                for (unsigned j=i_fail - i - 1; j<permut_vec.size(); ++j) {
                    permut_vec_i_fail_i_1.push_back(permut_vec[j]);
                }

                std::pair<int,std::vector<int>> k_e_permut_vec_i_fail_i_1 = exchange_with_smallest_greater_than(permut_vec_i_fail_i_1);

                int size_permut_vec_i_fail_i_1(k_e_permut_vec_i_fail_i_1.second.size());

                for (unsigned j=0; j<size_permut_vec_i_fail_i_1; ++j) {
                    permut_vec[m - 1 - j] = k_e_permut_vec_i_fail_i_1.second[size_permut_vec_i_fail_i_1 - 1 - j];
                }
                return true;
            }

        }
    }

    return false;
}


bool permut_by_avoiding_wrong_learned_permutations(std::vector<int>& permut_vec, int i_fail, int m, std::vector<std::vector<int>>& Fail,
                                            int & number_of_failed_couple_permut,int & number_of_failed_perfect_matching, bool & restart) {

    bool new_permut = permut_by_avoiding_wrong_learned_permutations0(permut_vec, i_fail,  m);

    if (!new_permut) return false;

    std::pair<bool, int> is_inside_or_not = is_inside_all(permut_vec,Fail);


    while (is_inside_or_not.first) {

        number_of_failed_couple_permut ++;

        if (number_of_failed_couple_permut == number_of_failed_perfect_matching) {
            restart = true;
            return false;
        }


        new_permut =  permut_by_avoiding_wrong_learned_permutations0(permut_vec,is_inside_or_not.second,m);


        if (!new_permut) return false;

        is_inside_or_not = is_inside_all(permut_vec,Fail);


    }
    return true;
}

bool two_consec_elts_are_in_increas_order(std::vector<int>& permut_vec) {
    for (unsigned i=0; i<permut_vec.size()-1; ++i) {
        if (permut_vec[i] < permut_vec[i+1]) return true;
    }
    return false;
}

bool gen_next_permut_greater_in_lexicographic_order(bool succesPermut, std::vector<int>& permut_vec) {

    if (succesPermut) return true;

    if (two_consec_elts_are_in_increas_order(permut_vec)) {
        int largestIndexI(0);
        for (int i=0; i<permut_vec.size()-1; ++i) {
            if (permut_vec[i] < permut_vec[i+1]) largestIndexI = i;
        }
        int largestIndexJ(largestIndexI + 1);
        for (int j=largestIndexI+1; j<permut_vec.size(); ++j) {
            if (permut_vec[largestIndexI] < permut_vec[j]) largestIndexJ = j;
        }
        int  swapper(permut_vec[largestIndexI]);
        permut_vec[largestIndexI] = permut_vec[largestIndexJ];
        permut_vec[largestIndexJ] = swapper;

        std::vector<int> reverser;
        for (int i=permut_vec.size()-1; i>=largestIndexI+1; --i) {
            reverser.push_back(permut_vec[i]);
        }

        int m = permut_vec.size();
        int size_reverser(reverser.size());
        for (int i=0; i<size_reverser; ++i) {
            permut_vec[m - 1 - i] = reverser[size_reverser - 1 - i];
        }
        return true;
    }
    return false;
}


std::pair<bool,int> a_corr_is_incompatible(std::vector<int>& permut_vec, std::vector<std::vector<int>>& Fail,
                                             std::vector<int>& permutationRows, std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols) {


    for (unsigned i=0; i<permut_vec.size(); ++i) {
        if (is_inside(permut_vec[i],Fail[i])) {

            return std::make_pair(true, i);
        }
    }

    for (int k =0; k<incom_couples_rows_cols.size(); ++k) {

        int i = std::get<0>(incom_couples_rows_cols[k]);
        int p_i = std::get<1>(incom_couples_rows_cols[k]);
        int j = std::get<2>(incom_couples_rows_cols[k]);
        int q_j = std::get<3>(incom_couples_rows_cols[k]);

        if (permut_vec[j] == q_j && permutationRows[i] == p_i) return std::make_pair(true, j);
    }

    return std::make_pair(false, -1);
}


bool gen_good_permutation0(std::vector<int>& permut_vec,  bool succesPermut, int m, std::vector<std::vector<int>>& Fail,
                                                          std::vector<int>& permutationR, std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols,
                                                          int & number_of_failed_couple_permut,int & number_of_failed_perfect_matching, bool & restart) {


    bool new_permut = gen_next_permut_greater_in_lexicographic_order(succesPermut, permut_vec);


    if (!new_permut) return false;


    std::pair<bool,int> a_corr_is_incomp = a_corr_is_incompatible(permut_vec, Fail, permutationR, incom_couples_rows_cols);


    while (a_corr_is_incomp.first ) {

        number_of_failed_couple_permut ++;

        if (number_of_failed_couple_permut == number_of_failed_perfect_matching) {
            restart = true;
            return false;
        }

        new_permut = permut_by_avoiding_wrong_learned_permutations(permut_vec, a_corr_is_incomp.second, m,Fail,
            number_of_failed_couple_permut,number_of_failed_perfect_matching, restart);


        if (!new_permut) return false;

        a_corr_is_incomp = a_corr_is_incompatible(permut_vec, Fail, permutationR, incom_couples_rows_cols);

    }
    return new_permut;
}

std::pair<bool,int> is_wrong_couple_matching(const std::vector<int> & permut_vec,
                                            std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_scaling) {

    for (const auto& pair : learned_wrong_scaling) {

        int k1 = pair.first.first;
        int k2 = pair.first.second;
        for (int i = 0; i < learned_wrong_scaling[pair.first].size(); ++i) {
            int l1 = learned_wrong_scaling[pair.first].at(i).first;
            int l2 = learned_wrong_scaling[pair.first].at(i).second;
            std::vector<std::pair<int,int>> uncompatibleScalings;
            uncompatibleScalings.push_back(std::make_pair(k1,k2));
            uncompatibleScalings.push_back(std::make_pair(l1,l2));

            // Sort  based on first component of the couple
            std::sort(uncompatibleScalings.begin(), uncompatibleScalings.end(),
                  [](std::pair<int,int> a, std::pair<int,int> b) {
                      return a.first < b.first; // Sort by first component of the couple in ascending order
                  });
            int i1 = uncompatibleScalings[0].first;
            int i2 = uncompatibleScalings[0].second;
            int j1 = uncompatibleScalings[1].first;
            int j2 = uncompatibleScalings[1].second;

            if (permut_vec[i1] == i2 && permut_vec[j1] == j2) {
                return std::make_pair(true , j1);
            }
        }
    }
    return std::make_pair(false,-1);
}



bool gen_good_permutation_without_wrong_learned_permutation0(std::vector<int>& permut_vec,  bool succesPermut, int m, std::vector<std::vector<int>>& Fail,
                                                          const std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> & learned_wrong_scaling,
                                                          int & number_of_failed_couple_permut, int & number_of_failed_perfect_matching, bool & restart) {


    bool new_permut = gen_next_permut_greater_in_lexicographic_order(succesPermut, permut_vec);

    if (!new_permut) return false;

    std::vector<int> permutationR;
    std::vector<std::tuple<int,int,int,int>> incom_couples_rows_cols;


    std::pair<bool,int> a_corr_is_incomp = a_corr_is_incompatible(permut_vec, Fail, permutationR, incom_couples_rows_cols);


    std::pair<bool,int> wrong_couple_matching_j1 =  is_wrong_couple_matching(permut_vec,learned_wrong_scaling);



    while (a_corr_is_incomp.first || wrong_couple_matching_j1.first) {

        number_of_failed_couple_permut ++;

        if (number_of_failed_couple_permut == number_of_failed_perfect_matching) {
            restart = true;
            return false;
        }

        int failingIndex;
        if (a_corr_is_incomp.first && wrong_couple_matching_j1.first) failingIndex = std::max(a_corr_is_incomp.second,wrong_couple_matching_j1.second);
        else {
            if (a_corr_is_incomp.first) failingIndex = a_corr_is_incomp.second;
            else failingIndex = wrong_couple_matching_j1.second;
        }

        new_permut = permut_by_avoiding_wrong_learned_permutations(permut_vec, failingIndex, m,Fail,
                    number_of_failed_couple_permut, number_of_failed_perfect_matching, restart);


        if (!new_permut) return false;

        a_corr_is_incomp = a_corr_is_incompatible(permut_vec, Fail, permutationR, incom_couples_rows_cols);

        wrong_couple_matching_j1 =  is_wrong_couple_matching(permut_vec,learned_wrong_scaling);


    }
    return new_permut;
}

std::vector<double> find_non_duplicate(int len_row, Eigen::SparseMatrix<double>& row) {
    std::unordered_map<double, int>  temoin_list;
    std::vector<double> non_duplicate;

    for (unsigned i=0; i<len_row; ++i) {
            temoin_list[row.coeffRef(0,i)] = 0;
    }
    for (unsigned i=0; i<len_row; ++i) {
        temoin_list[row.coeffRef(0,i)] = temoin_list[row.coeffRef(0,i)] + 1;
    }
    for (const auto& pair : temoin_list) {
            if (temoin_list[pair.first] == 1) {
                non_duplicate.push_back(pair.first);
            }
    }
    return non_duplicate;
}

std::vector<double> distinct_elts(int len_row, Eigen::SparseMatrix<double>& row) {

    std::unordered_map<double, int>  temoin_list;
    std::vector<double> distinct_elts;

    for (unsigned i=0; i<len_row; ++i) {
        temoin_list[row.coeffRef(0,i)] = 0;
    }
    for (const auto& pair : temoin_list) {

            distinct_elts.push_back(pair.first);
    }
    return distinct_elts;
}

std::vector<std::vector<int>> get_set_of_seq_corr_cols(Eigen::SparseMatrix<double>& row1, Eigen::SparseMatrix<double>& row2_corr1) {
    std::vector<std::vector<int>> set_of_seq_corr_cols;
    int len_row = row1.cols();
    for (unsigned i=0; i<len_row; ++i) {
        std::vector<int> corrColsI;
        for (unsigned j=0; j<len_row; ++j) {
            if (row1.coeffRef(0,i) == row2_corr1.coeffRef(0,j)) {
                corrColsI.push_back(j);
            }
        }
        set_of_seq_corr_cols.push_back(corrColsI);
    }
    return set_of_seq_corr_cols;
}

std::vector<int> get_permutation(Eigen::SparseMatrix<double> row1,Eigen::SparseMatrix<double> row2) {
    int l = row1.cols();

    assert(l == row2.cols());
    std::vector<double> non_duplicates = find_non_duplicate(l,row1);
    std::vector<int> permut_vect;
    if (!non_duplicates.empty()) {
        for (unsigned i=0; i<l; ++i) {
            permut_vect.push_back(-1);
        }

        for (int i=0; i<l; ++i) {
            for (int j=0; j<non_duplicates.size(); ++j) {
                if (row1.coeffRef(0,i) == non_duplicates[j]) {
                    for (int k=0; k<l; ++k) {
                        if (row1.coeffRef(0,i) == row2.coeffRef(0,k)) {
                            permut_vect[i] = k;
                            break;
                        }
                    }
                }
            }
        }
        return permut_vect;
    }
    return {-2};
}

void gen_johnsonTrotter_permutation(std::vector<std::pair<int,int>>&  permut) {
    std::pair<int,int> k_and_largest_mobile_elt = largest_mobile_elt(permut);
    swap_with_adjac_k_pointo_elt(k_and_largest_mobile_elt.first, permut);
    reverse_dir_all_larg_than_k(k_and_largest_mobile_elt.second, permut);
}

bool test_permutation(std::vector<std::pair<int,int>>&  permut, Eigen::SparseMatrix<double>& row1, Eigen::SparseMatrix<double>& row2_corr1) {
    int numEqualities = 0;
    for (int i = 0; i < row1.size(); ++i) {
        if (row2_corr1.coeffRef(0,permut[i].first) != row1.coeffRef(0,i)) {
            break;
        }
        numEqualities = numEqualities + 1;
    }
    if (numEqualities == row1.size()) {
        return true;
    }
    return false;
}

std::pair<bool, std::vector<int>> is_same_permut_for_elts(Eigen::SparseMatrix<double> row1,Eigen::SparseMatrix<double> row2_corr1,
                                                                        Eigen::SparseMatrix<double> row3,Eigen::SparseMatrix<double> row4_corr3) {
    std::vector<int> permut_vect1 = get_permutation(row1,row2_corr1);
    std::vector<int> permut_vect2 = get_permutation(row3,row4_corr3);

    if (permut_vect1[0] != -2 && permut_vect2[0] != -2) {

        for (unsigned i=0; i<permut_vect1.size(); ++i) {
            if (permut_vect1[i] != -1 && permut_vect2[i] != -1 && permut_vect1[i] != permut_vect2[i]) {

                std::vector<int> noPermut;
                return std::make_pair(false,noPermut);
            }
        }
    }
    std::vector<int> noPermut;
    return std::make_pair(true,noPermut);
}

// Created by Admin on 2025-10-08.
