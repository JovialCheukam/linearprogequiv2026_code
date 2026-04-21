#include "matrix_tool.h"
#include <iostream>
#include "printing_tool.h"
#include <fstream>
#include <sstream>
#include <string>
#include "permutation.h"
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>





Eigen::SparseMatrix<double> gen_matrix_with_given_non_zeros(const int rows, const int cols, const std::vector<Eigen::Triplet<double>>& setOfSetOfTriplets) {

    std::vector<Eigen::Triplet<double>> tripletList;
    tripletList.reserve(std::max(rows, cols)); // Reserve space for an estimated number of non-zero elements

    for (int i = 0; i < setOfSetOfTriplets.size(); i++) {
        tripletList.push_back(setOfSetOfTriplets[i]);
    }

    Eigen::SparseMatrix<double> sparseMatrix(rows, cols);
    sparseMatrix.setFromTriplets(tripletList.begin(), tripletList.end());
    sparseMatrix.makeCompressed();

    return sparseMatrix;
}

Eigen::SparseMatrix<double> randomly_generate_sparse_matrix(int numRows, int numCols) {

    // Define an empty sparse matrix
    Eigen::SparseMatrix<double> mat(numRows, numCols);

    // Reserve space for non-zero elts
    mat.reserve(Eigen::VectorXi::Constant(numCols, 3));

    // generate a random number of non-zero coeff of the sparse matrice
    int numNonZero = 4 + (rand() % numRows * numCols - 3) ;

    // generate the container of the non-zeros elements of mat
    std::vector<Eigen::Triplet<double>> tripletList;
    tripletList.reserve(numNonZero); // Pre-allocate for expected number of non-zeros


    for (int i = 0; i < numNonZero; i++) {

        int x = 0 + (rand() % numRows) ;
        int y = 0 + (rand() % numCols) ;
        double z = 1.0 + rand() % numRows * numCols;
        tripletList.push_back(Eigen::Triplet(x, y, z));

    }
    mat.setFromTriplets(tripletList.begin(), tripletList.end());

    // Compress the matrix to optimize storage and performance
    mat.makeCompressed();
    return mat;
}

Eigen::SparseMatrix<double> generate_sorted_rows_cols_based_value_sparse_matrix(bool row, Eigen::SparseMatrix<double> & matrix) {
    const int numRows = matrix.rows();
    const int numCols = matrix.cols();

    // Define an empty sparse matrix
    Eigen::SparseMatrix<double> mat(numRows, numCols);

    // Compress the matrix (important for accurate nonZeros() count)
    matrix.makeCompressed();

    int numNonZeros, numRowsOrCols;
    if (row) {
        // Compute the average number of non-zeros elts per column of matrix
        numNonZeros = matrix.nonZeros() / numCols;
        numRowsOrCols = numRows;
        // Reserve space for non-zero elts
        //mat.reserve(Eigen::VectorXi::Constant(numCols, numNonZeros));
    }
    else  {
        // Compute the average number of non-zeros elts per row of matrix
        numNonZeros = matrix.nonZeros() / numRows;
        numRowsOrCols = numCols;
        // Reserve space for non-zero elts
        //mat.reserve(Eigen::VectorXi::Constant(numRows, numNonZeros));
    }

    // generate the container of the non-zeros elements of mat
    std::vector<Eigen::Triplet<double>> tripletList;
    tripletList.reserve(numNonZeros); // Pre-allocate for expected number of non-zeros

    // generate the container of the non-zeros elements of mat
    std::vector<Eigen::Triplet<double>> nonZerosTripletsToChek;

    for (int k = 0; k < matrix.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
            if (it.value() != 0) nonZerosTripletsToChek.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
        }
    }


    // Extract non-zero elements of the desired row and put them in an empty triplets list
    for (int i = 0; i < numRowsOrCols; i++) {

        std::vector<double> rowOrColElements;
        for (int j = 0; j < nonZerosTripletsToChek.size(); j++) {
            if (row) {
                if (i == nonZerosTripletsToChek[j].row()) rowOrColElements.push_back(nonZerosTripletsToChek[j].value());
            }
            else {
                if (i == nonZerosTripletsToChek[j].col()) rowOrColElements.push_back(nonZerosTripletsToChek[j].value());
            }
        }
        //std::cout << "ici1" << std::endl;
        // std::cout << "row " << i << " "; print_vector1(rowOrColElements, "rowOrColElements");

        // Sort the extracted elements based on their values
        std::ranges::sort(rowOrColElements,
                          [](const double a, const double b) {
                              return a < b; // Sort by value in ascending order
                          });

        //std::cout << "ici2" << std::endl;
        // Put the sorted row in the tripletList
        int sizeRowOrColElements = rowOrColElements.size();
        for (int j0 = 0; j0 < sizeRowOrColElements; j0++) {
            if (row) {
                tripletList.push_back(Eigen::Triplet(i, j0 + (numCols - sizeRowOrColElements), rowOrColElements[j0]));
            }
            else tripletList.push_back(Eigen::Triplet(j0 + (numRows - sizeRowOrColElements), i, rowOrColElements[j0]));
        }
    }
    //std::cout << "ici3" << std::endl;
    mat.setFromTriplets(tripletList.begin(), tripletList.end());

    // Compress the matrix to optimize storage and performance
    mat.makeCompressed();
    return mat;
}

Eigen::SparseMatrix<double> sparse_row_col(bool row, Eigen::SparseMatrix<double> & matrix, int rank) {

    // generate the container of the non-zeros elements of matrix
    std::vector<Eigen::Triplet<double>> nonZerosTripletsToChek;

    for (int k = 0; k < matrix.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
            if (it.value() != 0) nonZerosTripletsToChek.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
        }
    }
    // generate the container of the non-zeros elements of the sparse vector
    std::vector<Eigen::Triplet<double>> tripletList;
    for (int i = 0; i < nonZerosTripletsToChek.size(); i++) {
        if (row) {
            if (rank == nonZerosTripletsToChek[i].row())  tripletList.push_back(Eigen::Triplet(0,nonZerosTripletsToChek[i].col(), nonZerosTripletsToChek[i].value()));
        }
        else {
            if (rank == nonZerosTripletsToChek[i].col())  tripletList.push_back(Eigen::Triplet(nonZerosTripletsToChek[i].row(),0, nonZerosTripletsToChek[i].value()));
        }

    }

    Eigen::SparseMatrix<double> sparseVec;
    if (row) {
        sparseVec = Eigen::SparseMatrix<double>(1,matrix.cols());
    }
    else {
        sparseVec = Eigen::SparseMatrix<double>(matrix.rows(), 1);
    }

    sparseVec.setFromTriplets(tripletList.begin(), tripletList.end());

    return sparseVec;
}

