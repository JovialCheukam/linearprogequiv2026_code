import sys
from pathlib import Path


from gen_mat_permut import MatForTestPermut

    
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
    luby_cst = argv[15]
    seed = argv[16]

    # get the characteristics of the instance:   
    num_permut_ctrs,num_permut_vars,num_permut_coefs,num_modif_coefs,num_scaled_ctrs = testing.get_chars_inst(argv[5],
                                                                                                              argv[6],
                                                                                                              argv[7],
                                                                                                              argv[8])

    result_one_inst_with_statistics = {
       'instance name':[argv[8]],
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
       'type of instance':[type_inst],
       'origin of the MPS': [set_web],
       'result of the test': [result],
       'execution time':[time_ex],
       'validation':[valid_or_not],
       'luby_cst':[luby_cst],
       'seed':[seed]
    }

    # identify the type of test or instance and the directory in which
    # it is recorded
    if  type_inst == 'eq':
        type_ins_f = "equal_matrices/"
    elif type_inst == 'not_eq':
        type_ins_f = "not_equal_matrices/"
    elif type_inst == 'eq_pro':
        type_ins_f = "scaled_matrices/"
    else :
        type_ins_f = "not_scaled_matrices/"
     
    # get the directory in which to save the results and statistics of one instance
    root = str(Path.home())
    directory = root+"/equivalence_linear_program_model/data_set/"+set_web+argv[6]+"/"+argv[7]+argv[6]+"_"+type_ins_f

    pickle_file_num = directory+"charac_"+argv[5]+"_"+argv[8]+".pickle"

    data = testing.get_data_saved_in_pickle_format(pickle_file_num)
    num = data['rank']

    luby = "_luby" + str(luby_cst) + "_" + str(seed)
    
    # get the file in which to record the results and statistics of one instance
    pickle_file = directory+argv[1]+"_"+argv[2]+"_"+argv[8]+argv[5]+str(num)+luby+".pickle"
    
    # save the results and statistics of one instance in the file defined above
    testing.save_any_data_format_with_pikle(result_one_inst_with_statistics,pickle_file)

save_results_of_algos()