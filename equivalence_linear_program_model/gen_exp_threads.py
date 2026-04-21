
import pandas as pd
from gen_mat_permut import MatForTestPermut



list_type_inst = ["eq","not_eq"]
max_lin_size_scale = 500
max_col_size_scale = 500
max_lin_size_eq = 5000
max_col_size_eq = 5000

min_medium_size = 156651
max_medium_size = 1115248

timeout = 2780
max_k = 10
list_solvers = [#"scip","cplex"#,
                #"chuffed","cp-sat" 
                "coinbc"#,
                #"gecode"#
                ]
list_models = [#"theorem_model"#,
          #"quadratic_model"#,
          #"algo",
          "algo_enhanced"
          ]
scaling = "False"

#set_web_name = "knapsack"
#set_web_name = "linear"
#set_web_name = "polynomial"
set_web_name = "netlib"

def gen_inst(set_web_name,first_inst):
    # Generate the instances 
    list_insts = []
    if set_web_name == 'netlib' :
        for i in range(63):
            list_insts.append("flow_lp" + str(i+1))
    else :
        for i in range(first_inst,100):
            if set_web_name == 'linear':
                if i != 62 :
                    list_insts.append("Mat_"+ set_web_name + str(i+1))
            else :
                list_insts.append("Mat_"+ set_web_name + str(i+1))
    return list_insts

def select_inst_threads(set_web_name,list_type_inst,list_solvers,list_models):

    threads = {}
    testing = MatForTestPermut()

    if set_web_name == "collection":
        list_set_inst = [("miplib/","collection","collection_npz/")]
    if set_web_name == "benchmark":
        list_set_inst = [("miplib/","benchmark","benchmark_npz/")]
    if set_web_name == "netlib":
        list_set_inst = [("netlib/","lp","lp_npz2/")]
        list_insts = gen_inst(set_web_name,0)
    if set_web_name == "linear":    
        list_set_inst = [("linear_fitting/","linear","linear_npz/")]
        list_insts = gen_inst(set_web_name,2)
    if set_web_name == "polynomial":    
        list_set_inst = [("polynomial_fitting/","polynomial","polynomial_npz/")]
        list_insts = gen_inst(set_web_name,2)
    if set_web_name == "knapsack":
        list_set_inst = [("fractional_multidim_knapsack/","knapsack","knapsack_npz/")]
        list_insts = gen_inst(set_web_name,1)
    
    thread_rank = 0
    with open('threads_coinbc_'+set_web_name+'.txt', "w") as file:
    
        for (set_web,set_inst,set_inst_test) in list_set_inst :
            for type_inst in list_type_inst:
                if set_web == "netlib/" :
                    df = pd.read_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')
                #if type_inst == "eq_pro" or type_inst == "not_eq_pro":
                #    max_lin_size = max_lin_size_scale
                #    max_col_size = max_col_size_scale
                #else :
                #    max_lin_size = max_lin_size_eq
                #    max_col_size = max_col_size_eq
                if set_web == "netlib/" :
                    list_insts = df[(df['size'] >= min_medium_size)]['instance name']
                #if set_web == "netlib/" :
                #    list_insts = df['InstanceInst.']
                for inst in list_insts :
                    for model in list_models :
                    
                       for solver in list_solvers :
                           if model in ["algo_test","enhanced_test","enhanced2_test"]:
                              solver = "no-solver"
                              if model in ["enhanced_test","enhanced2_test"] and not type_inst in ["eq_pro","not_eq_pro"] :
                                 pass
                              else:
                                  thread_rank = thread_rank + 1
                                  threads[thread_rank] = [model,solver,type_inst,set_inst,set_inst_test,inst]
                                  file.write(model+" "+solver+" "+str(max_k)+" "+scaling+" "+str(timeout)+" "+type_inst+" "+set_inst+" "+set_inst_test+" "+inst+" "+str(thread_rank)+"\n")
                             
                              
                              break
                           else :
                               thread_rank = thread_rank + 1
                               threads[thread_rank] = [model,solver,type_inst,set_inst,set_inst_test,inst]
                               file.write(model+" "+solver+" "+str(max_k)+" "+scaling+" "+str(timeout)+" "+type_inst+" "+set_inst+" "+set_inst_test+" "+inst+" "+str(thread_rank)+"\n")
                           
                              
                          
                               
                               
                           
    testing.save_any_data_format_with_pikle(threads,"threads.pickle")
    print("num_threads =",thread_rank)
    return threads

#select_inst_threads(set_web_name,list_type_inst,list_solvers,list_models)

def select_algos_threads(set_web_name,list_type_inst,list_models):

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

    thread_rank = 0

    with open('threads_luby_algos_'+set_web_name+'.txt', "w") as file:
    
        for (set_web,set_inst,set_inst_test) in list_set_inst :
            
            if set_web == "netlib/":
                df = pd.read_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')
                list_insts = df[(df['size'] < min_medium_size)]['instance name']
            #if set_web == "netlib/" :
            #    list_insts = df['InstanceInst.']
            for type_inst in list_type_inst:

                for inst in list_insts :
                    for model in list_models :
                        file.write(model+" "+str(timeout)+" "+type_inst+" "+set_web+" "+set_inst+" "+set_inst_test+" "+inst+"\n")

                        thread_rank = thread_rank + 1

    print("num_threads =",thread_rank)


#select_algos_threads(set_web_name,list_type_inst,list_models)




#################################### LUBY ALGO ##############################

def select_luby_algos_threads(set_web_name,list_type_inst,list_models,luby_cst_set,seed_set):

    if set_web_name == "collection":
        list_set_inst = [("miplib/","collection","collection_npz/")]
    if set_web_name == "benchmark":
        list_set_inst = [("miplib/","benchmark","benchmark_npz/")]
    if set_web_name == "netlib":
        list_set_inst = [("netlib/","lp","lp_npz2/")]
        #list_insts = gen_inst(set_web_name,0)
    if set_web_name == "linear":    
        list_set_inst = [("linear_fitting/","linear","linear_npz/")]
        list_insts = gen_inst(set_web_name,2)
    if set_web_name == "polynomial":    
        list_set_inst = [("polynomial_fitting/","polynomial","polynomial_npz/")]
        list_insts = gen_inst(set_web_name,2)
    if set_web_name == "knapsack":
        list_set_inst = [("fractional_multidim_knapsack/","knapsack","knapsack_npz/")]
        list_insts = gen_inst(set_web_name,1)

    thread_rank = 0

    with open('threads_luby_algo_full_restarts_'+set_web_name+'.txt', "w") as file:
    
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
                                file.write(model+" "+str(timeout)+" "+type_inst+" "+set_web+" "+set_inst+" "+set_inst_test+" "+inst+" "+str(luby_cst)+" "+str(seed)+"\n")

                                thread_rank = thread_rank + 1
    
    print("num_threads =",thread_rank)


# For full restarts
luby_cst_set = [1024]
seed_set = [954716]

select_luby_algos_threads(set_web_name,list_type_inst,list_models,luby_cst_set,seed_set)