
import numpy as np
import time
import scipy as sc
from gen_mat_permut import Solutions
import subprocess


# This class create the simple model in MiniZinc
class ModelsMiniZinc:
  
  def __init__(self, A = None, B = None, lowerBoundsMatA = None,
                           upperBoundsMatA = None,
                           lowerBoundsMatB = None,
                           upperBoundsMatB = None,
                           ):
        if A != None :
            assert(type(A) == sc.sparse._csr.csr_matrix)
            assert(type(B) == sc.sparse._csr.csr_matrix)
            #assert(A.dtype == "int64" and B.dtype == "int64")
        self.A = A
        self.B = B
        self.lowerBoundsMatA = lowerBoundsMatA
        #print("lowerBoundsMatA = ",self.lowerBoundsMatA)
        self.upperBoundsMatA = upperBoundsMatA
        #print("upperBoundsMatA = ",self.upperBoundsMatA)
        self.lowerBoundsMatB = lowerBoundsMatB
        #print("lowerBoundsMatB = ",self.lowerBoundsMatB)
        self.upperBoundsMatB = upperBoundsMatB
        #print("upperBoundsMatB = ",self.upperBoundsMatB)
        
        
    
           


  # solve the instance
  def solve_instance(self, timeout, model_name, solver, directory, inst_name, type_inst):
        

        solve_cmd = [ "< MiniZinc Directory >",
                     #"/Applications/MiniZincIDE.app/Contents/Resources/minizinc",
                     #'/home/jovial/MiniZincIDE-2.9.3-bundle-linux-x86_64/bin/minizinc',
                    "--solver", solver, "--time-limit", str(timeout*60000), "--free-search", 
                    "--solver-statistics", "-o", directory + "results_" + solver + "_" + model_name + "_" + inst_name + ".txt", 
                    model_name + ".mzn", directory + model_name + "_"+inst_name + ".dzn"]
        
        
        subprocess.run(solve_cmd, capture_output=True, text=True, check=True)
    

        with open(directory + "results_" + solver + "_" + model_name + "_" + inst_name + ".txt", 'r') as file:
            all_lines = file.readlines()
            
            line_num = 0
            for line in all_lines:
                
                if line in ["=====UNSATISFIABLE=====\n",'=====UNBOUNDED=====\n',
                            "=====UNSATorUNBOUNDED=====\n"]:
                    print("result: ",line)
                    return False, None
                if line in ["=====UNKNOWN=====\n"]:
                    return "unknown due to timeout", None
                if line == "=====ERROR=====\n":
                    print("Error during solving")
                    
                    exit(1)
                else :
                    return self.get_results(line_num, all_lines, model_name, type_inst)
                line_num =  line_num + 1
        return False, None
  
  def get_results(self, line_num0, all_lines, model_name, type_inst):
      
      m, n = self.A.shape
      result = Results()
      if model_name == "quadratic_model" :
          for line_num in range(line_num0,len(all_lines)):
                if all_lines[line_num] == "k1 = \n" :
                        if len(all_lines[line_num :]) >= 9 + n + m :
                            k1 = [int(i) for i in all_lines[line_num + 1].split()]
                            k2 = [int(i) for i in all_lines[line_num + 4].split()]
                            P = [[int(i) for i in all_lines[line_num + 7 + j].split()] for j in range(m)]
                            
                            P = np.array(P)
                            Q = [[int(i) for i in all_lines[line_num + 10 + m + j].split()] for j in range(n)]
                            Q = np.array(Q)
                            result = Results(k1,k2,P,Q)
                        break

      if model_name in ["permutation_model","theorem_model"] :
          for line_num in range(line_num0,len(all_lines)):
                if all_lines[line_num] == "k1 = \n" :
                        
                        if len(all_lines[line_num :]) >= 10 :
                            
                            k1 = [int(i) for i in all_lines[line_num + 1].split()]
                            k2 = [int(i) for i in all_lines[line_num + 4].split()]
                            l = [int(i) for i in all_lines[line_num + 7].split()]
                            c = [int(i) for i in all_lines[line_num + 10].split()]
                            result = Results(k1,k2,l = l, c = c)
                        break
      
      if result.solution.k1 is None :
          return False, "Or missing results in the file"
      Sol = Solutions(self.A,self.B,result,model_name,type_inst)
      Sol.get_scaling_permutations_matrices()
      return Sol.validate_solution()
          

  
  def check_solvability_of_instance(self, set_web, inst_dzn_file,  Solver, timeout, checked_inst):

    command_cp = ["cp", inst_dzn_file, "data_set/" + set_web + "dzn/data.dzn"]
    subprocess.run(command_cp, capture_output=True, text=True, check=True)


    solve_cmd = ["/Applications/MiniZincIDE.app/Contents/Resources/minizinc",
                 #'/home/jovial/MiniZincIDE-2.9.3-bundle-linux-x86_64/bin/minizinc',
                "--solver", Solver, "--time-limit", str(timeout), "--output-objective", "--solver-statistics", 
                "-o", "data_set/" + set_web + "dzn/results.txt", "data_set/" + set_web + "dzn/test_lp.mzn"]
    # initiate the time compter
    start_time = time.time()
    subprocess.run(solve_cmd, capture_output=True, text=True, check=True)
    end_time = time.time()
    # compute the execution time 
    execution_time = end_time - start_time

    with open('data_set/' + set_web + 'dzn/results.txt', 'r') as file:
        all_lines = file.readlines()
       
        sol = 0
        for line in all_lines:
            
            if line in ["=====UNSATISFIABLE=====\n",'=====UNBOUNDED=====\n',
                        "=====UNSATorUNBOUNDED=====\n","=====UNKNOWN=====\n"]:
                print("result: ",line)
                return False, execution_time
            if line == "=====ERROR=====\n":
                print("Error")
                print(all_lines)
                exit(1)
            else :
                list_words = line.split()
                
                if set_web =="linear_fitting/":
                    if list_words[0] == "vX":
                        print(list_words)
                        checked_inst.append(float(list_words[2][1:-1]))
                        checked_inst.append(float(list_words[3][:-1]))
                        print("checked_inst = ", checked_inst)
                        return True, execution_time
                elif set_web =="polynomial_fitting/" :
                    
                    if list_words[0] == "vX":
                        print(list_words)
                        degre = checked_inst[0]
                        coef0 = list_words[2][1:-1]
                        checked_inst.append(float(coef0))
                        splited_line = list_words[3:degre+3]
                        for coef in splited_line:
                            checked_inst.append(float(coef[:-1]))
                        
                        return True, execution_time
                else :
                    
                    if list_words[0] == "vX":
                        sol = 1
                        print(line)
                    for word in list_words:
                        if sol == 1 and word.split("=")[0] == "objective" :
                            return True, execution_time
    return False, execution_time
    
class Results:

    def __init__(self, k1 = None,k2 = None,P = None,
                       Q = None,l = None, c = None) :
        
        self.solution = EltsOfSolution(k1, k2, P, Q, l , c)

class EltsOfSolution:

    def __init__(self, k1 = None,k2 = None,P = None,
                       Q = None,l = None, c = None) :
        self.k1 = k1
        self.k2 = k2
        self.P = P
        self.Q = Q
        self.l = l
        self.c = c