Eigen::Triplet<double> exchange_rows_or_col(bool row, std::vector<std::pair<int, int>> pairsExchangIndexes, Eigen::Triplet<double> triplet) {

        //std::cout << triplet.row()<< " " << triplet.col()<< " " << triplet.value() << std::endl;

    for (int i = 0; i < pairsExchangIndexes.size(); i++) {
        if (row) {

            if (pairsExchangIndexes[i].first == triplet.row()) return Eigen::Triplet(pairsExchangIndexes[i].second,triplet.col(),triplet.value());

            if (pairsExchangIndexes[i].second == triplet.row()) return Eigen::Triplet(pairsExchangIndexes[i].first,triplet.col(),triplet.value());

        }
        else {
            if (pairsExchangIndexes[i].first == triplet.col()) return Eigen::Triplet(triplet.row(),pairsExchangIndexes[i].second,triplet.value());
            if (pairsExchangIndexes[i].second == triplet.col()) return Eigen::Triplet(triplet.row(),pairsExchangIndexes[i].first,triplet.value());
        }
    }
    return triplet;
}

std::vector<std::pair<int, int>> gen_pairs_exchang_indexes(int numRowsOrCols, int notPermutedRowOrCol) {

    std::vector<std::pair<int, int>> pairsExchangIndexes;


    int sizePairsExchangIndexes(1);

    for (int i = 0; i < numRowsOrCols; i++) {
        int x = 0 + (rand() % numRowsOrCols) ;
        int y = 0 + (rand() % numRowsOrCols) ;
        if (x != notPermutedRowOrCol && y != notPermutedRowOrCol) {
            int numDifferences(0);
            for (int j = 0; j < sizePairsExchangIndexes; j++) {

                if (pairsExchangIndexes.size() > 0) {
                    if (x == pairsExchangIndexes[j].first || x == pairsExchangIndexes[j].second || y == pairsExchangIndexes[j].first || y == pairsExchangIndexes[j].second) {
                        break;
                    }
                    numDifferences ++;
                }
            }

            if (numDifferences == pairsExchangIndexes.size() && x != y) {
                //std::cout << "(x, y)  = " <<  x  << " , " << y << std::endl;
                pairsExchangIndexes.push_back(std::pair(x, y));
                sizePairsExchangIndexes = pairsExchangIndexes.size();
            }
        }
    }
    if (pairsExchangIndexes.empty()) pairsExchangIndexes.push_back(std::pair(0, 0));
    return pairsExchangIndexes;
}

Eigen::SparseMatrix<double> permut_rows_or_cols_matrix(bool row, Eigen::SparseMatrix<double> & matrix, const std::vector<std::pair<int, int>>& pairsExchangIndexes) {

    const int numRows = matrix.rows();
    const int numCols = matrix.cols();

    // Define an empty sparse matrix
    Eigen::SparseMatrix<double> mat(numRows, numCols);

    // Compress the matrix (important for accurate nonZeros() count)
    matrix.makeCompressed();

    int numNonZeros;
    if (row) {
        // Compute the average number of non-zeros elts per column of matrix
        numNonZeros = matrix.nonZeros() / numCols;
    }
    else  {
        // Compute the average number of non-zeros elts per row of matrix
        numNonZeros = matrix.nonZeros() / numRows;
    }

    //std::cout << "uuuuuu" << std::endl;
    // Reserve space for non-zero elts
    mat.reserve(Eigen::VectorXi::Constant(numCols, numNonZeros));



    // generate the container of the non-zeros elements of mat
    std::vector<Eigen::Triplet<double>> tripletList;
    tripletList.reserve(numNonZeros); // Pre-allocate for expected number of non-zeros

    // generate the container of the non-zeros elements of matrix
    std::vector<Eigen::Triplet<double>> nonZerosTripletsToChek;

    for (int k = 0; k < matrix.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
            if (it.value() != 0) nonZerosTripletsToChek.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
        }
    }
    //std::cout << "uuuuuu11111" << std::endl;
    //print_triplet_list(nonZerosTripletsToChek, "nonZerosTripletsToChek");

    for (int k = 0; k < nonZerosTripletsToChek.size(); ++k) {
        Eigen::Triplet<double> triplet = exchange_rows_or_col(row, pairsExchangIndexes, nonZerosTripletsToChek[k]);
        tripletList.push_back(triplet);
    }

    mat.setFromTriplets(tripletList.begin(), tripletList.end());

    //std::cout << "uuuuuu33333" << std::endl;

    // Compress the matrix to optimize storage and performance
    mat.makeCompressed();
    return mat;
}

std::vector<Eigen::Triplet<double>> set_of_non_zero_coefs(int numRows, int numCols, int maxNonZeros, double minCoef, double maxCoef) {
    std::vector<Eigen::Triplet<double>> setOfNonZeroCoefs;
    for (int i = 0; i < maxNonZeros; i++) {

        int x = 0 + (rand() % numRows) ;
        int y = 0 + (rand() % numCols) ;
        double coef = minCoef + ((double)rand() / RAND_MAX) * (maxCoef - minCoef);
        Eigen::Triplet<double> triplet0(x, y, std::round(coef * 100.0)/100.0);
        setOfNonZeroCoefs.push_back(triplet0);
        if (x + 1 <= numRows-1) {
            Eigen::Triplet<double> triplet1(x+1, y, std::round(coef * 100.0)/100.0);
            setOfNonZeroCoefs.push_back(triplet1);
        }
        if (y + 1 <= numCols-1) {
            Eigen::Triplet<double> triplet2(x, y+1, std::round(coef * 100.0)/100.0);
            setOfNonZeroCoefs.push_back(triplet2);
        }
    }
    return setOfNonZeroCoefs;
}

std::vector<Eigen::Triplet<double>> set_of_non_zero_coefs_for_bounds_mat(int numCols,  double minCoef, double maxCoef, double id) {
    std::vector<Eigen::Triplet<double>> setOfNonZeroCoefs;

    for (int i = 0; i < numCols-1; i++) {

        double coef = minCoef + ((double)rand() / RAND_MAX) * (maxCoef - minCoef);

        Eigen::Triplet<double> triplet1(i, i, id);
        Eigen::Triplet<double> triplet2;
        if (id == 1.0) {
            triplet2 = Eigen::Triplet<double>(i, numCols-1, std::round(coef * 100.0)/100.0);
        }
        else { triplet2 = Eigen::Triplet<double>(i, numCols-1, -std::round(coef * 100.0)/100.0); }
        setOfNonZeroCoefs.push_back(triplet1);
        setOfNonZeroCoefs.push_back(triplet2);
    }
    return setOfNonZeroCoefs;
}



