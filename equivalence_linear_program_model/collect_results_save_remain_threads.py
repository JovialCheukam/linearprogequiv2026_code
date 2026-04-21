import pandas as pd
from gen_mat_permut import MatForTestPermut


list_type_inst = ["eq","not_eq"]
max_lin_size_scale = 500
max_col_size_scale = 500
max_lin_size_eq = 5000
max_col_size_eq = 5000

timeout = 1440
max_k = 1
list_solvers = ["cp-sat","coinbc","chuffed","gecode"#,
                #"scip","cplex"
                ]
list_models = ["theorem_model",
          "quadratic_model",
          "permutation_model",
          "algo",
          "algo_enhanced"
          ]
"""
sys.argv[1] = model
sys.argv[2] = solver
sys.argv[3] = max_k
sys.argv[4] = timeout
sys.argv[5] = type_inst
sys.argv[6] = set_inst
sys.argv[7] = set_inst_test
sys.argv[8] = inst
"""

def find_pickle_file_one_result(model,solver,type_inst,set_inst,set_inst_test,inst) :

    testing = MatForTestPermut()

    _, _, _,type_ins_f,set_web = testing.collect_results_stats(type_inst = type_inst,set_inst = set_inst)

    # get the directory in which to save the results and statistics of one instance
    directory = "data_set/"+set_web+set_inst+"/"+set_inst_test+set_inst+"_"+type_ins_f

    pickle_file_num = directory+"charac_"+type_inst+"_"+inst+".pickle"

    data = testing.get_data_saved_in_pickle_format(pickle_file_num)
    num = data['rank']
    
    # get the file in which to record the results and statistics of one instance
    pickle_file = directory+model+"_"+solver+"_"+inst+type_inst+str(num)+".pickle"
      
    return pickle_file

def handle_absent_results(model,solver,type_inst,set_inst,set_inst_test,inst):

    testing = MatForTestPermut()

    _, _, type_ins,_,set_web = testing.collect_results_stats(type_inst = type_inst,set_inst = set_inst)

    A,_ = testing.get_testing_matrices_from_npz(False,type_inst,set_inst,
                                                        set_inst_test,inst,"_A")
    num_of_lines, num_of_columns = A.shape

    num_permut_ctrs,num_permut_vars,num_permut_coefs,num_modif_coefs,num_scaled_ctrs = testing.get_chars_inst(type_inst,set_inst,set_inst_test,inst)

    result_absent_one_inst = {}
    result_absent_one_inst['instance name'] = [inst]
    result_absent_one_inst['number of variables'] = [num_of_columns]
    result_absent_one_inst["number of constraints"] = [num_of_lines]
    result_absent_one_inst['size'] = [num_of_columns*num_of_lines]
    result_absent_one_inst['number of permuted constraints'] = [num_permut_ctrs]
    result_absent_one_inst['number of permuted variables'] = [num_permut_vars]
    result_absent_one_inst['number of permuted coef variables'] = [num_permut_coefs]
    result_absent_one_inst['number of modified coef variables'] = [num_modif_coefs]
    result_absent_one_inst['number of scaled constraints'] = [num_scaled_ctrs]
    result_absent_one_inst['model'] = [model]
    result_absent_one_inst['solver'] = [solver]
    result_absent_one_inst['max scale'] = [1]
    result_absent_one_inst['timeout with no decision'] =['unknown'] 
    result_absent_one_inst['type of instance'] = [type_ins]
    result_absent_one_inst['origin of the MPS'] = [set_web]
    result_absent_one_inst['result of the test'] = ["unknown"]
    result_absent_one_inst['execution time'] = [" "]
    result_absent_one_inst['validation'] = ["no result file"]
    
    return result_absent_one_inst

def gen_inst(set_web_name,first_inst):
    # Generate the instances 
    list_insts = []
    for i in range(first_inst,100):
        if set_web_name == 'linear':
            if i != 62 :
                list_insts.append("Mat_"+ set_web_name + str(i+1))
        else :
            list_insts.append("Mat_"+ set_web_name + str(i+1))
    return list_insts

