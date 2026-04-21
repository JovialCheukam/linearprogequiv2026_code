//
// Created by Admin on 2025-10-14.
//

#include "algo_enhanced.h"
#include "matrix_tool.h"
#include "permutation.h"
#include <iostream>
#include <cmath>

#include "printing_tool.h"

Algo_equal::Algo_equal(/* args */) {
}

Algo_equal::~Algo_equal() {
}


bool Algo_equal::chek_equal_set(bool row, Eigen::SparseMatrix<double> sortedVec1, Eigen::SparseMatrix<double> sortedVec2) {
    // Ensure that the two vectors have the same size
    assert(sortedVec1.cols() == sortedVec2.cols());
    assert(sortedVec1.rows() == sortedVec2.rows());

    // Ensure the vector is in compressed mode for efficient iteration
    sortedVec1.makeCompressed();
    sortedVec2.makeCompressed();

    if (row) {
        for (int k = 0; k < sortedVec1.cols(); ++k) {
            if (sortedVec1.coeffRef(0,k) != sortedVec2.coeffRef(0,k)) return false;
        }
    }
    else {
        for (int k = 0; k < sortedVec1.rows(); ++k) {
            if (sortedVec1.coeffRef(k, 0) != sortedVec2.coeffRef(k,0)) return false;
        }
    }
    return true;
}

std::vector<std::vector<int>> Algo_equal::get_set_indexes_corr_cols_by_bounds_matrices(int numCols, Eigen::SparseMatrix<double>& matrixBoundsA,
                                                          Eigen::SparseMatrix<double>& matrixBoundsB) {

    std::vector<std::vector<int>> corrColA = initialize_struct(numCols);
    corrColA.at(numCols-1).push_back(numCols-1);

    for (unsigned i=0; i<numCols-1; i++) {

        for (unsigned j=0; j<numCols-1; j++) {
            if (matrixBoundsA.coeffRef(i,numCols-1) == matrixBoundsB.coeffRef(j,numCols-1)) corrColA.at(i).push_back(j) ;
        }
    }

   return corrColA;
}

std::vector<std::vector<int>> Algo_equal::get_set_indexes_corr(bool row, int numRowsCols, Eigen::SparseMatrix<double>& matrixA,
                                                          Eigen::SparseMatrix<double>& matrixB) {

    Eigen::SparseMatrix<double> sortedMatrixA, sortedMatrixB;



    sortedMatrixA = generate_sorted_rows_cols_based_value_sparse_matrix(row, matrixA);
    sortedMatrixB = generate_sorted_rows_cols_based_value_sparse_matrix(row, matrixB);

    // Print the sorted sparse matrix
     //std::cout << "sortA = \n" << sortedMatrixA << std::endl;
     //std::cout << "sortB = \n" << sortedMatrixB << std::endl;

    std::vector<std::vector<int>> corrRowColA = initialize_struct(numRowsCols);// choose this data structure to have flexibility on components sizes

    int startIndex, lastIndex;
    if (row) {
        corrRowColA.at(0).push_back(0);
        startIndex = 1;
        lastIndex = 0;
    }
    if (!row) {
        corrRowColA.at(numRowsCols-1).push_back(numRowsCols-1);
        startIndex = 0;
        lastIndex = 1;
    }

    for (unsigned i=startIndex; i<numRowsCols-lastIndex; i++) {

        for (unsigned j=startIndex; j<numRowsCols-lastIndex; j++) {

            bool equal ;
            Eigen::SparseMatrix<double> sortedVec1 = sparse_row_col(row, sortedMatrixA,i);
            Eigen::SparseMatrix<double> sortedVec2 = sparse_row_col(row, sortedMatrixB,j);

            equal = chek_equal_set(row, sortedVec1, sortedVec2);

            if (equal) corrRowColA.at(i).push_back(j) ;
        }
    }
    return corrRowColA;
}

bool a_couple_is_inside(std::pair<int,int> pair, std::vector<std::pair<int,int>>& list_of_pairs) {

    for (int i = 1; i < list_of_pairs.size(); ++i) {
        if (pair.first == list_of_pairs[i].first && pair.second == list_of_pairs[i].second) return true;
    }
    return false;
}

