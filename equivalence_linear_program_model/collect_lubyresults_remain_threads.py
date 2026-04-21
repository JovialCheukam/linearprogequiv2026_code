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
list_models = [#"theorem_model",
          #"quadratic_model",
          #"permutation_model",
          #"algo",
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

def find_pickle_file_one_result(model,solver,type_inst,set_inst,set_inst_test,inst,luby_cst,seed) :

    testing = MatForTestPermut()

    _, _, _,type_ins_f,set_web = testing.collect_results_stats(type_inst = type_inst,set_inst = set_inst)

    # get the directory in which to save the results and statistics of one instance
    directory = "data_set/"+set_web+set_inst+"/"+set_inst_test+set_inst+"_"+type_ins_f

    pickle_file_num = directory+"charac_"+type_inst+"_"+inst+".pickle"

    data = testing.get_data_saved_in_pickle_format(pickle_file_num)
    num = data['rank']

    luby = "_luby" + str(luby_cst) + "_" + str(seed)
    
    # get the file in which to record the results and statistics of one instance
    pickle_file = directory+model+"_"+solver+"_"+inst+type_inst+str(num)+luby+".pickle"
      
    return pickle_file

def handle_absent_results(model,solver,type_inst,set_inst,set_inst_test,inst,luby_cst,seed):

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
    result_absent_one_inst['total hour'] = [72]
    result_absent_one_inst['validation'] = ["no result file"]
    result_absent_one_inst['luby_cst']=[luby_cst]
    result_absent_one_inst['seed']=[seed]
    
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

def get_total_hour(execution_time):
    hs_ms_ss = execution_time[0].split(":")
    return int(hs_ms_ss[0]) + int(hs_ms_ss[1])//60 + int(hs_ms_ss[2])//3600

def collect_all_results_(set_web_name,list_type_inst,list_solvers,list_models,luby_cst_set,seed_set):

   
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
       'total hour': [],
       'validation':[],
       'luby_cst':[],
       'seed':[]
    }

    

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
            for seed in seed_set :
                for luby_cst in luby_cst_set :
                    if set_web == "netlib/":
                        df = pd.read_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')
                        
                        # For training phase
                        #list_insts = df[(df['size'] < min_medium_size)]['instance name']

                        # For testing phase
                        #list_insts = df[(df['size'] >= min_medium_size)]['instance name']


                    if set_web == "netlib/" :
                        list_insts = df['instance name']
                    for type_inst in list_type_inst:
                        for inst in list_insts :
                        
                            for model in list_models :
                            
                                    for solver in list_solvers :
                                        try :
                                            if model in ["algo","algo_enhanced"]:
                                                solver = 'no_solver'
                                                
                                                pickle_file = find_pickle_file_one_result(model,solver,type_inst,set_inst,set_inst_test,inst,luby_cst,seed) 
                                                
                                                result_one_inst_with_statistics = testing.get_data_saved_in_pickle_format(pickle_file)
                                                
                                                for charac in result_one_inst_with_statistics_f.keys():
                                        
                                                    if charac == 'size':
                                                        added = [int(result_one_inst_with_statistics['number of variables'][0])*int(result_one_inst_with_statistics['number of constraints'][0])]
                                                    elif charac == 'total hour':
                                                        hour = get_total_hour(result_one_inst_with_statistics['execution time'])
                                                        added = [hour]
                                                    else :
                                                        added = result_one_inst_with_statistics[charac]
                                                    result_one_inst_with_statistics_f[charac] = result_one_inst_with_statistics_f[charac]+added
                                                break
                                            else :
                                    
                                                pickle_file = find_pickle_file_one_result(model,solver,type_inst,set_inst,set_inst_test,inst,luby_cst,seed) 
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
                                            result_absent_one_inst = handle_absent_results(model,solver,type_inst,set_inst,set_inst_test,inst,luby_cst,seed)
                                            for charac in result_one_inst_with_statistics_f.keys():
                                                added = result_absent_one_inst[charac]
                                                result_one_inst_with_statistics_f[charac] = result_one_inst_with_statistics_f[charac]+added
                                            if model in ["algo","algo_enhanced"]:
                                                break

    #load results into a DataFrame object:
    df = pd.DataFrame(result_one_inst_with_statistics_f)
    return df
    
#set_web_name = "knapsack"
#set_web_name = "linear"
#set_web_name = "polynomial"
set_web_name = "netlib"

# For learning phase and small restarts
#luby_cst_set = [7,20,32,64]
#seed_set = [1,2,3,4]


# For testing phase and small restarts
luby_cst_set = [20]
seed_set = [315271, 825196,	886517,	146874,	59695, 746812,	381072,	144189,	214220,	304889]
   
df1 = collect_all_results_(set_web_name,list_type_inst,list_solvers,list_models,luby_cst_set,seed_set)


# For medium restarts
luby_cst_set = [1025]
seed_set = [954717]
   
df2 = collect_all_results_(set_web_name,list_type_inst,list_solvers,list_models,luby_cst_set,seed_set)


# For full restarts
luby_cst_set = [1024]
seed_set = [954716]
   
df3 = collect_all_results_(set_web_name,list_type_inst,list_solvers,list_models,luby_cst_set,seed_set)



for index, row in df1.iterrows():
        df1.loc[index, 'model'] = 'luby_test_small_restart'

for index, row in df2.iterrows():
        df2.loc[index, 'model'] = 'luby_test_medium_restart'

for index, row in df2.iterrows():
        df3.loc[index, 'model'] = 'luby_test_full_restart'





df4 = pd.read_csv('results_exp/results_of_experiments_'+set_web_name+'.csv')
do_not_exist_list = []
for index, row in df4.iterrows():
    do_not_exist_list.append('do_not_exist')
df4['seed'] = do_not_exist_list
df4['luby_cst'] = do_not_exist_list


df = pd.concat([df1, df2, df3, df4], ignore_index=True)
df = df.drop(columns=['Unnamed: 0'])

#save the results in csv file:
df.to_csv('results_exp/results_of_exp_luby_'+set_web_name+'.csv')