
#include <iostream>
#include <vector>
#include <chrono>
#include "permutation.h"
#include "matrix_tool.h"
#include "algo.h"
#include "printing_tool.h"
#include <cstdlib> // For rand() and srand()
#include <ctime>
#include <fstream>




int main () {

    /*

    srand(time(00));



    int num_lines(7), num_cols(7), maxNonZeros(10);
    double minCoef(1.0), maxCoef(20.0);
    std::vector<std::vector<std::string>> list_set_inst = {{"synthetic/","synth", "synth_npz/"}};
    std::vector<std::string> list_type_inst = {"eq", "not_eq"};
    int num_insts(10);
    int maxNumModifiedCoefs(4);

    gen_random_synthetic_insts(num_lines, num_cols, maxNonZeros,   minCoef, maxCoef,
                                        maxNumModifiedCoefs, list_set_inst, list_type_inst, num_insts);

    std::cout << "Ici " <<  std::endl;

    std::string set_web("synthetic/"), type_inst("eq"),  set_inst("synth"), set_inst_test("synth_npz/"), inst("synth_1");

    auto [fst, snd,
          lowerBoundFst, lowerBoundSnd,
          upperBoundFst, upperBoundSnd] = get_sparse_matrice(set_web,type_inst,set_inst, set_inst_test, inst);

    std::cout << "A = " <<  fst << std::endl;

    std::cout << "B = " <<  snd << std::endl;

    std::cout << "lowerBoundA = " <<  lowerBoundFst << std::endl;

    std::cout << "lowerBoundB = " <<  lowerBoundSnd << std::endl;
    */

    std::vector<int> permutationRows = {0, 31, 15, 16, 13, 20, 11, 24, 8, 26, 30, 6, 12, 4, 19, 2, 3, 18, 22, 14, 5, 21, 17, 23, 7, 25, 9, 27, 28, 29, 10, 1, 32, 33};
    bool succesPermutRows(false);
    int numRows = 34;
    // Get correspondances between same sets of coefficients of rows of A and  B
    Algo_equal algo;
    std::string set_web, type_inst,  set_inst, set_inst_test, inst;
    set_web = "netlib/"; type_inst = "eq";  set_inst = "lp"; set_inst_test = "lp_npz2/"; inst = "afiro";
    auto [matrixA, matrixB,
      lowerBoundsA, lowerBoundsB,
      upperBoundsA, upperBoundsB ]  = get_sparse_matrice(set_web, type_inst,set_inst, set_inst_test, inst);
    std::vector<std::vector<int>> corrRowA = algo.get_set_indexes_corr(true, numRows, matrixA, matrixB);

    print_matrix(corrRowA, "corrRowA");


    std::vector<std::vector<int>> FailRows;
    for (int i = 0; i < numRows; ++i) {
        std::vector<int> uncompatibleRowsWith_i;
        FailRows.push_back(uncompatibleRowsWith_i);
    }
    initialize_failing_set(FailRows, corrRowA);
    std::vector<std::vector<std::pair<int,int>>> incom_couples_rows;
    bool new_permut_rows = gen_good_permutation(permutationRows, succesPermutRows, numRows, FailRows, incom_couples_rows);


    std::cout << "new_permut_rows = " << new_permut_rows << std::endl;
    print_vector(permutationRows, "permutationRows");
    std::vector<int> permutationRowsJ = {0, 31, 15, 16, 13, 20, 11, 24, 8, 26, 30, 6, 12, 4, 19, 2, 3, 22, 18, 14, 5, 21, 17, 23, 7, 25, 9, 27, 28, 29, 10, 1, 32, 33};
    //                                   0, 31, 15, 16, 13, 20, 11, 24, 8, 26, 30, 6, 12, 4, 19, 2, 3, 18, 22, 14, 5, 21, 17, 23, 7, 25, 9, 27, 28, 29, 10, 1, 32, 33
    //                                   0, 31, 15, 16, 13, 20, 11, 26, 22, 24, 30, 6, 12, 4, 19, 2, 18, 3, 8, 14, 5, 21, 17, 23, 7, 25, 9, 27, 28, 29, 10, 1, 32, 33
    //                                   0, 31, 15, 16, 13, 20, 11, 26, 24, 22, 30, 6, 12, 4, 19, 2, 18, 8, 3, 14, 5, 21, 17, 23, 7, 25, 9, 27, 28, 29, 10, 1, 32, 33
    if (permutationRows.size() == 34 && vectors_are_equal(permutationRowsJ,permutationRows)) {
        std::cout << "permutationRows generated in learned_code000 = "  << std::endl;

    }

    return 0;
}