bool learn_wrong_scaling(std::vector<std::vector<int>>& proportCoefs,
                                                            Eigen::SparseMatrix<double>& matrixA,Eigen::SparseMatrix<double>& matrixB,
                                                            std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash>& learned_wrong_scaling) {
    int num_lines = proportCoefs.size();

    bool is_same_permutation(true);
    // normalize the matrices A and B as equal by permutations of
    // lines and columns using the cross products property of proportions

    for (int i = 0; i < num_lines-1; ++i) {
        Eigen::SparseMatrix<double> row1 = sparse_row_col(true,matrixA,proportCoefs[i][3]) * proportCoefs[i][1];
        Eigen::SparseMatrix<double> row2_corr1 = sparse_row_col(true,matrixB,proportCoefs[i][2]) * proportCoefs[i][0];
        //print_matrix(proportCoefs, "proportCoefs");
        //std::cout << "rowA1 = " << proportCoefs[i][3] << " rowB1 = " << proportCoefs[i][2] << std::endl;


        for (int j = i+1; j < num_lines; ++j) {

            // normalize the matrices A and B as equal by permutations of
            // lines and columns using the cross products property of proportions
            Eigen::SparseMatrix<double> row3 = sparse_row_col(true,matrixA,proportCoefs[j][3]) * proportCoefs[j][1];
            Eigen::SparseMatrix<double> row4_corr3 = sparse_row_col(true,matrixB,proportCoefs[j][2]) * proportCoefs[j][0];


            std::pair<bool, std::vector<int>> same_permut_i = is_same_permut_for_elts(row1,row2_corr1,row3,row4_corr3);


            if (!same_permut_i.first) {
                std::pair key = std::make_pair(proportCoefs[i][3],proportCoefs[i][2]);
                if (!learned_wrong_scaling.count(key)) {
                    learned_wrong_scaling[key].push_back(std::make_pair(proportCoefs[j][3],proportCoefs[j][2]));
                }
                else {
                    if (!a_couple_is_inside({proportCoefs[j][3],proportCoefs[j][2]},learned_wrong_scaling[key])) {
                        learned_wrong_scaling[key].push_back(std::make_pair(proportCoefs[j][3],proportCoefs[j][2]));
                    }
                }
                is_same_permutation = false;
            }
        }
    }
    return is_same_permutation;
}



Algo_enhanced::Algo_enhanced(/* args */){}

Algo_enhanced::~Algo_enhanced(){}