std::tuple<Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
            Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
            Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>> gen_random_matrices_A_B(const int numRows, const int numCols,
                                                                                            const int maxNonZeros, const int maxNumModifiedCoefs,
                                                                                            const double minCoef, const double maxCoef, const std::string& type_inst) {

    std::vector<std::pair<int, int>> pairsExchangIndexesRows = gen_pairs_exchang_indexes(numRows, 0);
    std::vector<std::pair<int, int>> pairsExchangIndexesCols = gen_pairs_exchang_indexes(numCols, numCols - 1);
    print_pair(pairsExchangIndexesRows, "pairsExchangIndexesRows");
    print_pair(pairsExchangIndexesCols, "pairsExchangIndexesCols");

    //std::vector<Eigen::Triplet<double>> setOfNonZeroCoefs = set_of_non_zero_coefs(numRows, numCols, maxNonZeros, minCoef, maxCoef);
    //std::vector<Eigen::Triplet<double>> setOfNonZeroCoefsLowerBounds = set_of_non_zero_coefs_for_bounds_mat(numCols,  minCoef, maxCoef, 1.0);
    //std::vector<Eigen::Triplet<double>> setOfNonZeroCoefsUpperBounds = set_of_non_zero_coefs_for_bounds_mat(numCols,  minCoef, maxCoef, -1.0);



    std::vector<Eigen::Triplet<double>> setOfNonZeroCoefs = {Eigen::Triplet(0, 2, 1.0),
                                                             Eigen::Triplet(0, 3, 5.0),
                                                             Eigen::Triplet(0, 4, 5.0),
                                                             Eigen::Triplet(0, 6, 0.0),
                                                             Eigen::Triplet(1, 2, 1.0),
                                                             Eigen::Triplet(1, 4, 5.0),
                                                             Eigen::Triplet(1, 5, 5.0),
                                                             Eigen::Triplet(2, 2, 5.0),
                                                             Eigen::Triplet(2, 3, 5.0),
                                                             Eigen::Triplet(2, 5, 1.0),
                                                             Eigen::Triplet(3, 0, 5.0),
                                                             Eigen::Triplet(3, 1, 5.0),
                                                             Eigen::Triplet(3, 5, 1.0),
                                                             Eigen::Triplet(4, 0, 5.0),
                                                             Eigen::Triplet(4, 1, 5.0),
                                                             Eigen::Triplet(4, 4, 1.0),
                                                             Eigen::Triplet(5, 3, 1.0),
                                                             Eigen::Triplet(5, 6, 1.0),
                                                             Eigen::Triplet(6, 0, 1.0),
                                                             Eigen::Triplet(6, 1, 1.0),
    };

    std::vector<Eigen::Triplet<double>> setOfNonZeroCoefsLowerBounds = {Eigen::Triplet(0, 0, 1.0),
                                                             Eigen::Triplet(0, 6, 3.0),
                                                             Eigen::Triplet(1, 1, 1.0),
                                                             Eigen::Triplet(1, 6, 2.0),
                                                             Eigen::Triplet(2, 2, 1.0),
                                                             Eigen::Triplet(2, 6, 3.0),
                                                             Eigen::Triplet(3, 3, 1.0),
                                                             Eigen::Triplet(3, 6, 2.0),
                                                             Eigen::Triplet(4, 4, 1.0),
                                                             Eigen::Triplet(4, 6, 2.0),
                                                             Eigen::Triplet(5, 5, 1.0),
                                                             Eigen::Triplet(5, 6, 2.0)
    };

    std::vector<Eigen::Triplet<double>> setOfNonZeroCoefsUpperBounds = {Eigen::Triplet(0, 0, -1.0),
                                                             Eigen::Triplet(0, 6, -3.0),
                                                             Eigen::Triplet(1, 1, -1.0),
                                                             Eigen::Triplet(1, 6, -2.0),
                                                             Eigen::Triplet(2, 2, -1.0),
                                                             Eigen::Triplet(2, 6, -3.0),
                                                             Eigen::Triplet(3, 3, -1.0),
                                                             Eigen::Triplet(3, 6, -2.0),
                                                             Eigen::Triplet(4, 4, -1.0),
                                                             Eigen::Triplet(4, 6, -2.0),
                                                             Eigen::Triplet(5, 5, -1.0),
                                                             Eigen::Triplet(5, 6, -2.0)
    };
    Eigen::SparseMatrix<double> A  = gen_matrix_with_given_non_zeros(numRows, numCols, setOfNonZeroCoefs);
    Eigen::SparseMatrix<double> permutRowA =  permut_rows_or_cols_matrix(true, A, pairsExchangIndexesRows);
    Eigen::SparseMatrix<double> B =  permut_rows_or_cols_matrix(false, permutRowA, pairsExchangIndexesCols);

    Eigen::SparseMatrix<double> lowerBoundsA  = gen_matrix_with_given_non_zeros(numCols-1, numCols, setOfNonZeroCoefsLowerBounds);
    Eigen::SparseMatrix<double> upperBoundsA  = gen_matrix_with_given_non_zeros(numCols-1, numCols, setOfNonZeroCoefsUpperBounds);

    //std::cout << "iiiiiiiiioo" <<  std::endl;
    //print_pair(pairsExchangIndexesCols, "pairsExchangIndexesCols");
    Eigen::SparseMatrix<double> permutRowLowerBoundsA =  permut_rows_or_cols_matrix(true, lowerBoundsA, pairsExchangIndexesCols);
    Eigen::SparseMatrix<double> permutRowUpperBoundsA =  permut_rows_or_cols_matrix(true, upperBoundsA, pairsExchangIndexesCols);

    //std::cout << "iiiiiiiiiooo" <<  std::endl;
    Eigen::SparseMatrix<double> lowerBoundsB =  permut_rows_or_cols_matrix(false, permutRowLowerBoundsA, pairsExchangIndexesCols);
    Eigen::SparseMatrix<double> upperBoundsB =  permut_rows_or_cols_matrix(false, permutRowUpperBoundsA, pairsExchangIndexesCols);

    //std::cout << "iiiiiiiiioooo" <<  std::endl;



    if (type_inst == "not_eq" || type_inst == "not_eq_pro") {
        int numModifedCoefs = 1 + (rand() % maxNumModifiedCoefs) ;
        for (int k = 0; k < numModifedCoefs; k++) {
            int x = 0 + (rand() % numRows) ;
            int y = 0 + (rand() % numCols) ;
            double coef = minCoef + ((double)rand() / RAND_MAX) * (maxCoef - minCoef);
            B.coeffRef(x,y) = B.coeffRef(x,y) + coef;
        }
    }
    return std::make_tuple(A, B, lowerBoundsA, lowerBoundsB, upperBoundsA, upperBoundsB);
}

std::vector<std::vector<int>>  initialize_struct(int numRowsCols) {

    std::vector<std::vector<int>> matrix ;

    for (unsigned i=0; i<numRowsCols; i++)
    {
        matrix.push_back({}) ;
    }
    return matrix;
}

std::vector<std::pair<int,int>> get_one_seq_corr(int k, std::vector<std::tuple<int, int, int>> tree) {
    int father = k;
    int child = k;
    int u = 0;
    std::vector<std::pair<int,int>> seq_corr;
    while (father != -1) {
        u = 0;
        for (unsigned i=0; i<tree.size(); ++i) {
            if (child == std::get<1>(tree[i])) {
                child = std::get<0>(tree[i]);
                father = std::get<0>(tree[i]);
                seq_corr.push_back(std::make_pair(std::get<1>(tree[i]),std::get<2>(tree[i])));
                break;
            }
            u = u + 1;
        }
        if (u == tree.size()) {
            std::vector<std::pair<int,int>> seq_corr_empty;
            return seq_corr_empty;
        }
    }
    return seq_corr;
}

bool pair_is_not_inside(std::pair<int,int> pair, std::vector<std::pair<int,int>> incom_couples_rows) {

    for (int i = 0; i < incom_couples_rows.size(); i++) {
        if (pair.first == incom_couples_rows[i].first && pair.second == incom_couples_rows[i].second) {
            return false;
        }
    }
    return true;
}

