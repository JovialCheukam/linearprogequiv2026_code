import sys
import time
#from algo_mat_permut import Algo_test_equal_permut_mat,Algo_test_proport_permut_mat
from model_mat_permut import ModelsMiniZinc
from gen_mat_permut import MatForTestPermut

# This routine converts seconds to a string in HH:MM:SS format.
def convert_secs_to_hour_min_sec(seconds):
    hours, remainder = divmod(seconds, 3600)
    hours = seconds // 3600
    remainder = seconds - hours * 3600
    minutes = remainder // 60
    seconds_ = remainder - minutes * 60
    return "{:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds_))


# This function test the instance according to its characteristics
def test(model, testor, type_inst,  timeout,solver, inst, set_web):
    
    
      directory = "data_set/"+ set_web + "dzn_for_eq/matrices_" + type_inst + "/"
       
      return testor.solve_instance(timeout, model, solver, directory, inst, type_inst)
   
# This function identify the instance and the type of test to apply to the instance
# and effectively test the instance with the identified model of test
def experiment(model,solver,max_k, timeout, type_inst, testing, inst, set_inst, A, B,
               lowerBoundsMatA,upperBoundsMatA,lowerBoundsMatB,upperBoundsMatB):
    
      # for printing purpose the below code identify the characteristics to didplay
      _, _, _,type_ins_f,set_web = testing.collect_results_stats(type_inst = type_inst,set_inst = set_inst)
      print("\nTest for the ",model, "with", solver, "solver", "on the instance ", inst, "for ",type_ins_f)
      print("----------------------------------------------------------------------------------------------")

      # get the size of the instance
      num_lines,num_columns = A.shape
      print("num_lines :", num_lines)
      print("num_columns :", num_columns)
    
      # identify the test method and define it initialisation
    
      testor = ModelsMiniZinc(A,B,lowerBoundsMatA,upperBoundsMatA,lowerBoundsMatB,upperBoundsMatB)
      if model not in ["theorem_model","quadratic_model"] :
          raise TypeError("The input constraint model", model," is unknown !")
          
      min_k = 1
      if type_inst == "eq" or type_inst == "not_eq" :
          max_k = 1
      else :
          max_k = max_k

      
      testing.gen_dzn_for_eq_inst(type_inst,inst,set_web,min_k,max_k,A, B,
               lowerBoundsMatA,upperBoundsMatA,lowerBoundsMatB,upperBoundsMatB, model)
       

    
      # initiate the time compter
      start_time = time.time()

      # execute the test of equality of matrices by permutation
      result = test(model, testor, type_inst,  timeout,solver, inst,set_web)

      # record the time of execution
      end_time = time.time()

      # compute the execution time 
      execution_time = end_time - start_time

      return result, execution_time

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


def launch_a_thread_experiment():
    
    # initiate the class generator of instances to test
    testing = MatForTestPermut()
    
    
    # get the thread of a test of an instance with its characteristics as arguments
    # of sys.argv. The arguments are embedded in a string separated by the space ' '
    argv = sys.argv

   
    
    
    # get the instance of to matrices A and B of two linear programs in standard form
    A,_ = testing.get_testing_matrices_from_npz(argv[5],argv[6],
                                                argv[7],argv[8],"_A")
    B,_ = testing.get_testing_matrices_from_npz(argv[5],argv[6],
                                                argv[7],argv[8],"_B")
    
    lowerBoundsMatA,_ = testing.get_testing_matrices_from_npz(argv[5],argv[6],
                                                argv[7],argv[8],"_lowerBoundsA")
    lowerBoundsMatB,_ = testing.get_testing_matrices_from_npz(argv[5],argv[6],
                                                argv[7],argv[8],"_lowerBoundsB")
    

    upperBoundsMatA,_ = testing.get_testing_matrices_from_npz(argv[5],argv[6],
                                                argv[7],argv[8],"_upperBoundsA")
    upperBoundsMatB,_ = testing.get_testing_matrices_from_npz(argv[5],argv[6],
                                                argv[7],argv[8],"_upperBoundsB")
    
   
    # run the experiment and get the result and the execution time
    result_, execution_time = experiment(argv[1],argv[2],int(argv[3]),
                                         int(argv[4]),argv[5],testing, argv[8],argv[6], A, B,
                                         lowerBoundsMatA,upperBoundsMatA,lowerBoundsMatB,upperBoundsMatB)
    
    # collection of statistics of the experiment
    yes_or_no, result, type_ins, type_ins_f, set_web = testing.collect_results_stats(int(argv[4]),
                                                                                     execution_time,
                                                                                     result_[0] ,
                                                                                     argv[5],
                                                                                     argv[6])
    print("result = ", result, " and ",result_[1])
    # get the size of the instance
    num_lines,num_columns = A.shape

    # convert the execution time in normalized format: h-min-s
    time_ex = convert_secs_to_hour_min_sec(execution_time)
    
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
       'type of instance':[type_ins],
       'origin of the MPS': [set_web],
       'result of the test': [result],
       'execution time':[time_ex],
       'validation':[result_[1]]
    }
     
    # get the directory in which to save the results and statistics of one instance
    directory = "data_set/"+set_web+argv[6]+"/"+argv[7]+argv[6]+"_"+type_ins_f

    pickle_file_num = directory+"charac_"+argv[5]+"_"+argv[8]+".pickle"

    data = testing.get_data_saved_in_pickle_format(pickle_file_num)
    num = data['rank']
    
    # get the file in which to record the results and statistics of one instance
    pickle_file = directory+argv[1]+"_"+argv[2]+"_"+argv[8]+argv[5]+str(num)+".pickle"
    
    # save the results and statistics of one instance in the file defined above
    testing.save_any_data_format_with_pikle(result_one_inst_with_statistics,pickle_file)



launch_a_thread_experiment()