std::tuple<bool,
    Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
    std::string> Algo_enhanced::enhanced_check_equiv_between_asso_lp_matrices_learning(bool luby, const std::string& algoType, Eigen::SparseMatrix<double>& matrixA,Eigen::SparseMatrix<double>& matrixB,
                                                                                                            Eigen::SparseMatrix<double>& matrixLowerBoundsA, Eigen::SparseMatrix<double>& matrixLowerBoundsB,
                                                                                                            Eigen::SparseMatrix<double>& matrixUpperBoundsA, Eigen::SparseMatrix<double>& matrixUpperBoundsB,
                                                                                                                std::chrono::microseconds& timeout,
                                                                                                                std::vector<std::vector<int>>&  FailRows,
                                                                                                                std::vector<std::vector<int>>&  FailCols,
                                                                                                                std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash>& learned_wrong_row_corr,
                                                                                                                std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols,
                                                                                                                int& number_of_failed_perfect_matching) {


    //std::cout << "enter of enhanced algo" << std::endl;
    // initiate the time compter
    auto start = std::chrono::high_resolution_clock::now();

    Algo_equal algo;
    int numRows = matrixA.rows();

    // initialize the first permutation of rows
    std::vector<int> permutationRows;
    for (int i = 0; i < numRows; ++i) {
        permutationRows.push_back(i);
    }

    // instanciate the recorder of the coefs of proportionality
    std::vector<std::vector<int>> proportCoefs;
    for (int i = 0; i < numRows; ++i) {
        proportCoefs.push_back(std::vector<int>(4,0));
    }

    int numCols = matrixA.cols();


    bool succesPermutRows(true);

    int u(0);


    std::vector<int> permut_empty;

    int number_of_failed_couple_permut(0);
    bool restart(false);

    if (!luby) {

        Algo_equal algo;
        int numRows = matrixA.rows();

        int numCols = matrixA.cols();

        // Get correspondances between same sets of coefficients of columns of A and  B
        std::vector<std::vector<int>> corrColA0 = algo.get_set_indexes_corr_cols_by_bounds_matrices(numCols, matrixLowerBoundsA, matrixLowerBoundsB);
        std::vector<std::vector<int>> corrColA1 = algo.get_set_indexes_corr_cols_by_bounds_matrices(numCols, matrixUpperBoundsA, matrixUpperBoundsB);
        std::vector<std::vector<int>> corrColA = algo.get_set_indexes_corr(false, numCols, matrixA, matrixB);

        for (int i = 0; i < numCols; ++i) {
            std::vector<size_t> corrCols = {corrColA0[i].size(),  corrColA1[i].size(), corrColA[i].size()};
            int min_ind =  get_min_index(corrCols);
            std::vector<std::vector<int>> corrColsSet = {corrColA0[i],  corrColA1[i], corrColA[i]};
            corrColA[i] = corrColsSet[min_ind];
        }

        std::vector<std::vector<int>> corrRowA = algo.get_set_indexes_corr(true, numRows, matrixA, matrixB);


        for (int i = 0; i < numRows; ++i) {
            std::vector<int> uncompatibleRowsWith_i;
            FailRows.push_back(uncompatibleRowsWith_i);
        }
        initialize_failing_set(FailRows, corrRowA);


        for (int i = 0; i < numCols; ++i) {
            std::vector<int> uncompatibleColsWith_i;
            FailCols.push_back(uncompatibleColsWith_i);
        }

        initialize_failing_set(FailCols, corrColA);
        //print_matrix(FailCols, "FailCols");

        // instanciate the recorder of learned wrong scaling
        std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_row_corr;

        std::vector<std::tuple<int,int,int,int>>  incom_couples_rows_cols;

    }

    while (true) {

        bool new_permut_rows  =  gen_good_permutation_without_wrong_learned_permutation0(permutationRows,  succesPermutRows, numRows,FailRows,
                                            learned_wrong_row_corr,number_of_failed_couple_permut,number_of_failed_perfect_matching, restart);

        if (!luby) restart = false;
        if (restart) {
            Eigen::SparseMatrix<double> nullMatrix;
            return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "unknown-due-to-restart");
        }

        succesPermutRows = false;

        u = u + 1;

        if (!new_permut_rows) { //std::cout << "available rows permutations finish" << std::endl;
            break;
        }

        for (int lA = 0; lA < numRows; ++lA) {
            int lB = permutationRows[lA];
                proportCoefs[lB][0] = 1;
                proportCoefs[lB][1] = 1;
                proportCoefs[lB][2] = lB;
                proportCoefs[lB][3] = lA;

        }

        /////////////////////////////////////////////////////////////////////////////////////////////////
        if (algoType == "algo_enhanced") {
            learn_wrong_scaling(proportCoefs,matrixA,matrixB,learned_wrong_row_corr);
        }

        std::vector<int> permutationCols;
        // initialize the first permutation of cols

        for (int i = 0; i < numCols; ++i) {
            permutationCols.push_back(i);
        }

        bool succesPermutCols(true);

        while (true) {

            bool new_permut_cols = gen_good_permutation0(permutationCols, succesPermutCols, numCols, FailCols,permutationRows,
                            incom_couples_rows_cols,number_of_failed_couple_permut,number_of_failed_perfect_matching, restart);

            if (!luby) restart = false;
            if (restart) {
                Eigen::SparseMatrix<double> nullMatrix;
                return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "unknown-due-to-restart");
            }

            succesPermutCols = false;

            if (!new_permut_cols) { //std::cout << "available columns permutations finish" << std::endl;
                break;
            }

            int numOfEqualities = 0;
            bool stopNextInnerForLoop = false;
            for (unsigned lA=0; lA<numRows; lA++) {
                for (unsigned cA=0; cA<numCols; cA++) {

                    if (matrixA.coeffRef(lA,cA) != matrixB.coeffRef(permutationRows[lA],permutationCols[cA])) {


                        if (algoType == "algo_enhanced") {
                            if (tuple_is_not_inside({lA,permutationRows[lA],cA,permutationCols[cA]},
                                                                            incom_couples_rows_cols)) incom_couples_rows_cols.emplace_back(lA,permutationRows[lA],
                                                                                                                                                    cA,permutationCols[cA]);
                        }
                        number_of_failed_couple_permut ++;

                        if (number_of_failed_couple_permut == number_of_failed_perfect_matching) restart = true;

                        if (!luby) restart = false;
                        if (restart) {
                            Eigen::SparseMatrix<double> nullMatrix;
                            return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "unknown-due-to-restart");
                        }

                    stopNextInnerForLoop = true;
                    break;
                    }
                    numOfEqualities = numOfEqualities + 1;
                }
                if (stopNextInnerForLoop) {
                    break;
                }
            }
            if (numOfEqualities == numRows * numCols) {


                std::vector<Eigen::Triplet<double>> tripletListRows;
                for (int lA = 0; lA < numRows; ++lA) {
                    tripletListRows.push_back(Eigen::Triplet<double>(permutationRows[lA], lA,1));
                }
                std::vector<Eigen::Triplet<double>> tripletListCols;
                for (int cA = 0; cA < numCols; ++cA) {
                    tripletListCols.push_back(Eigen::Triplet<double>(cA, permutationCols[cA], 1));

                }
                Eigen::SparseMatrix<double> permutMatrixRows(numRows,numRows);
                Eigen::SparseMatrix<double> permutMatrixCols(numCols,numCols);

                permutMatrixRows.setFromTriplets(tripletListRows.begin(), tripletListRows.end());
                permutMatrixRows.makeCompressed();
                permutMatrixCols.setFromTriplets(tripletListCols.begin(), tripletListCols.end());
                permutMatrixCols.makeCompressed();

                Eigen::SparseMatrix<double> nullMatrix;
                std::tuple<bool,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
                            Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,std::string> result;
                result = std::make_tuple(true,permutMatrixRows,permutMatrixCols,
                     nullMatrix, nullMatrix,"solved-without-timeout");

                std::cout << "Equivalence found ." << std::endl << std::endl;
                return result;
            }

            // Record ending time
            auto end = std::chrono::high_resolution_clock::now();

            // Compute the duration in microseconds
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            if (timeout < duration) {
                Eigen::SparseMatrix<double> nullMatrix;
                return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "unknown-due-to-timeout");
            }
        }

    }

    Eigen::SparseMatrix<double> nullMatrix;
    return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "solved-without-timeout");
}