bool tuple_is_not_inside(std::tuple<int,int,int,int> tuple, std::vector<std::tuple<int,int,int,int>> incom_couples_rows_cols) {

    int i0 = std::get<0>(tuple);
    int p_i0 = std::get<1>(tuple);
    int j0 = std::get<2>(tuple);
    int q_j0 = std::get<3>(tuple);


    for (int k = 0; k < incom_couples_rows_cols.size(); k++) {

        int i = std::get<0>(incom_couples_rows_cols[k]);
        int p_i = std::get<1>(incom_couples_rows_cols[k]);
        int j = std::get<2>(incom_couples_rows_cols[k]);
        int q_j = std::get<3>(incom_couples_rows_cols[k]);
        if (i0 == i && j0 == j && p_i0 == p_i && q_j0 == q_j) {
            return false;
        }
    }
    return true;
}

int get_min_index(std::vector<size_t> vector) {

    auto minElementIterator = std::min_element(vector.begin(), vector.end());

    // Calculate the index by finding the distance from the beginning
    int indexOfMin = std::distance(vector.begin(), minElementIterator);

    return indexOfMin;
}


bool set_is_not_inside(std::vector<std::pair<int,int>> set_of_incom_couples_rows, std::vector<std::vector<std::pair<int,int>>> incom_couples_rows) {

    if (incom_couples_rows.empty()) return true;

    std::ranges::sort(set_of_incom_couples_rows,
                          [](const std::pair<int,int> a, const std::pair<int,int> b) {
                              return a < b; // Sort by value in ascending order
                          });
    for (int i=0; i < incom_couples_rows.size(); i++ ) {
        std::ranges::sort(incom_couples_rows[i],
                          [](const std::pair<int,int> a, const std::pair<int,int> b) {
                              return a < b; // Sort by value in ascending order
                          });
        if (set_of_incom_couples_rows.size() != incom_couples_rows[i].size()) return true;
        for (int j=0; j < set_of_incom_couples_rows.size(); j++) {
                if (set_of_incom_couples_rows[j] != incom_couples_rows[i][j]) return true;
        }
    }
    return false;
}

bool is_not_inside(std::tuple<int, int, int> tuple, std::vector<std::pair<int,int>> &  seq_corr) {
    bool father (false), child (true), name(true);
    for (unsigned i=0; i<seq_corr.size(); i++) {
        if (std::get<0>(tuple) == seq_corr[i].first ) {
            father = true;
            break;
        }
    }
    for (unsigned i=0; i<seq_corr.size(); i++) {
        if (std::get<1>(tuple) == seq_corr[i].first ) {
            child = false;
            break;
        }
    }
    for (unsigned i=0; i<seq_corr.size(); i++) {
        if (std::get<2>(tuple) == seq_corr[i].second ) {
            name = false;
            break;
        }
    }

    return father && child && name;
}

void make_children(std::vector<std::tuple<int, int, int>> lastBranches, std::vector<std::tuple<int, int, int>>& tree,
     std::vector<std::vector<int>> currCorrRowColA, int & lastTreeNode) {

    print_matrix(currCorrRowColA, "currCorrRowColA");
    //return;

    if (currCorrRowColA.size() == 0) return;


    std::vector<std::tuple<int, int, int>> currentBranches;
    for (unsigned i=0; i<lastBranches.size(); i++) {

        for (unsigned j=0; j<currCorrRowColA[0].size(); j++) {
            lastTreeNode = lastTreeNode + 1;
            std::tuple<int, int, int> nextNode = std::make_tuple(std::get<1>(lastBranches[i]), lastTreeNode, currCorrRowColA[0][j]);

            // Generate the path semiSeqCorr from the root to the next father to be assigned corrDistinctRowCols[i-1][l]
            std::vector<std::pair<int,int>> semiSeqCorr = get_one_seq_corr(std::get<1>(lastBranches[i]),  tree);

            // The new node is added to the tree iff there is not already a node on the path semiSeqCorr with the same name
            if (is_not_inside(nextNode, semiSeqCorr)) {

                tree.push_back(nextNode);
                currentBranches.push_back(nextNode);
            }
        }
    }
    std::vector<std::vector<int>> nextCorrRowColA;
    for (unsigned k=1; k<currCorrRowColA.size(); k++) {
        nextCorrRowColA.push_back(currCorrRowColA[k]);
    }
    //std::cout << "i = " << i << std::endl;
    //print_matrix(nextCorrRowColA, "nextCorrRowColA");

    make_children(currentBranches, tree, nextCorrRowColA, lastTreeNode);
}

std::vector<std::tuple<int, int, int>> gen_tree_of_corr(std::vector<std::vector<int>> corrRowColA) {

    std::vector<std::tuple<int, int, int>> tree;
    std::vector<std::tuple<int, int, int>> lastBranches;
    int lastTreeNode = 0;

    print_matrix(corrRowColA, "corrRowColA");


    for (unsigned j=0; j<corrRowColA[0].size(); ++j) {

        // Assign the foot -1 as the father of the nodes inside corrDistinctRowCols[0]
        tree.push_back(std::make_tuple(-1,lastTreeNode,corrRowColA[0][j]));
        lastBranches.push_back(std::make_tuple(-1,lastTreeNode,corrRowColA[0][j]));
        lastTreeNode = lastTreeNode + 1;
    }
    std::vector<std::vector<int>> currCorrRowColA;
    for (unsigned k=1; k<corrRowColA.size(); k++) {
        currCorrRowColA.push_back(corrRowColA[k]);
    }
    make_children(lastBranches, tree, currCorrRowColA, lastTreeNode);
    return tree;
}


std::vector<std::vector<std::tuple<int, int, int>>> get_children_per_node(std::vector<std::tuple<int, int, int>> tree) {

    std::vector<std::vector<std::tuple<int, int, int>>> allChildren;
    int sizeAllChildren = std::get<1>(tree.back()) + 1; // 1 is added to prevent the stopping criteria of

    for (unsigned i=0; i<=sizeAllChildren; ++i) {
        std::vector<std::tuple<int, int, int>> childrenPerNode;

        allChildren.push_back(childrenPerNode);
    }

    // Sort the nodes based on their father (the first component of the tuple node)
    std::sort(tree.begin(), tree.end(),
              [](const std::tuple<int, int, int> a, const std::tuple<int, int, int> b) {
                  return std::get<0>(a) < std::get<0>(b); // Sort by first component in ascending order
              });


    int k = 0;

    for (int i=-1; i<sizeAllChildren; ++i) {

        for (int j=k; j<tree.size(); ++j) {

            if (std::get<0>(tree[j]) == i) allChildren[i+1].push_back(tree[j]);

            if (std::get<0>(tree[j]) > i) {
                k = j;
                break;
            }
        }
    }
    return allChildren;
}

