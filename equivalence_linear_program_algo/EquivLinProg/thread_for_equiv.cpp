#include <iostream>
#include "solutions.h"
#include "matrix_tool.h"
#include <vector>
#include "algo_enhanced.h"
#include <chrono>



std::tuple<bool,
std::string, std::vector<double>, std::vector<double>,
std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>,
Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> test_equiv_lp_one_thread(bool luby, const std::string& seed, int intial_num_failed,
                                                                                    const std::string& algoType,
                                                                                    const std::string& set_web,
                                                                                    const std::string& type_inst,const std::string& set_inst,
                                                                                    const std::string& set_inst_test, const std::string& inst,
                                                                                    std::chrono::microseconds timeout) {


    //set_web = "netlib/"; type_inst = "eq";  set_inst = "lp"; set_inst_test = "lp_npz2/"; inst = "afiro";

    std::cout << "Test of " << algoType << " on instance " << inst << " for " <<  type_inst  << std::endl;
    std::cout << "======================================================" << std::endl;


    auto [A, B,
          lowerBoundsA, lowerBoundsB,
          upperBoundsA, upperBoundsB ]  = get_sparse_matrice(set_web, type_inst,set_inst, set_inst_test, inst);



    //std::cout << "A = " << std::endl <<  A << std::endl;
    //std::cout << "B = " << std::endl <<  B << std::endl;

    std::tuple<bool,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,Eigen::SparseMatrix<double>,
    std::string> result;

    // Initialize  timers
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();


    // Initialize solution
    Solutions solution;



    Algo_enhanced testor;
    // Record stating time
    start = std::chrono::high_resolution_clock::now();
    if (!luby) {

        std::vector<std::vector<int>> FailRows;
        std::vector<std::vector<int>> FailCols;
        std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_row_corr;
        std::vector<std::tuple<int,int,int,int>>  incom_couples_rows_cols;
        int number_failed_perfects_matching(0);

        result = testor.enhanced_check_equiv_between_asso_lp_matrices_learning(luby, algoType, A,B,
                                                                                  lowerBoundsA, lowerBoundsB,
                                                                         upperBoundsA, upperBoundsB,
                                                                                  timeout,
                                                                                  FailRows,FailCols,
                                                                                learned_wrong_row_corr,
                                                                                incom_couples_rows_cols,
                                                                                number_failed_perfects_matching);
    }
    else result = testor.luby_learning(intial_num_failed, algoType, A,B,lowerBoundsA, lowerBoundsB,
              upperBoundsA, upperBoundsB,timeout);

    // Record ending time
    end = std::chrono::high_resolution_clock::now();
    solution = Solutions(A,B,result,algoType);

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

            solution.get_elts_to_validate_solution();
            if (solution.validate_a_solution()) valid_or_not = "valid-solution";
            else valid_or_not = "approximate-solution";
        }
        if (std::get<0>(result) == false  && std::get<5>(result) == "unknown-due-to-timeout") final_result = "unknown";
        if (std::get<0>(result) == false  && std::get<5>(result) == "unknown-due-to-restart") final_result = "unknown-due-to-restart";
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
    std::string intial_num_failed_string = std::to_string(intial_num_failed);


    record_solution_with_python(model, solver, max_scale, timeout_or_not,
                               type_inst, set_inst,set_inst_test,inst,set_web,
                               final_result, time_ex ,  valid_or_not , num_columns ,num_lines, intial_num_failed_string, seed);

    return solution.get_elts_to_validate_solution(true);
}


int main(int argc, char* argv[]) {

    std::string seed = argv[9];
    // Seed with current time
    srand(std::stoi(seed));

    std::string algoType = argv[1];
    std::string set_web = argv[4];
    std::string type_inst = argv[3];
    std::string set_inst = argv[5];
    std::string set_inst_test = argv[6];
    std::string inst = argv[7];
    std::string intial_num_failed_string = argv[8];
    std::string luby_string = argv[10];
    int timeout_min_int = std::stoi(argv[2]);

    // 2. Convert integer to std::chrono::minutes
    std::chrono::minutes timeout_min(timeout_min_int);

    // Convert to microseconds
    auto timeout = std::chrono::duration_cast<std::chrono::microseconds>(timeout_min);

    int intial_num_failed = std::stoi(intial_num_failed_string);

    if (argc < 2) {
        std::cerr << "Erreur : Veuillez fournir au moins un argument." << std::endl;
        return 1; // Quitte avec un code d'erreur
    }

    bool luby = false;
    if (luby_string == "luby") luby = true;

    test_equiv_lp_one_thread(luby, seed, intial_num_failed, algoType, set_web,type_inst,set_inst,set_inst_test, inst, timeout);

    return 0;
}