void update_number_failed_couple_perfects_matching_strategy(int intial_num_failed, int& iteration, std::vector<int>& time_sequence, int& number_failed_perfects_matching) {


        if (log2(iteration + 1) == std::floor(log2(iteration + 1))) {

            number_failed_perfects_matching = intial_num_failed * std::pow(2, std::floor(log2(iteration + 1))-1);
            time_sequence.push_back(number_failed_perfects_matching);

        }
        else {

            number_failed_perfects_matching = time_sequence[iteration-std::pow(2,std::ceil(log2(iteration + 1))-1)];
            time_sequence.push_back(number_failed_perfects_matching);

        }

}

std::tuple<Eigen::SparseMatrix<double>,
std::vector<std::pair<int, int>>,std::vector<std::pair<int, int>>> shuffle_matrix(int& iteration, Eigen::SparseMatrix<double>& matrixA) {

    std::vector<std::pair<int, int>> pairsExchangIndexesRows, pairsExchangIndexesCols;

    if (iteration == 1) return std::make_tuple(matrixA,pairsExchangIndexesRows,pairsExchangIndexesCols);

    int numRows = matrixA.rows();

    int numCols = matrixA.cols();

    pairsExchangIndexesRows = gen_pairs_exchang_indexes(numRows, 0);
    pairsExchangIndexesCols = gen_pairs_exchang_indexes(numCols, numCols - 1);

    Eigen::SparseMatrix<double> shuffledRowsA =  permut_rows_or_cols_matrix(true, matrixA, pairsExchangIndexesRows);
    Eigen::SparseMatrix<double> shuffledMatrixA =  permut_rows_or_cols_matrix(false, shuffledRowsA, pairsExchangIndexesCols);
    return std::make_tuple(shuffledMatrixA, pairsExchangIndexesRows,pairsExchangIndexesCols)  ;
}