std::vector<int> get_branches_for_root(int node, int numNodes, std::vector<std::vector<std::tuple<int, int, int>>> & allChildren,
                                                                                                   std::vector<int>  branch,
                                                                                                   std::vector<std::vector<int>> & seqCorrList) {
    //std::cout << "node = " << node  << std::endl;

    if (allChildren[node + 1].empty()) return branch;

    for (unsigned i=0; i<allChildren[node + 1].size(); ++i) {

        //std::cout << "node + 1 = " << node + 1 << " i = " << i << std::endl;

        int child = std::get<1>(allChildren[node + 1][i]);
        int childName = std::get<2>(allChildren[node + 1][i]);
        std::vector<int>  branchCurrent;
        for (unsigned j=0; j<branch.size(); ++j) {
            branchCurrent.push_back(branch[j]);
        }
        branchCurrent.push_back(childName);
        std::vector<int> branch_i = get_branches_for_root(child, numNodes, allChildren, branchCurrent, seqCorrList);

        //std::cout << i << " " ; print_vector(branch_i, "branch_i");

        if (allChildren[child + 1].empty()) seqCorrList.push_back(branch_i);
    }
    return branch;
}

std::vector<std::vector<int>> depth_first_search(std::vector<std::vector<std::tuple<int, int, int>>> allChildren) {

    int numNodes = 0;
    for (int i=0; i<allChildren.size(); ++i) {
        numNodes = numNodes + allChildren[i].size();
    }
    std::vector<int> branch;
    std::vector<std::vector<int>> seqCorrList;
    get_branches_for_root(-1, numNodes, allChildren, branch, seqCorrList);
    return seqCorrList;
}

void build_permutation_matrix(std::vector<Eigen::Triplet<double>>& tripletList, Eigen::Triplet<double> triplet) {

    for (int i=0; i<tripletList.size(); ++i) {

        if (tripletList[i].row() == triplet.row() && tripletList[i].col() == triplet.col()) {
            return;
        }
    }
    tripletList.push_back(triplet);
}

void initialize_failing_set(std::vector<std::vector<int>>& Fail, std::vector<std::vector<int>>& corrRowColA) {

    for (int i = 0; i < corrRowColA.size(); ++i) {

        for (int j = 0; j < corrRowColA.size(); ++j) {

            if (!is_inside(j, corrRowColA[i])) {
                //std::cout << "i = " << i << std::endl;
                //print_vector(corrRowColA[i], "corrRowColA[i]");
                Fail[i].push_back(j);
            }
        }
    }
}

std::vector<Eigen::Triplet<double>> process_line_of_non_zero_string(std::string lineToProcess, int position) {

    std::string line = "";
    for (int i=position; i<lineToProcess.length()-1; ++i) {
        line =  line + lineToProcess[i];
    }

    std::stringstream ss(line);
    std::string segment;
    std::vector<std::string> segments;

    while (std::getline(ss, segment, '|')) { // Extract segments using getline with the delimiter '|'
        segments.push_back(segment);
    }

    std::vector<Eigen::Triplet<double>> tripletList;
    for (const std::string& nonZero : segments) {
        std::stringstream ssNonZero(nonZero);
        std::string rowColVal;
        std::vector<std::string> row_col_val;
        while (std::getline(ssNonZero, rowColVal, ',')) { // Extract segments using getline with the delimiter ','
            row_col_val.push_back(rowColVal);
        }
        std::string row = "";
        for (int i=1; i<row_col_val[0].length(); ++i) {
            row = row + row_col_val[0][i];
        }

        std::string col = row_col_val[1];

        std::string value = "";
        for (int i=0; i<row_col_val[2].length()-1; ++i) {
            value = value + row_col_val[2][i];
        }

        tripletList.push_back(Eigen::Triplet<double>(std::stoi(row),std::stoi(col),std::stod(value)));
    }
    return tripletList;
}

std::string get_home_dir() {
    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        struct passwd *pw = getpwuid(getuid());
        if (pw != NULL) {
            homedir = pw->pw_dir;
        }
    }

    if (homedir != NULL) {
        return std::string(homedir);
    } else {
        // Handle error: home directory not found
        return "";
    }
}

std::tuple<Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
           Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
           Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>> get_sparse_matrice(const std::string& set_web, const std::string& type_inst,
                                                                                       const std::string& set_inst,const std::string& set_inst_test,
                                                                                       const std::string& inst) {

    //std::string directory = "/Users/admin/Documents/CLionProjects/EquivLinProg/matrices_data/";
    //std::string directory = "/home/jovial/scratch/CLionProjects/EquivLinProg/matrices_data/";
    std::string directory = get_home_dir() + "/equivalence_linear_program_algo/EquivLinProg/matrices_data/";


    std:: string file =  directory + set_web + set_inst + "/" + set_inst_test + "matrices_" + type_inst + "/" + inst + ".txt";


    std::ifstream inputFile(file);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " + file << std::endl;
    }

    std::string line;
    std::string num_rows = "";
    std::string num_cols = "";
    std::getline(inputFile, line) ;
    for (int i=11; i<line.length(); ++i) {
         num_rows = num_rows + line[i];
    }
    std::cout << "num_rows = " << num_rows << std::endl;
    std::getline(inputFile, line) ;
    for (int i=11; i<line.length(); ++i) {
        num_cols = num_cols + line[i];
    }
    std::cout << "num_cols = " << num_cols << std::endl;



    std::getline(inputFile, line) ;
    std::vector<Eigen::Triplet<double>> tripletListA = process_line_of_non_zero_string(line,15);

    std::getline(inputFile, line) ;
    std::vector<Eigen::Triplet<double>> tripletListB = process_line_of_non_zero_string(line,15);

    std::getline(inputFile, line) ;
    std::vector<Eigen::Triplet<double>> tripletListLowerBoundsA = process_line_of_non_zero_string(line,26);

    std::getline(inputFile, line) ;
    std::vector<Eigen::Triplet<double>> tripletListLowerBoundsB = process_line_of_non_zero_string(line,26);

    std::getline(inputFile, line) ;
    std::vector<Eigen::Triplet<double>> tripletListUpperBoundsA = process_line_of_non_zero_string(line,26);

    std::getline(inputFile, line) ;
    std::vector<Eigen::Triplet<double>> tripletListUpperBoundsB = process_line_of_non_zero_string(line,26);

    inputFile.close();

    Eigen::SparseMatrix<double> sparseMatrixA(std::stoi(num_rows), std::stoi(num_cols));
    sparseMatrixA.setFromTriplets(tripletListA.begin(), tripletListA.end());
    sparseMatrixA.makeCompressed();

    Eigen::SparseMatrix<double> sparseMatrixB(std::stoi(num_rows), std::stoi(num_cols));
    sparseMatrixB.setFromTriplets(tripletListB.begin(), tripletListB.end());
    sparseMatrixB.makeCompressed();

    Eigen::SparseMatrix<double> sparseMatrixLowerBoundsA(std::stoi(num_cols)-1, std::stoi(num_cols));
    sparseMatrixLowerBoundsA.setFromTriplets(tripletListLowerBoundsA.begin(), tripletListLowerBoundsA.end());
    sparseMatrixLowerBoundsA.makeCompressed();

    Eigen::SparseMatrix<double> sparseMatrixLowerBoundsB(std::stoi(num_cols)-1, std::stoi(num_cols));
    sparseMatrixLowerBoundsB.setFromTriplets(tripletListLowerBoundsB.begin(), tripletListLowerBoundsB.end());
    sparseMatrixLowerBoundsB.makeCompressed();

    Eigen::SparseMatrix<double> sparseMatrixUpperBoundsA(std::stoi(num_cols)-1, std::stoi(num_cols));
    sparseMatrixUpperBoundsA.setFromTriplets(tripletListUpperBoundsA.begin(), tripletListUpperBoundsA.end());
    sparseMatrixUpperBoundsA.makeCompressed();

    Eigen::SparseMatrix<double> sparseMatrixUpperBoundsB(std::stoi(num_cols)-1, std::stoi(num_cols));
    sparseMatrixUpperBoundsB.setFromTriplets(tripletListUpperBoundsB.begin(), tripletListUpperBoundsB.end());
    sparseMatrixUpperBoundsB.makeCompressed();


    return std::make_tuple(sparseMatrixA,sparseMatrixB,sparseMatrixLowerBoundsA,sparseMatrixLowerBoundsB,
                                                                sparseMatrixUpperBoundsA,sparseMatrixUpperBoundsB);

}

