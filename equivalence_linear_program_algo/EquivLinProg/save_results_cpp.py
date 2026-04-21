import sys

from gen_mat_permut import MatForTestPermut


"""
yes_or_no = argv[4],
type_ins  = argv[5],
set_web  = argv[10],
result  = argv[11],
time_ex  = argv[12],
result_[1]  = argv[13],
"""
    
def save_results_of_algos():
    # initiate the class generator of instances to test
    testing = MatForTestPermut()
    
    argv = sys.argv
    yes_or_no = argv[4]
    type_inst  = argv[5]
    set_web  = argv[9]
    result  = argv[10]
    time_ex  = argv[11]
    valid_or_not  = argv[12]
    num_columns  = argv[13]
    num_lines  = argv[14]

    # get the characteristics of the instance:   
    num_permut_ctrs,num_permut_vars,num_permut_coefs,num_modif_coefs,num_scaled_ctrs = testing.get_chars_inst(argv[5],
                                                                                                              argv[6],
                                                                                                              argv[7],
                                                                                                              argv[8])

    result_one_inst_with_statistics = {
       'instance name':[argv[9]],
       'number of variables': [num_columns],
       'number of constraints': [num_lines],
       'number of permuted constraints':[num_permut_ctrs],
       'number of permuted variables': [num_permut_vars],
       'number of permuted coef variables': [num_permut_coefs],
       'number of modified coef variables':[num_modif_coefs],
       'number of scaled constraints':[num_scaled_ctrs],
       'model':[argv[1]],
       'solver':[argv[2]],
       'max scale':[argv[3]],
       'timeout with no decision': [yes_or_no],
       'type of instance':[type_ins],
       'origin of the MPS': [set_web],
       'result of the test': [result],
       'execution time':[time_ex],
       'validation':[valid_or_not]
    }

    # identify the type of test or instance and the directory in which
    # it is recorded
    if  type_inst == 'eq':
        type_ins = "equal by permutation"
        type_ins_f = "equal_matrices/"
    elif type_inst == 'not_eq':
        type_ins = "not equal by permutation"
        type_ins_f = "not_equal_matrices/"
    elif type_inst == 'eq_pro':
        type_ins = "equal by permutation and scaling"
        type_ins_f = "scaled_matrices/"
    else :
        type_ins = "not equal by permutation and scaling"
        type_ins_f = "not_scaled_matrices/"
     
    # get the directory in which to save the results and statistics of one instance
    root = "/Users/admin/Documents/POSTDOC_MichealMorin/CODE_PostDoc/env_code_postdoc_jovial/for_fir/code_exp_postdoc_jov_/"
    #root = 
    directory = root+"data_set/"+set_web+argv[7]+"/"+argv[8]+argv[7]+"_"+type_ins_f

    pickle_file_num = directory+"charac_"+argv[6]+"_"+argv[9]+".pickle"

    data = testing.get_data_saved_in_pickle_format(pickle_file_num)
    num = data['rank']
    
    # get the file in which to record the results and statistics of one instance
    pickle_file = directory+argv[1]+"_"+argv[2]+"_"+argv[9]+argv[6]+str(num)+".pickle"
    
    # save the results and statistics of one instance in the file defined above
    testing.save_any_data_format_with_pikle(result_one_inst_with_statistics,pickle_file)