void  update_fail_rows_or_columns( std::vector<std::pair<int, int>>& pairsExchangIndexesRowsA,
                                   std::vector<std::pair<int, int>>& pairsExchangIndexesRowsB,
                                   std::vector<std::vector<int>>& FailRows) {
    std::vector<std::vector<int>> FailRowsCop = FailRows;

    for (int i = 0; i < pairsExchangIndexesRowsA.size(); ++i) {
        for (int k = 0; k < FailRowsCop.size(); ++k) {
            if (pairsExchangIndexesRowsA[i].first == k) {
                int j = pairsExchangIndexesRowsA[i].second;
                FailRows[j] = FailRowsCop[k];
                FailRows[k] = FailRowsCop[j];
            }
            else {
                if (pairsExchangIndexesRowsA[i].second == k) {
                    int j = pairsExchangIndexesRowsA[i].first;
                    FailRows[j] = FailRowsCop[k];
                    FailRows[k] = FailRowsCop[j];
                }
            }
        }
    }

    for (int i = 0; i < pairsExchangIndexesRowsB.size(); ++i) {
        for (int k = 0; k < FailRows.size(); ++k) {
            for (int l = 0; l < FailRows[k].size(); ++l) {
                if (pairsExchangIndexesRowsB[i].first == FailRows[k][l]) {
                    int j = pairsExchangIndexesRowsB[i].second;
                    FailRows[k][l] = j;
                }
                else {
                    if (pairsExchangIndexesRowsB[i].second == FailRows[k][l]) {
                        int j = pairsExchangIndexesRowsB[i].first;
                        FailRows[k][l] = j;
                    }
                }
            }
        }
    }
}

void update_learned_wrong_row_corr(std::vector<std::pair<int, int>>& pairsExchangIndexesRowsA,
                                   std::vector<std::pair<int, int>>& pairsExchangIndexesRowsB,
                                   std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash>& learned_wrong_row_corr) {


    std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_row_corrCp;

    for (const auto& pairs_rows : learned_wrong_row_corr) {

        // Update the key pairs_rows.first
        std::pair<int,int> new_key_pair;

        bool first_index_new_key_pair_is_updated = false ;
        bool second_index_new_key_pair_is_updated = false ;

        for (int i = 0; i < pairsExchangIndexesRowsA.size(); ++i) {

            // Update the first component of the key pairs_rows.first
            if (pairsExchangIndexesRowsA[i].first == pairs_rows.first.first) {

                new_key_pair.first = pairsExchangIndexesRowsA[i].second;
                first_index_new_key_pair_is_updated = true ;
                break;

            }
            if (pairsExchangIndexesRowsA[i].second == pairs_rows.first.first) {

                new_key_pair.first = pairsExchangIndexesRowsA[i].first;
                first_index_new_key_pair_is_updated = true ;
                break;
            }
        }
        if (!first_index_new_key_pair_is_updated) new_key_pair.first = pairs_rows.first.first;
        for (int i = 0; i < pairsExchangIndexesRowsB.size(); ++i) {
            // Update the first component of the key pairs_rows.first
            if (pairsExchangIndexesRowsB[i].first == pairs_rows.first.second) {

                new_key_pair.second = pairsExchangIndexesRowsB[i].second;
                second_index_new_key_pair_is_updated = true ;
                break;

            }
            if (pairsExchangIndexesRowsB[i].second == pairs_rows.first.second) {

                new_key_pair.second = pairsExchangIndexesRowsB[i].first;
                second_index_new_key_pair_is_updated = true ;
                break;
            }
        }
        if (!second_index_new_key_pair_is_updated) new_key_pair.second = pairs_rows.first.second;
        std::vector<std::pair<int,int>> pairsIndex;

        learned_wrong_row_corrCp[new_key_pair] = pairsIndex ;

        // Update the value pairs_rows.second
        for (int j = 0; j < pairs_rows.second.size(); ++j) {

            // Update the value pairs_rows.second[j]
            std::pair<int,int> new_val_pair = pairs_rows.second[j];

            bool first_index_new_val_pair_is_updated = false ;
            bool second_index_new_val_pair_is_updated = false ;

            for (int i = 0; i < pairsExchangIndexesRowsA.size(); ++i) {

                // Update the first component of the value pairs_rows.second[j]
                if (pairsExchangIndexesRowsA[i].first == pairs_rows.second[j].first) {

                    new_val_pair.first = pairsExchangIndexesRowsA[i].second;
                    first_index_new_val_pair_is_updated = true ;
                    break;
                }
                if (pairsExchangIndexesRowsA[i].second == pairs_rows.second[j].first) {

                    new_val_pair.first = pairsExchangIndexesRowsA[i].first;
                    first_index_new_val_pair_is_updated = true ;
                    break;
                }
            }
            if (!first_index_new_val_pair_is_updated) new_val_pair.first = pairs_rows.second[j].first ;
            for (int i = 0; i < pairsExchangIndexesRowsB.size(); ++i) {

                // Update the first component of the value pairs_rows.second[j]
                if (pairsExchangIndexesRowsB[i].first == pairs_rows.second[j].second) {

                    new_val_pair.second = pairsExchangIndexesRowsB[i].second;
                    second_index_new_val_pair_is_updated = true ;
                    break;
                }
                if (pairsExchangIndexesRowsB[i].second == pairs_rows.second[j].second) {

                    new_val_pair.second = pairsExchangIndexesRowsB[i].first;
                    second_index_new_val_pair_is_updated = true ;
                    break;
                }
            }
            if (!second_index_new_val_pair_is_updated) new_val_pair.second = pairs_rows.second[j].second ;
            learned_wrong_row_corrCp[new_key_pair].push_back(new_val_pair);
        }
    }

    learned_wrong_row_corr = std::move(learned_wrong_row_corrCp);

}