void write_non_zeros(std::ofstream & file_, const std::vector<Eigen::Triplet<double>> & non_zeros_A) {
    for (int i = 0 ; i < non_zeros_A.size() ; i++) {
        file_ << "(" + std::to_string(non_zeros_A[i].row()) + ","  + std::to_string(non_zeros_A[i].col()) + ","  + std::to_string(non_zeros_A[i].value()) + ")";
        if (i != non_zeros_A.size() - 1)
            file_ << "|";
    }
}

void save_matrices_inst(const int& num_lines, const int& num_cols,
                  const std::vector<Eigen::Triplet<double>> & non_zeros_A, const std::vector<Eigen::Triplet<double>> & non_zeros_B,
                  const std::vector<Eigen::Triplet<double>> & non_zeros_lowerBoundsA, const std::vector<Eigen::Triplet<double>> & non_zeros_lowerBoundsB,
                  const std::vector<Eigen::Triplet<double>> & non_zeros_upperBoundsA, const std::vector<Eigen::Triplet<double>> & non_zeros_upperBoundsB,
                  const std::string& set_web, const std::string& type_inst,const std::string& set_inst,const std::string& set_inst_test,const std::string& inst) {

    std::string directory = get_home_dir() + "CLionProjects/EquivLinProg/matrices_data/";

    std::string file = directory + set_web + set_inst + "/" + set_inst_test + "matrices_" + type_inst + "/" + inst + ".txt";

    std::ofstream file_(file);

    if (file_.is_open()) {
        file_ << "num_rows = " + std::to_string(num_lines) + "\n";
        file_ << "num_cols = " + std::to_string(num_cols) + "\n";
        file_ << "non_zeros_A = [";
        write_non_zeros(file_,non_zeros_A);
        file_ << "]\n";
        file_ << "non_zeros_B = [";
        write_non_zeros(file_,non_zeros_B);
        file_ << "]\n";
        file_ << "non_zeros_lowerBoundsA = [";
        write_non_zeros(file_,non_zeros_lowerBoundsA);
        file_ << "]\n";
        file_ << "non_zeros_lowerBoundsB = [";
        write_non_zeros(file_,non_zeros_lowerBoundsB);
        file_ << "]\n";
        file_ << "non_zeros_upperBoundsA = [";
        write_non_zeros(file_,non_zeros_upperBoundsA);
        file_ << "]\n";
        file_ << "non_zeros_upperBoundsB = [";
        write_non_zeros(file_,non_zeros_upperBoundsB);
        file_ << "]\n";
        file_.close();
    }
    else {
        std::cerr << "Impossible d'ouvrir le fichier " << file << std::endl;
    }
}

//std::vector<std::string>::value_type operator*(const std::vector<std::vector<std::string>>::iterator::value_type & vector);