def collect_all_results_(list_type_inst,list_solvers,list_models):

   
    testing = MatForTestPermut()

    # initialise the recorder of results and statistics
    result_one_inst_with_statistics_f = {
       'instance name': [],
       'number of variables': [],
       'number of constraints': [],
       'size': [],
       'number of permuted constraints': [],
       'number of permuted variables': [],
       'number of permuted coef variables': [],
       'number of modified coef variables': [],
       'number of scaled constraints': [],
       'model': [],
       'solver': [],
       'max scale': [],
       'timeout with no decision': [],
       'type of instance': [],
       'origin of the MPS': [],
       'result of the test': [],
       'execution time': [],
       'validation':[]
    }

    #set_web_name = "knapsack"
    #set_web_name = "linear"
    #set_web_name = "polynomial"
    set_web_name = "netlib"

    if set_web_name == "collection":
        list_set_inst = [("miplib/","collection","collection_npz/")]
    if set_web_name == "benchmark":
        list_set_inst = [("miplib/","benchmark","benchmark_npz/")]
    if set_web_name == "netlib":
        list_set_inst = [("netlib/","lp","lp_npz2/")]
    if set_web_name == "linear":    
        list_set_inst = [("linear_fitting/","linear","linear_npz/")]
        list_insts = gen_inst(set_web_name,2)
    if set_web_name == "polynomial":    
        list_set_inst = [("polynomial_fitting/","polynomial","polynomial_npz/")]
        list_insts = gen_inst(set_web_name,2)
    if set_web_name == "knapsack":
        list_set_inst = [("fractional_multidim_knapsack/","knapsack","knapsack_npz/")]
        list_insts = gen_inst(set_web_name,1)
                 


    with open("remain_threads_"+set_web_name+".txt", "w") as file: 
       job_number = 0   
       for (set_web,set_inst,set_inst_test) in list_set_inst :
            if set_web == "netlib/" :
                df = pd.read_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')
            for type_inst in list_type_inst:
                if set_web == "netlib/" :
                    list_insts = df['instance name']
                for inst in list_insts :
                   
                       for model in list_models :
                    
                            for solver in list_solvers :
                                try :
                                    if model in ["algo","algo_enhanced"]:
                                        solver = 'no_solver'
                                        if model in ["enhanced_test","enhanced2_test"] and not type_inst in ["eq_pro","not_eq_pro"] :
                                            pass
                                        else:
                                            pickle_file = find_pickle_file_one_result(model,solver,type_inst,set_inst,set_inst_test,inst) 
                                            
                                            result_one_inst_with_statistics = testing.get_data_saved_in_pickle_format(pickle_file)
                                            
                                            for charac in result_one_inst_with_statistics_f.keys():
                                    
                                                if charac == 'size':
                                                    added = [int(result_one_inst_with_statistics['number of variables'][0])*int(result_one_inst_with_statistics['number of constraints'][0])]
                                                else :
                                                    added = result_one_inst_with_statistics[charac]
                                                result_one_inst_with_statistics_f[charac] = result_one_inst_with_statistics_f[charac]+added
                                        break
                                    else :
                                        pickle_file = find_pickle_file_one_result(model,solver,type_inst,set_inst,set_inst_test,inst) 
                                        result_one_inst_with_statistics = testing.get_data_saved_in_pickle_format(pickle_file)
                                        for charac in result_one_inst_with_statistics_f.keys():
                                            if charac == 'size':
                                                    added = [int(result_one_inst_with_statistics['number of variables'][0])*int(result_one_inst_with_statistics['number of constraints'][0])]
                                            else :
                                                    added = result_one_inst_with_statistics[charac]
                                            result_one_inst_with_statistics_f[charac] = result_one_inst_with_statistics_f[charac]+added
                                        
                                except Exception as e :
                                    job_number = job_number + 1
                                    file.write(model+" "+solver+" "+str(max_k)+" False "+str(timeout)+" "+type_inst+" "+set_inst+" "+set_inst_test+" "+inst+" "+str(job_number)+"\n")         
                                    result_absent_one_inst = handle_absent_results(model,solver,type_inst,set_inst,set_inst_test,inst)
                                    for charac in result_one_inst_with_statistics_f.keys():
                                        added = result_absent_one_inst[charac]
                                        result_one_inst_with_statistics_f[charac] = result_one_inst_with_statistics_f[charac]+added
                                    if model in ["algo","algo_enhanced"]:
                                        break
                                        
    #load results into a DataFrame object:
    df = pd.DataFrame(result_one_inst_with_statistics_f)

    #save the results in csv file:
    df.to_csv('results_exp/results_of_experiments_'+set_web_name+'.csv')
   
collect_all_results_(list_type_inst,list_solvers,list_models)