void update_incom_couples_rows_cols(std::vector<std::pair<int, int>>& pairsExchangIndexesRowsA,
                     std::vector<std::pair<int, int>>& pairsExchangIndexesColsA,
                     std::vector<std::pair<int, int>>& pairsExchangIndexesRowsB,
                     std::vector<std::pair<int, int>>& pairsExchangIndexesColsB,
                     std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols) {


    for (int i = 0; i < incom_couples_rows_cols.size(); ++i) {
        for (int j = 0; j < pairsExchangIndexesRowsA.size(); ++j) {

            if (pairsExchangIndexesRowsA[j].first == std::get<0>(incom_couples_rows_cols[i])) {
                std::get<0>(incom_couples_rows_cols[i]) = pairsExchangIndexesRowsA[j].second;
            }
            else {
                if (pairsExchangIndexesRowsA[j].second == std::get<0>(incom_couples_rows_cols[i])) {
                    std::get<0>(incom_couples_rows_cols[i]) = pairsExchangIndexesRowsA[j].first;
                }
            }
        }
        for (int j = 0; j < pairsExchangIndexesRowsB.size(); ++j) {

            if (pairsExchangIndexesRowsB[j].first == std::get<1>(incom_couples_rows_cols[i])) {
                std::get<1>(incom_couples_rows_cols[i]) = pairsExchangIndexesRowsB[j].second;
            }
            else {
                if (pairsExchangIndexesRowsB[j].second == std::get<1>(incom_couples_rows_cols[i])) {
                    std::get<1>(incom_couples_rows_cols[i]) = pairsExchangIndexesRowsB[j].first;
                }
            }
        }
        for (int j = 0; j < pairsExchangIndexesColsA.size(); ++j) {

            if (pairsExchangIndexesColsA[j].first == std::get<2>(incom_couples_rows_cols[i])) {
                std::get<2>(incom_couples_rows_cols[i]) = pairsExchangIndexesColsA[j].second;
            }
            else {
                if (pairsExchangIndexesColsA[j].second == std::get<2>(incom_couples_rows_cols[i])) {
                    std::get<2>(incom_couples_rows_cols[i]) = pairsExchangIndexesColsA[j].first;
                }
            }
        }
        for (int j = 0; j < pairsExchangIndexesColsB.size(); ++j) {

            if (pairsExchangIndexesColsB[j].first == std::get<3>(incom_couples_rows_cols[i])) {
                std::get<3>(incom_couples_rows_cols[i]) = pairsExchangIndexesColsB[j].second;
            }
            else {
                if (pairsExchangIndexesColsB[j].second == std::get<3>(incom_couples_rows_cols[i])) {
                    std::get<3>(incom_couples_rows_cols[i]) = pairsExchangIndexesColsB[j].first;
                }
            }
        }
    }
}