void gen_random_synthetic_insts(const int& num_lines, const int& num_cols,
                                    const int& maxNonZeros,const int& maxNumModifiedCoefs,
                    const double& minCoef, const double& maxCoef,
                    const std::vector<std::vector<std::string>>& list_set_inst, const std::vector<std::string>& list_type_inst, const int& num_insts) {

    for (int set = 0; set < list_set_inst.size(); ++set) {

        std::string set_web = list_set_inst[set][0];
        std::string set_inst = list_set_inst[set][1];
        std::string set_inst_test = list_set_inst[set][2];

        for (int inst_type = 0; inst_type < list_type_inst.size();  ++inst_type) {

            const std::string& type_inst = list_type_inst[inst_type];

            for (int i = 1; i <= num_insts; ++i) {

                std::string inst = "synth_" + std::to_string(i);

                std::tuple<Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
                           Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
                            Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>> A_B = gen_random_matrices_A_B(num_lines, num_cols,
                                                                                            maxNonZeros, maxNumModifiedCoefs,minCoef, maxCoef, type_inst);

                Eigen::SparseMatrix<double> matrixA = std::get<0>(A_B);
                Eigen::SparseMatrix<double> matrixB = std::get<1>(A_B);
                Eigen::SparseMatrix<double> lowerBoundsA = std::get<2>(A_B);
                Eigen::SparseMatrix<double> lowerBoundsB = std::get<3>(A_B);
                Eigen::SparseMatrix<double> upperBoundsA = std::get<4>(A_B);
                Eigen::SparseMatrix<double> upperBoundsB = std::get<5>(A_B);

                // This is crucial for efficient iteration using InnerIterator. It organizes the internal storage of the sparse matrix.
                matrixA.makeCompressed();
                matrixB.makeCompressed();

                std::vector<Eigen::Triplet<double>>  non_zeros_A;
                std::vector<Eigen::Triplet<double>> non_zeros_B;
                std::vector<Eigen::Triplet<double>>  non_zeros_lowerBoundsA;
                std::vector<Eigen::Triplet<double>> non_zeros_lowerBoundsB;
                std::vector<Eigen::Triplet<double>>  non_zeros_upperBoundsA;
                std::vector<Eigen::Triplet<double>> non_zeros_upperBoundsB;

                for (int k = 0; k < matrixA.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(matrixA, k); it; ++it) {
                        if (it.value() != 0) non_zeros_A.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                for (int k = 0; k < matrixB.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(matrixB, k); it; ++it) {
                        if (it.value() != 0) non_zeros_B.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                for (int k = 0; k < lowerBoundsA.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(lowerBoundsA, k); it; ++it) {
                        if (it.value() != 0) non_zeros_lowerBoundsA.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                for (int k = 0; k < lowerBoundsB.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(lowerBoundsB, k); it; ++it) {
                        if (it.value() != 0) non_zeros_lowerBoundsB.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                for (int k = 0; k < lowerBoundsA.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(upperBoundsA, k); it; ++it) {
                        if (it.value() != 0) non_zeros_upperBoundsA.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                for (int k = 0; k < lowerBoundsB.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(upperBoundsB, k); it; ++it) {
                        if (it.value() != 0) non_zeros_upperBoundsB.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                save_matrices_inst(num_lines,num_cols,non_zeros_A,non_zeros_B,non_zeros_lowerBoundsA,non_zeros_lowerBoundsB,
                                                                    non_zeros_upperBoundsA,non_zeros_upperBoundsB,
                                                       set_web,type_inst,set_inst,set_inst_test,inst);
            }
        }
    }
}

bool are_same_rows(std::vector<std::pair<int,double>> coefsRowsA,std::vector<std::pair<int,double>> coefsRowsB) {

    if (coefsRowsA.size() != coefsRowsB.size()) return false;

    for (int row = 0; row < coefsRowsA.size(); ++row) {
        if (coefsRowsA[row].second != coefsRowsB[row].second ) return false;
    }
    return true;
}

int modulo_positif(int dividende, int diviseur) {
    int reste = dividende % diviseur;
    if (reste < 0) {
        reste += diviseur;
    }
    return reste;
}


std::tuple<bool, int, std::vector<std::pair<int, int>>,
    std::vector<std::pair<int, int>>, std::vector<std::pair<int, int>>,
    std::vector<std::pair<int, int>>> identify_rows_or_cols_with_at_most_2_non_zeros(Eigen::SparseMatrix<double>& matrixA,
                                                                                            Eigen::SparseMatrix<double>& matrixB,
                                                                                                        std::vector<int>& coefsPerRowsWithAtMost2A,
                                                                                                        std::vector<int>& coefsPerRowsWithAtMost2B,
                                                                                                        std::vector<std::vector<std::pair<int,double>>>& coefsPerRowsA,
                                                                                                        std::vector<std::vector<std::pair<int,double>>>& coefsPerRowsB) {

    int  rows = matrixA.rows();
    int  cols = matrixA.cols();
    int last_row(0);
    std::vector<std::pair<int, int>> pairsExchIndRowsA,pairsExchIndRowsB;
    std::vector<std::pair<int, int>> pairsExchIndColsA,pairsExchIndColsB;

    std::vector<bool> already_assigned(coefsPerRowsWithAtMost2A.size(),false);
    std::vector<bool> assigned(coefsPerRowsWithAtMost2A.size(),false);
    bool no_possible_assignement(false);
    for (int  i = 0 ; i < coefsPerRowsWithAtMost2A.size() ; ++i) {
        if (no_possible_assignement) {

            return std::make_tuple(false, last_row, pairsExchIndRowsA, pairsExchIndRowsB, pairsExchIndColsA, pairsExchIndColsB);
        }
        int  rowA = coefsPerRowsWithAtMost2A[i];

        for (int  j = 0 ; j < coefsPerRowsWithAtMost2B.size() ; ++j) {
            int  rowB = coefsPerRowsWithAtMost2B[j];
            if (! already_assigned[j]) {

                if (are_same_rows(coefsPerRowsA[rowA],coefsPerRowsB[rowB])) {

                    already_assigned[j] = true;
                    assigned[i] = true;
                    last_row = rows - 1 - i;
                    pairsExchIndRowsA.push_back(std::make_pair<int, int>(std::move(rowA), std::move(last_row)));
                    pairsExchIndRowsB.push_back(std::make_pair<int, int>(std::move(rowB), std::move(last_row)));

                    int colA, colB;
                    if (coefsPerRowsA[rowA].size() == 1) colA = coefsPerRowsA[rowA][0].first;
                    if (coefsPerRowsA[rowA].size() == 2) {
                        if (coefsPerRowsA[rowA][0].second == matrixA.coeffRef(rowA,cols-1)) {
                            colA = coefsPerRowsA[rowA][1].first;
                        }
                        else colA = coefsPerRowsA[rowA][0].first;
                    }

                    if (coefsPerRowsB[rowB].size() == 1) colB = coefsPerRowsA[rowB][0].first;
                    if (coefsPerRowsB[rowB].size() == 2) {
                        if (coefsPerRowsB[rowB][0].second == matrixB.coeffRef(rowB,cols-1)) {
                            colB = coefsPerRowsB[rowB][1].first;
                        }
                        else colB = coefsPerRowsB[rowB][0].first;
                    }

                    pairsExchIndColsA.push_back(std::make_pair<int, int>(std::move(colA),modulo_positif(cols - 2 - i, cols - 1)));
                    pairsExchIndColsB.push_back(std::make_pair<int, int>(std::move(colB), modulo_positif(cols - 2 - i, cols - 1)));
                    break;
                }
            }
        }
        if (!assigned[i]) no_possible_assignement = true;
    }
    if (!assigned[coefsPerRowsWithAtMost2A.size()-1]) return std::make_tuple(false, last_row, pairsExchIndRowsA, pairsExchIndRowsB, pairsExchIndColsA, pairsExchIndColsB);
    return std::make_tuple(true, last_row, pairsExchIndRowsA, pairsExchIndRowsB, pairsExchIndColsA, pairsExchIndColsB);
}

Eigen::SparseMatrix<double> sub_sparse_mat(Eigen::SparseMatrix<double>& matrixA, int firstRow, int lastRow, int firstCol, int lastCol) {

    std::vector<Eigen::Triplet<double>>  nonZeros;

    for (int k = 0; k < matrixA.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(matrixA, k); it; ++it) {
            if (it.value() != 0 && it.row() >= firstRow && it.row() <= lastRow &&  it.col() >= firstCol && it.row() <= lastCol) {
                nonZeros.push_back(Eigen::Triplet<double>(it.row()-firstRow, it.col()-firstCol, it.value()));
            }
        }
    }
    Eigen::SparseMatrix<double> subMatrix(lastRow - firstRow + 1, lastCol - firstCol + 1);
    subMatrix.setFromTriplets(nonZeros.begin(), nonZeros.end());

    // Compress the matrix to optimize storage and performance
    subMatrix.makeCompressed();
    return subMatrix;
}

std::tuple<bool, Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> reduce_size_of_matrices(Eigen::SparseMatrix<double>& matrixA, Eigen::SparseMatrix<double>& matrixB) {
    std::vector<Eigen::Triplet<double>>  non_zeros_A;
    std::vector<Eigen::Triplet<double>> non_zeros_B;

    for (int k = 0; k < matrixA.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(matrixA, k); it; ++it) {
            if (it.value() != 0) non_zeros_A.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
        }
    }
    for (int k = 0; k < matrixB.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(matrixB, k); it; ++it) {
            if (it.value() != 0) non_zeros_B.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
        }
    }

    std::vector<std::vector<std::pair<int,double>>> coefsPerRowsA;
    for (int  i = 0 ; i < matrixA.rows() ; ++i) {
        std::vector<std::pair<int,double>> coefsOfCols;
        coefsPerRowsA.push_back(coefsOfCols);
    }

    for (int  i = 0 ; i < non_zeros_A.size() ; ++i) {

        int row = non_zeros_A[i].row();
        int col = non_zeros_A[i].col();
        double value = non_zeros_A[i].value();

        coefsPerRowsA[row].emplace_back(col, value);
    }

    std::vector<std::vector<std::pair<int,double>>> coefsPerRowsB;
    for (int  i = 0 ; i < matrixA.rows() ; ++i) {
        std::vector<std::pair<int,double>> coefsOfCols;
        coefsPerRowsB.push_back(coefsOfCols);
    }

    for (int  i = 0 ; i < non_zeros_B.size() ; ++i) {

        int row = non_zeros_B[i].row();
        int col = non_zeros_B[i].col();
        double value = non_zeros_B[i].value();

        coefsPerRowsB[row].emplace_back(col, value);
    }
    std::vector<int> coefsPerRowsWithAtMost2A;
    for (int  i = 1 ; i < coefsPerRowsA.size() ; ++i) {
        if (coefsPerRowsA[i].size() == 1 || (coefsPerRowsA[i].size() == 2 && matrixA.coeffRef(i,matrixA.cols()-1) != 0)) {
            coefsPerRowsWithAtMost2A.push_back(i);
        }
    }

    print_vector(coefsPerRowsWithAtMost2A, "coefsPerRowsWithAtMost2A");

    std::vector<int> coefsPerRowsWithAtMost2B;
    for (int  i = 1 ; i < coefsPerRowsB.size() ; ++i) {
        if (coefsPerRowsB[i].size() == 1 || (coefsPerRowsB[i].size() == 2 && matrixB.coeffRef(i,matrixB.cols()-1) != 0)) {
            coefsPerRowsWithAtMost2B.push_back(i);
        }
    }
    print_vector(coefsPerRowsWithAtMost2B, "coefsPerRowsWithAtMost2B");

    std::tuple<bool, int, std::vector<std::pair<int, int>>,
    std::vector<std::pair<int, int>>, std::vector<std::pair<int, int>>,
    std::vector<std::pair<int, int>>> reducePairsExchIndRows;
    reducePairsExchIndRows = identify_rows_or_cols_with_at_most_2_non_zeros(matrixA, matrixB, coefsPerRowsWithAtMost2A,coefsPerRowsWithAtMost2B,
                                                                                                    coefsPerRowsA,coefsPerRowsB);

    std::vector<std::pair<int, int>> pairsExchIndRowsA = std::get<2>(reducePairsExchIndRows);
    std::vector<std::pair<int, int>> pairsExchIndRowsB = std::get<3>(reducePairsExchIndRows);
    std::vector<std::pair<int, int>> pairsExchIndColsA = std::get<4>(reducePairsExchIndRows);
    std::vector<std::pair<int, int>> pairsExchIndColsB = std::get<5>(reducePairsExchIndRows);

    std::sort(pairsExchIndRowsA.begin(), pairsExchIndRowsA.end(),
              [](const std::pair<int, double> a, const std::pair<int, double> b) {
                  return a.first > b.first; // Sort by first component in ascending order
              });

    std::sort(pairsExchIndRowsB.begin(), pairsExchIndRowsB.end(),
              [](const std::pair<int, double> a, const std::pair<int, double> b) {
                  return a.first > b.first; // Sort by first component in ascending order
              });

    std::sort(pairsExchIndColsA.begin(), pairsExchIndColsA.end(),
              [](const std::pair<int, double> a, const std::pair<int, double> b) {
                  return a.first > b.first; // Sort by first component in ascending order
              });

    std::sort(pairsExchIndColsB.begin(), pairsExchIndColsB.end(),
              [](const std::pair<int, double> a, const std::pair<int, double> b) {
                  return a.first > b.first; // Sort by first component in ascending order
              });

    print_pair(std::get<2>(reducePairsExchIndRows), "rowsExchange");
    print_pair(std::get<4>(reducePairsExchIndRows), "colsExchange");
    Eigen::SparseMatrix<double> permutRowsMatrixA, permutRowsMatrixB, permutColsMatrixA, permutColsMatrixB;

    if (std::get<0>(reducePairsExchIndRows)) {

        std::cout << "succes of identify_rows_or_cols_with_at_most_2_non_zeros" << std::endl;

        permutRowsMatrixA = permut_rows_or_cols_matrix(true, matrixA, pairsExchIndRowsA);
        permutRowsMatrixB = permut_rows_or_cols_matrix(true, matrixB, pairsExchIndRowsB);
        permutColsMatrixA = permut_rows_or_cols_matrix(false, permutRowsMatrixA, pairsExchIndColsA);
        permutColsMatrixB = permut_rows_or_cols_matrix(false, permutRowsMatrixB, pairsExchIndColsB);

        Eigen::SparseMatrix<double> subMatrixA =  sub_sparse_mat(permutColsMatrixA, 0, std::get<1>(reducePairsExchIndRows)-1, 0, matrixA.cols()-1);
        Eigen::SparseMatrix<double> subMatrixB =  sub_sparse_mat(permutColsMatrixB, 0, std::get<1>(reducePairsExchIndRows)-1, 0, matrixA.cols()-1);
        return std::make_tuple(true,subMatrixA, subMatrixB);
    }
    Eigen::SparseMatrix<double> nullMatrix;
    return std::make_tuple(false,nullMatrix, nullMatrix);

}

std::string get_line_command(std::vector<std::string> line_vec) {

    std::string saving_cmd_file;
    for (int i = 0; i < line_vec.size(); i++) {
        saving_cmd_file +=  line_vec[i] ;
        if (i < line_vec.size() - 1) saving_cmd_file += " ";
    }
    return saving_cmd_file;
}


void record_solution_with_python(const std::string& model, const std::string& solver, const std::string& max_scale, const std::string& timeout_or_not,
    const std::string& type_inst, const std::string& set_inst,const std::string& set_inst_test,const std::string& inst,const std::string& set_web,
    const std::string& result,const std::string& time_ex , const std::string&  valid_or_not ,const std::string& num_columns , const std::string& num_lines,
    const std::string& luby_cst, const std::string& seed) {

    std::vector<std::string> line_vec;
    line_vec.push_back(model);
    line_vec.push_back(solver);
    line_vec.push_back(max_scale);
    line_vec.push_back(timeout_or_not);
    line_vec.push_back(type_inst);
    line_vec.push_back(set_inst);
    line_vec.push_back(set_inst_test);
    line_vec.push_back(inst);
    line_vec.push_back(set_web);
    line_vec.push_back(result);
    line_vec.push_back(time_ex);
    line_vec.push_back(valid_or_not);
    line_vec.push_back(num_columns);
    line_vec.push_back(num_lines);
    line_vec.push_back(luby_cst);
    line_vec.push_back(seed);

    std::string saving_cmd_file = get_line_command(line_vec);

    //std::cout << "saving_cmd_file = " << saving_cmd_file << std::endl;

    //int executed_cmd = system(("python3  /home/jovial/scratch/code_exp_postdoc_jov_/save_results_cpp.py " + saving_cmd_file).c_str());

    std::string directory = get_home_dir();
    int executed_cmd = system(("python3  " + directory + "/equivalence_linear_program_model/save_results_cpp.py " + saving_cmd_file).c_str());

    //int executed_cmd = system(("python3  /Users/admin/Documents/POSTDOC_MichealMorin/CODE_PostDoc/env_code_postdoc_jovial/for_fir/code_exp_postdoc_jov_/save_results_cpp.py " + saving_cmd_file).c_str());


    if (executed_cmd == 0) {
        std::cout << "The saving of results is successfull" << std::endl;
    }
    else std::cout << "The saving of results failed" << std::endl;
}


// Created by Admin on 2025-10-08.