std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> update_learning(std::vector<std::pair<int, int>>& pairsExchangIndexesRowsA,
                     std::vector<std::pair<int, int>>& pairsExchangIndexesColsA,
                     std::vector<std::pair<int, int>>& pairsExchangIndexesRowsB,
                     std::vector<std::pair<int, int>>& pairsExchangIndexesColsB,
                     std::vector<std::vector<int>>& FailRows,
                     std::vector<std::vector<int>>& FailCols,
                     std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash>& learned_wrong_row_corr,
                     std::vector<std::tuple<int,int,int,int>>& incom_couples_rows_cols) {

    update_fail_rows_or_columns( pairsExchangIndexesRowsA,pairsExchangIndexesRowsB,FailRows);

    update_fail_rows_or_columns( pairsExchangIndexesColsA,pairsExchangIndexesColsB,FailCols);

    //std::cout << "finish of update_fail_rows_or_columns 2 " << std::endl;

    update_learned_wrong_row_corr(pairsExchangIndexesRowsA,pairsExchangIndexesRowsB,learned_wrong_row_corr);

    //std::cout << "finish of update_learned_wrong_row_corr  " << std::endl;

    update_incom_couples_rows_cols(pairsExchangIndexesRowsA,pairsExchangIndexesColsA,
                                   pairsExchangIndexesRowsB,pairsExchangIndexesColsB,
                                   incom_couples_rows_cols);

    //std::cout << "finish of update_incom_couples_rows_cols " << std::endl;

return learned_wrong_row_corr;
}



std::tuple<bool,
    Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
std::string> Algo_enhanced::luby_learning(int intial_num_failed, const std::string& algoType, Eigen::SparseMatrix<double>& matrixA,Eigen::SparseMatrix<double>& matrixB,
                                                    Eigen::SparseMatrix<double>& matrixLowerBoundsA, Eigen::SparseMatrix<double>& matrixLowerBoundsB,
                                                    Eigen::SparseMatrix<double>& matrixUpperBoundsA, Eigen::SparseMatrix<double>& matrixUpperBoundsB,
                                                                                                            std::chrono::microseconds timeout) {

    std::cout << "running of luby Algorithm" << std::endl;
    // initiate the time compter
    auto start = std::chrono::high_resolution_clock::now();

    Algo_equal algo;
    int numRows = matrixA.rows();

    int numCols = matrixA.cols();

    // Get correspondances between same sets of coefficients of columns of A and  B
    std::vector<std::vector<int>> corrColA0 = algo.get_set_indexes_corr_cols_by_bounds_matrices(numCols, matrixLowerBoundsA, matrixLowerBoundsB);
    std::vector<std::vector<int>> corrColA1 = algo.get_set_indexes_corr_cols_by_bounds_matrices(numCols, matrixUpperBoundsA, matrixUpperBoundsB);
    std::vector<std::vector<int>> corrColA = algo.get_set_indexes_corr(false, numCols, matrixA, matrixB);

    for (int i = 0; i < numCols; ++i) {
        std::vector<size_t> corrCols = {corrColA0[i].size(),  corrColA1[i].size(), corrColA[i].size()};
        int min_ind =  get_min_index(corrCols);
        std::vector<std::vector<int>> corrColsSet = {corrColA0[i],  corrColA1[i], corrColA[i]};
        corrColA[i] = corrColsSet[min_ind];
    }

    std::vector<std::vector<int>> corrRowA = algo.get_set_indexes_corr(true, numRows, matrixA, matrixB);

    std::vector<std::vector<int>> FailRows;
    for (int i = 0; i < numRows; ++i) {
        std::vector<int> uncompatibleRowsWith_i;
        FailRows.push_back(uncompatibleRowsWith_i);
    }
    initialize_failing_set(FailRows, corrRowA);

    std::vector<std::vector<int>> FailCols;
    for (int i = 0; i < numCols; ++i) {
        std::vector<int> uncompatibleColsWith_i;
        FailCols.push_back(uncompatibleColsWith_i);
    }

    initialize_failing_set(FailCols, corrColA);

    // instanciate the recorder of learned wrong scaling
    std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_row_corr;

    std::vector<std::tuple<int,int,int,int>>  incom_couples_rows_cols;


    for (const auto & i : corrRowA) {
        if (i.empty()) {
            Eigen::SparseMatrix<double> nullMatrix;
            return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "solved-without-timeout");
        }
    }

    for (const auto & i : corrColA) {
        if (i.empty()) {
            Eigen::SparseMatrix<double> nullMatrix;
            return std::make_tuple(false, nullMatrix,nullMatrix,nullMatrix,nullMatrix, "solved-without-timeout");
        }
    }

    // Record ending time
    auto end = std::chrono::high_resolution_clock::now();

    // Compute the duration in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


    std::tuple<bool,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
                    Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,std::string> result;
    std::get<0>(result) = false;

    Eigen::SparseMatrix<double> shuffledMatrixA = matrixA;
    Eigen::SparseMatrix<double> shuffledMatrixB = matrixB;

    // Initialize the luby algo
    int number_failed_perfects_matching(0);
    int iteration(1);
    std::vector<int> time_sequence;

    std::tuple<Eigen::SparseMatrix<double>,
    std::vector<std::pair<int, int>>,std::vector<std::pair<int, int>>> shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsA;

    std::tuple<Eigen::SparseMatrix<double>,
    std::vector<std::pair<int, int>>,std::vector<std::pair<int, int>>> shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsB;


    while (timeout >= duration && !(std::get<0>(result))) {

        update_number_failed_couple_perfects_matching_strategy(intial_num_failed, iteration, time_sequence, number_failed_perfects_matching);

        std::string iteration_string ;
        if (iteration == 1) iteration_string = "Intialization" ;
        else iteration_string = "Restart ";

        if (iteration % 20000 == 0) {
        std::cout << iteration_string << iteration-1 << ", " << "number_failed_perfects_matching = " << number_failed_perfects_matching <<"----------------------------" <<  std::endl;
        }

        if (iteration > 1) {

            shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsA = shuffle_matrix(iteration,shuffledMatrixA);
            shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsB = shuffle_matrix(iteration,shuffledMatrixB);

            shuffledMatrixA = std::get<0>(shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsA);

            std::vector<std::pair<int, int>> pairsExchangIndexesRowsA = std::get<1>(shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsA);
            std::vector<std::pair<int, int>> pairsExchangIndexesColsA = std::get<2>(shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsA);

            shuffledMatrixB = std::get<0>(shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsB);

            std::vector<std::pair<int, int>> pairsExchangIndexesRowsB = std::get<1>(shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsB);
            std::vector<std::pair<int, int>> pairsExchangIndexesColsB = std::get<2>(shuffledMatrix_pairsExchangIndexesRows_pairsExchangIndexesColsB);


            update_learning(pairsExchangIndexesRowsA,pairsExchangIndexesColsA,
                            pairsExchangIndexesRowsB,pairsExchangIndexesColsB,
                                                        FailRows,FailCols,
                                                        learned_wrong_row_corr,
                                                        incom_couples_rows_cols);

        }

        result = enhanced_check_equiv_between_asso_lp_matrices_learning(true, algoType, shuffledMatrixA,shuffledMatrixB,
                                                                                  matrixLowerBoundsA, matrixLowerBoundsB,
                                                                         matrixUpperBoundsA, matrixUpperBoundsB,
                                                                                  timeout,
                                                                                  FailRows,FailCols,
                                                                                learned_wrong_row_corr,
                                                                                incom_couples_rows_cols,
                                                                                number_failed_perfects_matching);
        iteration ++;

        // Record ending time
        auto end = std::chrono::high_resolution_clock::now();
        // Compute the duration in microseconds
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }
    return result;
}
