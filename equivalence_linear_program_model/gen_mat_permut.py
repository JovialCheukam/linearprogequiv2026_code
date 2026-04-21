# Fourth check of the algorithm code: with two randomn matrices lines X columns which 
# are equal or not by permutation of lines and columns

import numpy as np
import matplotlib.pyplot as plt
import random
import scipy as sc
import gurobipy as gp
import copy
import pickle
import os

class MatForTestPermut:

    def __init__(self, lines = None, 
                       columns = None, 
                       min_val = None, 
                       max_val = None, 
                       equal = None, 
                       proportional = None,
                       set_inst = None,
                       set_inst_test = None,
                       inst = None,
                       min_scale = None,
                       max_scale = None,
                       rank = None,
                       scale = None,
                       num_synthetic_exples = None,
                       degre = None):

        self.lines = lines
        self.columns = columns
        self.min_val = min_val
        self.max_val = max_val
        self.equal = equal
        self.proportional = proportional
        self.type = "eq_pro"
        self.set_inst = set_inst
        self.set_inst_test = set_inst_test
        self.inst = inst
        self.min_scale = min_scale
        self.max_scale = max_scale
        self.rank = rank
        self.scale = scale
        self.num_synth_exples = num_synthetic_exples
        self.degre = degre

        #prefix_path = '/home/jovial/scratch/code_exp_postdoc_jov_/'
        prefix_path = ''
        self.prefix_path = prefix_path
        

    def update_equal(self,equal):
        self.equal = equal
    def update_proportional(self,proportional):
        self.proportional = proportional
    def update_rank(self,rank):
        self.rank = rank

    def get_set_web(self):
        if self.set_inst == 'collection':
           set_web = "miplib/"
        elif self.set_inst == 'benchmark':
           set_web = "miplib/"
        elif self.set_inst == 'exples':
           set_web = "synthetic/"
        elif self.set_inst == 'linear':
           set_web = "linear_fitting/"
        elif self.set_inst == 'polynomial':
           set_web = "polynomial_fitting/"
        elif self.set_inst == 'knapsack':
           set_web = "fractional_multidim_knapsack/"
        else :
           set_web = "netlib/"
        return set_web

    def get_inst_directory(self):
        if  self.equal:
           type_ins_f = "equal_matrices/"
        else :
           type_ins_f = "not_equal_matrices/"
        set_web = self.get_set_web()
        directory = "data_set/"+set_web+self.set_inst+"/"+self.set_inst_test+self.set_inst+"_"+type_ins_f
        return directory

    # This function generate a random permutation of lines or columns of a matrix A
    def gen_random_permut(self, num_line_or_column):
        
        # Ensure an effective permutation (not identity)
        assert(num_line_or_column >= 3)

        # Define  a random  maximum number of permutation to generate
        max_num_of_permutations = random.randint(1,num_line_or_column)

        # Set of couples of exchanged lines or columns
        list_couples_exch = []
        list_i_j_exch = []


        # Generate list of possible couple of exchanged lines or columns
        for _ in range(max_num_of_permutations) :
                i = random.randint(1,num_line_or_column-2)
                j = random.randint(1,num_line_or_column-2)
                if i != j and i not in list_i_j_exch and j not in list_i_j_exch :
                    list_couples_exch.append((i,j))
                    list_i_j_exch.append(i)
                    list_i_j_exch.append(j)
            
        #print(list_couples_exch)
        return list_couples_exch
    
    def get_pemut_mat_col_from_list_couples_exch(self, num_cols,list_couples_exch):
        matPermutCols = np.zeros((num_cols,num_cols),dtype=np.int64)
        for i in range(num_cols):
            k = 0
            for (a,b) in list_couples_exch:
                if i == a or i == b:
                    matPermutCols[a,b] = 1
                    matPermutCols[b,a] = 1
                    break
                k = k + 1
            if k == len(list_couples_exch):
                matPermutCols[i,i] = 1

        matPermutCols = sc.sparse.csr_matrix(matPermutCols).astype(np.int64)
        return matPermutCols, sc.sparse.csr_matrix(matPermutCols[0:num_cols-1,0:num_cols-1]).astype(np.int64)
    
    # This function permut lines and columns of matrices A randomly
    def permut_lines_and_columns(self, list_couples_exch_lines, list_couples_exch_columns, B) :
        
        # Permute the lines of the matrix B
        for (i,j) in list_couples_exch_lines :
            exch = copy.deepcopy(B[i,:])
            B[i, :] = B[j, :]
            B[j, :] = copy.deepcopy(exch)
        
        if self.scale != "scale_rows_only_":
            # Permute the columns of the matrix B
            for (i,j) in list_couples_exch_columns :
                exch = copy.deepcopy(B[:, i])
                B[:, i] = B[:, j]
                B[:, j] = copy.deepcopy(exch)
        
        return B
    
    def gen_synthetic_instanceA(self, data_name,points_for_linear_or_poly):
        # Generate a matrix lines X columns with random integers
        #data_rvs=lambda size: np.random.randint(self.min_val, self.max_val, size=size)
        #A = sc.sparse.random(self.lines, self.columns, format='csr', 
        #                    dtype="int64", data_rvs = data_rvs)
        if data_name is None:
            A = np.random.randint(self.min_val, self.max_val, size=(self.lines, self.columns))
            A = sc.sparse.csr_matrix(A)
            lowerBoundsMatA = np.eyes((self.columns-1, self.columns-1))
            lowerBoundsMatA = sc.sparse.csr_matrix(lowerBoundsMatA)
            rhs = np.zeros((self.columns,1))
            lowerBoundsMatA = sc.sparse.hstack([lowerBoundsMatA,rhs],"csr")
            upperBoundsMatA = lowerBoundsMatA.multiply(-1)
        if data_name == "linear":
            A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs = self.gen_synthetic_linear_instA(points_for_linear_or_poly)
        if data_name == "polynomial":
            #A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs = self.gen_synthetic_polynomial_instA(self.degre,
            #                                                                                   points_for_linear_or_poly)
            A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs = self.gen_synthetic_circula_instA(self.degre,
                                                                                               points_for_linear_or_poly)
        if data_name == "knapsack":
            A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs = self.gen_synthetic_knapsack_instA()
        

        
        A = sc.sparse.csr_matrix(A)
        lowerBoundsMatA = sc.sparse.csr_matrix(lowerBoundsMatA)
        upperBoundsMatA = sc.sparse.csr_matrix(upperBoundsMatA)
        return A,lowerBoundsMatA,upperBoundsMatA, sense_ctrs
    

    def gen_synthetic_linear_instA(self,points):
        
        assert((self.lines - 1) % 2 == 0)
        A = np.zeros((self.lines, self.lines + 2),dtype=np.float64)
        lowerBoundsMatA = np.zeros((self.lines+1,self.lines + 2),dtype=np.float64)
        upperBoundsMatA = np.zeros((self.lines+1,self.lines + 2),dtype=np.float64)

        lowerBoundsMatA[0,0] = 1
        lowerBoundsMatA[1,1] = 1
        lowerBoundsMatA[1,self.lines+1] =  -499999999
        
        bound = random.randint(0,1)
        if bound == 1:
            lowerBoundsMatA[0,self.lines+1] =  1
            upperBoundsMatA[0,self.lines+1] =  -499999999
        else :
            lowerBoundsMatA[0,self.lines+1] =  -499999999
            upperBoundsMatA[0,self.lines+1] =  1
        
        upperBoundsMatA[0,0] = -1
        upperBoundsMatA[1,1] = -1
        upperBoundsMatA[1,self.lines+1] =  -499999999


        # Objective function: Sum (u_i + v_i)
        for j in range(2,self.lines + 1):
            A[0,j] = 1
        # Generate the slope and intercept of the strait line 
        mu = random.uniform(1,50)
        sigma = random.uniform(1,50)
        slope = np.random.normal(loc=mu, scale=sigma, size=1)
        print("slope = ", slope)
        if (slope < 0 and bound == 1) or (slope > 0 and bound == 0):
            slope = -slope
        intercept = np.random.normal(loc=mu, scale=sigma, size=1)

        #generate a slope * x_i + intercept + u_i - v_i = y_i
        for i in range(1,(self.lines-1)//2+1):
            mu = random.uniform(1,50)
            sigma = random.uniform(1,50)
            x_i = np.random.normal(loc=mu, scale=sigma, size=1)
            mu = slope * x_i + intercept
            y_i = np.random.normal(loc=mu, scale=100, size=1)
            points.append((x_i[0],y_i[0]))
            A[2*i-1, 0] = x_i
            A[2*i, 0] = -x_i
            A[2*i-1, 1] = 1
            A[2*i, 1] = -1
            A[2*i-1,self.lines+1] = y_i
            A[2*i,self.lines+1] = - y_i
            lower_bound_i = 0
            delta = np.round(np.random.normal(loc=0, scale=1, size=1),3)
            bound = random.randint(0,1)
            if bound == 1:
                upper_bound_i = np.round(abs(y_i-slope * x_i - intercept) + abs(delta),3)
            else :
                upper_bound_i = 499999999
                
            
            j = i - 1
            lowerBoundsMatA[2*j+2,2*j+2] = 1
            lowerBoundsMatA[2*j+3,2*j+3] = 1
            lowerBoundsMatA[2*j+2,self.lines+1] = lower_bound_i
            lowerBoundsMatA[2*j+3,self.lines+1] = lower_bound_i
            
            
            upperBoundsMatA[2*j+2,2*j+2] = -1
            upperBoundsMatA[2*j+3,2*j+3] = -1
            upperBoundsMatA[2*j+2,self.lines+1] = -upper_bound_i
            upperBoundsMatA[2*j+3,self.lines+1] = -upper_bound_i
            
        
        for i in range(1,(self.lines-1)//2+1):
            A[2*i -1,2*(i-1)+2] = 1
            A[2*i -1,2*(i-1) + 3] = -1
            A[2*i,2*(i-1)+2] = -1
            A[2*i,2*(i-1) + 3] = 1
        sense_ctrs = []
        for _ in range(self.lines - 1):
            sense_ctrs.append(0)
        print("points = ", points)
        return A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs
    
    def eval_polynomial(self,CoefsList, x_i):

        pol_value = 0
        for i in range(len(CoefsList)):
            pol_value = pol_value + CoefsList[i]*pow(x_i,i)
        return pol_value
    
    def eval_with_zeros_polynomial(self,CoefsList, x_i):

        pol_value = CoefsList[-1]
        for coef in CoefsList[:-1]:
            pol_value = pol_value * (x_i - coef)

        #pol_value = (x_i - 2)*(x_i + 3)*(x_i - 4)*(x_i - 5)
        return pol_value
    
    def gen_synthetic_polynomial_instA(self,degre,points):
        
        assert((self.lines - 1) % 2 == 0)
        n = self.lines + degre + 1
        A = np.zeros((self.lines, n),dtype=np.float64)
        lowerBoundsMatA = np.zeros((n-1,n),dtype=np.float64)
        upperBoundsMatA = np.zeros((n-1,n),dtype=np.float64)

        for i in range(degre):
            lowerBoundsMatA[i,i] = 1
            upperBoundsMatA[i,i] = -1
            
            lowerBoundsMatA[i,n-1] =  -499999999
            upperBoundsMatA[i,n-1] =  -499999999
        
        lowerBoundsMatA[degre,degre] = 1
        upperBoundsMatA[degre,degre] = -1
        bound = random.randint(0,1)
        if bound == 1:
            lowerBoundsMatA[degre,n-1] =  0.01
            upperBoundsMatA[degre,n-1] =  -499999999
        else :
            lowerBoundsMatA[degre,n-1] =  -499999999
            upperBoundsMatA[degre,n-1] =  0.01
        
        
        # Objective function: Sum (u_i + v_i)
        for j in range(degre + 1,n-1):
            A[0,j] = 1
        # Generate the coefs of the polynomial
        CoefsList = []
        for _ in range(degre):
    
            CoefsList.append(random.uniform(-7,7))
        last_coef = random.uniform(-0.5,0.5)
        if last_coef == 0:
            last_coef = random.choice(list({-1,1}))
        if (last_coef > 0 and bound == 0) or (last_coef < 0 and bound == 1) :
            last_coef = -last_coef
        CoefsList.append(last_coef)
        CoefsList = [-7.5,-2,2.5,10,15,0.0625]
        #generate a sum_i(coef*x_i^i) + u_i - v_i = y_i
        points = [(-8,200),(-7.7,500),(-6,1200),(0.5,-400),(-1,-250),(-1,0),(-8,200),(2,500),
          (7.5,900),(6,600),(-8,200),(4,250),(-2.5,250),(0,-300),(11,300),(15,-288),
          (13,-1700),(-8,200),(14,-1000),(13,-1000),(11,-500),(13,-1200),(9,500),(-4.5,700)]
        for i in range(1,len(points)+1):
        #for i in range(1,(self.lines-1)//2+1):
            mu = random.uniform(1,10)
            sigma = random.uniform(1,10)
            #x_i = np.round(np.random.normal(loc=mu, scale=sigma, size=1),1)
            x_i = points[i-1][0]
            fraction = random.uniform(0,1.75)
            #y_i = self.eval_with_zeros_polynomial(CoefsList, x_i) + random.uniform(-50,50)
            y = self.eval_with_zeros_polynomial(CoefsList, x_i)
            #y_i = y*fraction
            y_i = points[i-1][1]
            y_i = np.round(y_i,1)
            #points.append((x_i[0],y_i[0]))
            for k in range(degre + 1):
                A[2*i-1, k] = pow(x_i,k)
                A[2*i, k] = -pow(x_i,k)
                A[2*i-1,n-1] = y_i
                A[2*i,n-1] = - y_i
            lower_bound_i = 0
            
            bound = random.randint(0,1)
            if bound == 1:
                upper_bound_i = np.round(abs(y_i-y),1)+1000 + 0.75*abs(y)
            else :
                upper_bound_i = 499999999
                
            
            j = i - 1
            lowerBoundsMatA[2*j+degre + 1,2*j+degre + 1] = 1
            lowerBoundsMatA[2*j+degre + 2,2*j+degre + 2] = 1
            lowerBoundsMatA[2*j+degre + 1,n-1] = lower_bound_i
            lowerBoundsMatA[2*j+degre + 2,n-1] = lower_bound_i
            
            
            upperBoundsMatA[2*j+degre + 1,2*j+degre + 1] = -1
            upperBoundsMatA[2*j+degre + 2,2*j+degre + 2] = -1
            upperBoundsMatA[2*j+degre + 1,n-1] = -upper_bound_i
            upperBoundsMatA[2*j+degre + 2,n-1] = -upper_bound_i
            
        
        for i in range(1,(self.lines-1)//2+1):
            A[2*i -1,2*(i-1)+degre + 1] = 1
            A[2*i -1,2*(i-1) + degre + 2] = -1
            A[2*i,2*(i-1)+degre + 1] = -1
            A[2*i,2*(i-1) + degre + 2] = 1
        sense_ctrs = []
        for _ in range(self.lines - 1):
            sense_ctrs.append(0)
        print("points = ", points)
        return A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs
    
    def gen_synthetic_circula_instA(self,degre,points):
        
        assert((self.lines - 1) % 2 == 0)
        n = self.lines + 5
        A = np.zeros((self.lines, n),dtype=np.float64)
        lowerBoundsMatA = np.zeros((n-1,n),dtype=np.float64)
        upperBoundsMatA = np.zeros((n-1,n),dtype=np.float64)

        
        lowerBoundsMatA[0,0] = 1
        upperBoundsMatA[0,0] = -1
        lowerBoundsMatA[0,n-1] =  8
        upperBoundsMatA[0,n-1] =  -12

        lowerBoundsMatA[1,1] = 1
        upperBoundsMatA[1,1] = -1
        lowerBoundsMatA[1,n-1] =  15
        upperBoundsMatA[1,n-1] =  -25

        lowerBoundsMatA[2,2] = 1
        upperBoundsMatA[2,2] = -1
        lowerBoundsMatA[2,n-1] =  2
        upperBoundsMatA[2,n-1] =  -5

        lowerBoundsMatA[3,3] = 1
        upperBoundsMatA[3,3] = -1
        lowerBoundsMatA[3,n-1] =  -20
        upperBoundsMatA[3,n-1] =  11

        lowerBoundsMatA[4,4] = 1
        upperBoundsMatA[4,4] = -1
        lowerBoundsMatA[4,n-1] =  24
        upperBoundsMatA[4,n-1] =  -28
        
        
        
        
        
        # Objective function: Sum (u_i + v_i)
        for j in range(5,n-1):
            A[0,j] = 1
        # Generate the coefs of the polynomial
        CoefsList = [9,18,4,-16,25]
        
        #generate  ax^2 +bx + a'y^2 +b'y + c + u_i - v_i = constante
        
        points = self.gen_circular_points(9)
        print("len points = ", len(points))
        for i in range(1,len(points)+1):
        #for i in range(1,(self.lines-1)//2+1):
            mu = random.uniform(1,10)
            sigma = random.uniform(1,10)
            #x_i = np.round(np.random.normal(loc=mu, scale=sigma, size=1),1)
            x_i = points[i-1][0]
            fraction = random.uniform(0,1.75)
            #y_i = self.eval_with_zeros_polynomial(CoefsList, x_i) + random.uniform(-50,50)
            #y_i = self.eval_with_zeros_polynomial(CoefsList, x_i)*fraction
            y_i = points[i-1][1]
            y_i = np.round(y_i,1)
            #points.append((x_i[0],y_i[0]))
            A[2*i-1, 0] = x_i * x_i
            A[2*i, 0] = -x_i * x_i
            A[2*i-1, 1] = x_i
            A[2*i, 1] = -x_i
            A[2*i-1, 2] = y_i * y_i
            A[2*i, 2] = -y_i * y_i
            A[2*i-1, 3] = y_i
            A[2*i, 3] = -y_i
            A[2*i-1, 4] = 1
            A[2*i, 4] = -1
            A[2*i-1,n-1] = 1225
            A[2*i,n-1] = - 1225
            
            lower_bound_i = 0
            
            bound = random.randint(0,1)
            if bound == 1:
                upper_bound_i = np.round(abs(1225-self.eval_circular(CoefsList, x_i,y_i)),1)+1000000
            else :
                upper_bound_i = 499999999
                
            
            j = i - 1
            lowerBoundsMatA[2*j+5,2*j+5] = 1
            lowerBoundsMatA[2*j+6,2*j+6] = 1
            lowerBoundsMatA[2*j+5,n-1] = lower_bound_i
            lowerBoundsMatA[2*j+5,n-1] = lower_bound_i
            
            
            upperBoundsMatA[2*j+5,2*j+5] = -1
            upperBoundsMatA[2*j+6,2*j+6] = -1
            upperBoundsMatA[2*j+5,n-1] = -upper_bound_i
            upperBoundsMatA[2*j+6,n-1] = -upper_bound_i
            
        
        for i in range(1,(self.lines-1)//2+1):
            A[2*i -1,2*(i-1)+5] = 1
            A[2*i -1,2*(i-1) + 6] = -1
            A[2*i,2*(i-1)+5] = -1
            A[2*i,2*(i-1) + 6] = 1
        sense_ctrs = []
        for _ in range(self.lines - 1):
            sense_ctrs.append(0)
        print("points = ", points)
        return A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs
    
    def draw_linear_or_polynomial_function(self,data_name,points,function,inst_name_fig):
        
        print("function = ", function)
        points = [(-8,200),(-7.7,500),(-6,1200),(0.5,-400),(-1,-250),(-1,0),(-8,200),(2,500),
          (7.5,900),(6,600),(-8,200),(4,250),(-2.5,250),(0,-300),(11,300),(15,-288),
          (13,-1700),(-8,200),(14,-1000),(13,-1000),(11,-500),(13,-1200),(9,500),(-4.5,700)]
        sorted_points = sorted(points)
        print("sorted_points = ",sorted_points)
        x_line = np.linspace(sorted_points[0][0], sorted_points[-1][0], 100)
        if data_name == "linear":
            y_line = function[1] * x_line + function[2]
        if data_name == "polynomial":
            y_line = []
            for x in x_line:
                #y_line.append(self.eval_with_zeros_polynomial(function[1:],x))
                #y_line.append(self.eval_polynomial(function[1:],x))
                y_line.append(self.eval_polynomial(function[1:],x))

        plt.figure(figsize=(8, 6)) # Optional: set figure size for better visualization

        # Plot the cloud of points using plt.scatter()
        x_points = []
        y_points = []
        for point in points:
            x_points.append(point[0])
            y_points.append(point[1])
        #print("y_points = ",y_points)
        plt.scatter(x_points, y_points, label='Data Points', color='blue', alpha=0.7)

        # Plot the linear equation using plt.plot()
        if data_name == "linear":
            label="Equation: y = ax + b"
            type_line = "Linear equation"
        if data_name == "polynomial":
            label="Polynomial of degre " + str(function[0])
            type_line = "Polynomial equation"
        plt.plot(x_line, y_line, label=label, color='red', linestyle='--')
       
        # 5. Add labels, title, and legend for clarity
        plt.xlabel('X-axis')
        plt.ylabel('Y-axis')
        plt.title('Cloud of Points and ' + type_line)
        plt.legend() # Display the labels defined in scatter and plot

        # 6. Save the plot
        plt.grid(True) # Optional: add a grid for better readability
        plt.savefig(inst_name_fig)
        print("-----fin du tracer et sauvegarde")

    def eval_circular(self, CoefsList, x_i,y_i):
        
        return CoefsList[0]* x_i * x_i + CoefsList[1]* x_i + CoefsList[2]* y_i * y_i + CoefsList[3]* y_i + CoefsList[4]
    
    def gen_circular_points(self,max):
        def courbe1(x):

            return ((35*35 - 9*(x + 1)*(x + 1)) ** 0.5)/2+2

        def courbe2(x):

            return -((35*35 - 9*(x + 1)*(x + 1)) ** 0.5)/2+2

        def add_point(x,points):
            
            if (35*35 - 9*(x + 1)*(x + 1)) >= 0 :
                    points.append((x,courbe1(x)))
                    points.append((x,courbe2(x)))

        def add_cloud_points(couples,points):

            for (x,y) in  couples:
                points.append((x,y))
     
        points = []
        for i in range(max):
            x = 0.5 + i
            if (35*35 - 9*(x + 1)*(x + 1)) >= 0 :
                points.append((x,courbe1(x)))
                points.append((x,courbe2(x)))
            x = 0.5 - i
            if (35*35 - 9*(x + 1)*(x + 1)) >= 0 :
                points.append((x,courbe1(x)))
                points.append((x,courbe2(x)))
        
        add_point(9.99,points)
        add_point(-12.6,points)
        add_point(-12.65,points)
        add_point(-12.66,points)
        add_point(10.65,points)
        add_point(10.66,points)

        couples = [(-11,-2),(-11,2),(-11,7),(-10,12),(-7,-10),(-8,15),(-8,17),(5,-12),(4,15),
                (8,-5),(7,6),(10,10),(5,20)]
        add_cloud_points(couples,points)
        return points
    
    def plot_2d(self,points,function,inst_name_fig):

        points = self.gen_circular_points(9)
        sorted_points = sorted(points)
        print("sorted_points = ",sorted_points)
        print("function = ", function)
        y_points = []
        for point in sorted_points:
                y_points.append(point[1])
        y_points_sorted = sorted(y_points)

        # 2. Create a grid
        x_range = np.linspace(sorted_points[0][0], sorted_points[-1][0], 100) # 100 points in X
        y_range = np.linspace(y_points_sorted[0], y_points_sorted[-1], 100) # 100 points in Y
        X, Y = np.meshgrid(x_range, y_range)

        # 3. Calculate Z values
        Z = self.eval_circular(function[1:], X, Y)

        # 3. Create the Plot
        plt.figure(figsize=(8, 6))      

        # Plot the cloud of points using plt.scatter()
        x_points = []
        y_points = []
        for point in points:
            x_points.append(point[0])
            y_points.append(point[1])
        #print("y_points = ",y_points)
        x_points.append(-14)
        y_points.append(0)
        x_points.append(12)
        y_points.append(0)
        x_points.append(0)
        y_points.append(-16.5)
        x_points.append(0)
        y_points.append(20.5)
        print("y_points = ",y_points)
        plt.scatter(x_points, y_points, label='Data Points', color='blue', alpha=0.7)


        # 4. Plot the contour
        #plt.contourf(X, Y, Z-1225, levels=[1], cmap='viridis')
        plt.contour(X, Y, Z-1225,  levels=[1], colors='red', linestyles='--')

        # Add labels and a title
        plt.xlabel('X-axis')
        plt.ylabel('Y-axis')
        plt.title('Contour Plot with Overlayed Scatter Points')
        plt.legend()

        plt.grid(True) # Optional: add a grid for better readability
        plt.savefig(inst_name_fig)
        print("-----fin du tracer et sauvegarde")

    
    def gen_synthetic_knapsack_instA(self):

        A = np.zeros((self.lines, self.columns),dtype=np.float64)
        lowerBoundsMatA = np.zeros((self.columns-1,self.columns),dtype=np.float64)
        upperBoundsMatA = np.zeros((self.columns-1,self.columns),dtype=np.float64)

        for j in range(self.columns-1):
            A[0,j] = - round(random.uniform(0.025,2*self.columns),2)
            lowerBoundsMatA[j,j] = 1
            upperBoundsMatA[j,j] = -1
            lowerBoundsMatA[j,self.columns-1] = 0
            upperBoundsMatA[j,self.columns-1] = -1
        
        for i in range(1,self.lines):
            for j in range(self.columns-1):
                A[i,j] = - round(random.uniform(0.025,25),2)
            A[i,self.columns-1] = round(sum(A[i,:]) + random.uniform(0.025,25),2)
        
        sense_ctrs = []
        for _ in range(self.lines - 1):
            sense_ctrs.append(0)
        return A,lowerBoundsMatA,upperBoundsMatA,sense_ctrs

    # This function generate two equal or not equal matrices by permutation of lines
    # and columns. The matrices are same sized
    def generate_two_matrices_by_permut(self, lowerBoundsMatA,upperBoundsMatA,senses_ctrs, A):
        
        
        self.lines, self.columns = A.shape
        
        # Ensure an effective permutation of line and columns
        assert(self.lines >= 2)
        assert(self.columns >= 2)

        # ceate a mask B of the matrix A: 
        B = copy.deepcopy(A)

        # convert to lil_matrix for efficient manipulation of rows
        A = A.tolil()
        B = B.tolil()


        
        # instanciate the recorder of scaling
        list_scale_B = []
        if self.proportional:

            # generate scaled matrices A and B
            for i in range(self.lines):
                scale_B = random.randint(self.min_scale,self.max_scale)
                B[i,:] = copy.deepcopy(A[i,:].multiply(scale_B))
                if scale_B != 1 :
                   list_scale_B = list_scale_B + [i, scale_B]

        
            
                

        # Get the set of lines of the matrix B to permut
        list_couples_exch_lines = self.gen_random_permut(self.lines)
        #print("list_couples_exch_lines = ", list_couples_exch_lines)
         
        # Get the set of columns of the matrix B to permut
        list_couples_exch_columns = self.gen_random_permut(self.columns)
        #print("list_couples_exch_columns = ", list_couples_exch_columns)

        # Permut the lines and columns of B 
        B = self.permut_lines_and_columns(list_couples_exch_lines, list_couples_exch_columns,B)
        matPermutCols, matPermutRows = self.get_pemut_mat_col_from_list_couples_exch(self.columns,
                                                 list_couples_exch_columns)
        lowerBoundsMatB = (matPermutRows.dot(lowerBoundsMatA)).dot(matPermutCols)
        upperBoundsMatB = (matPermutRows.dot(upperBoundsMatA)).dot(matPermutCols)
        

        # get the directory in which the generated instance is saved
        _, _, _,_,set_web = self.collect_results_stats(set_inst = self.set_inst)
        directory = self.prefix_path+"data_set/"+set_web+self.set_inst+"/"+self.set_inst_test
        
        

        if not self.equal:
           # permute some positions of coefficients of a lines and columns of 
           # the matrix B in this way

           # record it
           if self.proportional :
               self.type = "not_eq_pro"
           else :
               self.type = "not_eq"
           _, _, _,type_ins_f,_ = self.collect_results_stats(type_inst = self.type)
           
           file = directory+self.scale+self.set_inst+"_"+type_ins_f+self.inst
           file0 = directory+self.scale+self.set_inst+"_"+type_ins_f

           # Get the number of coefficients to modify in B
           num_coef_modified = random.randint(0,self.lines //4)
           
          
           list_modif_coef = []
           
           for _ in range(num_coef_modified):
               while True :
                i =   random.randint(1, self.lines-1)
                j =   random.randint(0, self.columns-1)
                if (i,j) not in  list_modif_coef :
                    coef = random.randint(0,self.lines //2)
                    if B[i,j] != coef :
                          list_modif_coef.append((i,j))
                    B[i,j] = coef
                    break
           B = B.tocsr()

           # Generate a random size of the submatrix of B to permut it lines and its columns
           first_rand_index_line = random.randint(1, self.lines //2)
           assert(self.lines >= 4)
           if self.lines in [4,5,6,7] :
               max_num_of_lines = 3
           else :
               max_num_of_lines = self.lines //2
        
           num_of_lines = random.randint(3, max_num_of_lines)
           first_rand_index_column = random.randint(0, self.columns//2)
           assert(self.columns >= 4)
           if self.columns in [4,5,6,7] :
               max_num_of_columns = 3
           else :
               max_num_of_columns = (self.columns //2)-1
           
           num_of_columns = random.randint(3, max_num_of_columns)
           
           # Define the random submatrix of B to permut
           not_B = B[first_rand_index_line:first_rand_index_line + num_of_lines,
               first_rand_index_column:first_rand_index_column + num_of_columns]
           
           # Get the subset of lines of the matrix B to permut
           list_couples_exch_sublin = self.gen_random_permut(num_of_lines)
         
           # Get the subset of columns of the matrix B to permut
           list_couples_exch_subcol = self.gen_random_permut(num_of_columns)

           # Permut the lines and columns of B 
           B[first_rand_index_line:first_rand_index_line + num_of_lines,
               first_rand_index_column:first_rand_index_column + num_of_columns] = self.permut_lines_and_columns(list_couples_exch_sublin, 
                                                                                                                 list_couples_exch_subcol,not_B)
           
           # save all traces for generating the instances test matrices
           self.save_testing_matrices_in_npz(file,self.type,self.rank,A.tocsr(),"_A")
           self.save_testing_matrices_in_npz(file,self.type,self.rank,B.tocsr(),"_B")
           self.save_testing_matrices_in_npz(file,self.type,self.rank,lowerBoundsMatA,"_lowerBoundsA")
           self.save_testing_matrices_in_npz(file,self.type,self.rank,lowerBoundsMatB,"_lowerBoundsB")
            
           self.save_testing_matrices_in_npz(file,self.type,self.rank,upperBoundsMatA,"_upperBoundsA")
           self.save_testing_matrices_in_npz(file,self.type,self.rank,upperBoundsMatB,"_upperBoundsB")
           # save the modified characteristics of the instances:
           self.rank = self.rank
           characteristics = {'rank': self.rank,
                              'type_inst': self.type,
                              'set_inst': self.set_inst,
                              'set_inst_test': self.set_inst_test,
                              'inst': self.inst,
                              'list_couples_exch_lines': list_couples_exch_lines,
                              'list_couples_exch_columns': list_couples_exch_columns,
                              'first_rand_index_line': first_rand_index_line,
                              'num_of_lines':self.lines,
                              'first_rand_index_column':first_rand_index_column,
                              'num_of_columns':self.columns,
                              'list_couples_exch_sublin': list_couples_exch_sublin,
                              'list_couples_exch_subcol': list_couples_exch_subcol,
                              'list_scale_B':list_scale_B,
                              'list_modif_coef':list_modif_coef,
                              'senses_ctrs_A': senses_ctrs}
           
           pickle_file = file0+"/charac_"+self.type+"_"+self.inst+".pickle"
           self.save_any_data_format_with_pikle(characteristics,pickle_file)
        else:
            # record it
            if self.proportional :
               self.type = "eq_pro"
            else :
               self.type = "eq"
            _, _, _,type_ins_f,_ = self.collect_results_stats(type_inst = self.type)
           
            file = directory+self.scale+self.set_inst+"_"+type_ins_f+self.inst
            file0 = directory+self.scale+self.set_inst+"_"+type_ins_f

            # save all traces for generating the instances test matrices for
            # equality testing 
            self.save_testing_matrices_in_npz(file,self.type,self.rank,A.tocsr(),"_A")
            self.save_testing_matrices_in_npz(file,self.type,self.rank,B.tocsr(),"_B")
            self.save_testing_matrices_in_npz(file,self.type,self.rank,lowerBoundsMatA,"_lowerBoundsA")
            self.save_testing_matrices_in_npz(file,self.type,self.rank,lowerBoundsMatB,"_lowerBoundsB")
            
            self.save_testing_matrices_in_npz(file,self.type,self.rank,upperBoundsMatA,"_upperBoundsA")
            self.save_testing_matrices_in_npz(file,self.type,self.rank,upperBoundsMatB,"_upperBoundsB")

            # save the modified characteristics of the instances:
            characteristics = {'rank': self.rank,
                               'type_inst': self.type,
                               'type_inst': self.type,
                               'set_inst': self.set_inst,
                               'set_inst_test': self.set_inst_test,
                               'inst': self.inst,
                               'num_of_lines':self.lines,
                               'num_of_columns':self.columns,
                               'list_couples_exch_lines': list_couples_exch_lines,
                               'list_couples_exch_columns': list_couples_exch_columns,
                               'list_scale_B':list_scale_B,
                               'senses_ctrs_A': senses_ctrs}
            pickle_file = file0+"/charac_"+self.type+"_"+self.inst+".pickle"
            self.save_any_data_format_with_pikle(characteristics,pickle_file)
        
        # reconvert to csr_matrix for efficient operation on sparses matrices
        B = B.tocsr()
        lowerBoundsMatB = lowerBoundsMatB.tocsr()
        upperBoundsMatB = upperBoundsMatB.tocsr()

        return B,lowerBoundsMatB,upperBoundsMatB
    

    def save_for_dzn(self,file_,A,nameA):
        if not (A is None):    
            m,n = A.shape
            file_.write(nameA)
            file_.write(" = [|")
            for i in range(m) :
                for j in range(n) :
                    file_.write(str(A[i,j]))
                    if j != n-1:
                        file_.write(",")
                    else :
                        file_.write("|")
            file_.write("];\n\n")
    
    def save_senses_ctrs_for_dzn(self,file_,senses_ctrs):
    
        file_.write('senses = [')
        for i in range(len(senses_ctrs)):
            file_.write(str(senses_ctrs[i]))
            if i != len(senses_ctrs)-1:
                    file_.write(",")
        file_.write('];\n\n')

    def save_all(self,set_web,type_inst, inst, senses_ctrs,
             A, nameAFile, lowerBoundsMatA, upperBoundsMatA):

        directory = "data_set/"
        file = directory + set_web + "dzn/" + "matrices_" + type_inst + "/" + inst + nameAFile + ".dzn"
    
        with open(file, "w") as file_:
            m,n = A.shape
            file_.write("m = " + str(m) + "; \n")
            file_.write("n = " + str(n) + "; \n\n")
            self.save_for_dzn(file_,A,"A")
            self.save_senses_ctrs_for_dzn(file_,senses_ctrs)
            self.save_for_dzn(file_,lowerBoundsMatA,"lowerBoundsMatA")
            self.save_for_dzn(file_,upperBoundsMatA,"upperBoundsMatA")

    def gen_dzn_inst(self, scaling,set_web, type_inst,set_inst,set_inst_test,inst,
                     nameAFile,nameLowerBoundsA,nameUpperBoundsA):

        # get the instance of to matrices A and B of two linear programs in standard form
        A,senses_ctrs = self.get_testing_matrices_from_npz(type_inst,set_inst,
                                                        set_inst_test,inst,nameAFile)
        lowerBoundsMatA,_ = self.get_testing_matrices_from_npz(type_inst,set_inst,
                                                        set_inst_test,inst,nameLowerBoundsA)
                
        upperBoundsMatA,_ = self.get_testing_matrices_from_npz(type_inst,set_inst,
                                                        set_inst_test,inst,nameUpperBoundsA)
                
                
        self.save_all(set_web, type_inst, inst, senses_ctrs,
                         A, nameAFile, lowerBoundsMatA, upperBoundsMatA)
        
    def save_all_for_eq(self,file_,A, nameAFile, lowerBoundsMatA, upperBoundsMatA,
                                           nameLowerBoundsMatA, nameUpperBoundsMatA):

        self.save_for_dzn(file_,A,nameAFile)
        self.save_for_dzn(file_,lowerBoundsMatA,nameLowerBoundsMatA)
        self.save_for_dzn(file_,upperBoundsMatA,nameUpperBoundsMatA)

    def check_scaling_sets_lines(self, set_1, set_2):
        
        # set_1 and set_2 should be scipy csr vectors of integers
        #assert(type(set_1) == sc.sparse._csr.csr_matrix)
        #assert(type(set_2) == sc.sparse._csr.csr_matrix)
        #assert(set_1.dtype == "int64" and set_2.dtype == "int64")

        # get the shape of set_1
        shape_set = set_1.shape 

        # set_1 and set_2 should have the same length
        assert(shape_set == set_2.shape)

        # set_1 and set_2 are first sorted:
        sort_set_1 = copy.deepcopy(np.sort(set_1.todense(),axis=-1))     # Complexity of O(nlogn)
        sort_set_2 = copy.deepcopy(np.sort(set_2.todense(),axis=-1))

        for i in range(shape_set[1]):
            if sort_set_1[0,i] != 0 and sort_set_2[0,i] != 0:
                scale_set_1 = sort_set_1[0,i]
                scale_set_2 = sort_set_2[0,i]
                break
            elif (sort_set_1[0,i] == 0 and sort_set_2[0,i] != 0) or (sort_set_2[0,i] == 0 and sort_set_1[0,i] != 0):
                #print('iciiiii')
                #print("set_1 =", sort_set_1[0,:],"\nset_2 =",sort_set_2[0,:],"\ni = ", i)
                return (False, (False,False))
            elif sort_set_1[0,i] == 0 and sort_set_2[0,i] == 0 and i < shape_set[1]-1:
                pass
            else:
                scale_set_1 = 1
                scale_set_2 = 1


        for i in range(shape_set[1]):
            if sort_set_1[0,i] * scale_set_2 != sort_set_2[0,i] * scale_set_1 :
                #print("set_1 =", set_1,"\nset_2 =",set_2,"\ni = ", i)
                return (False, (False,False))
        # print("ici=",set_1,set_2,sort_set_1[0],sort_set_2[0])
        return (True, (scale_set_1,scale_set_2))
    
    def get_corresponding_rows_or_col(self, A, B, lowerBoundsMatA, lowerBoundsMatB):
        m, n = A.shape
        x = np.zeros((m,m),dtype=int)
        y = np.zeros((n,n),dtype=int)
        proport_coefs = []
        for i in range(m):
            proport_coefs.append([])
        for i in range(m):
            for j in range(m):
                correspond, Coefs = self.check_scaling_sets_lines(A[i,:], B[j,:])
                if correspond :
                    x[i,j] = 1
                    proport_coefs[i].append([Coefs[0],Coefs[1],i,j])
        
        # A completer avec l'algo recursif du deep first search
        y[n-1,n-1] = 1    
        for i in range(n-1):
            for j in range(n-1):
                if lowerBoundsMatA[i,n-1] == lowerBoundsMatB[j,n-1]:
                    y[i,j] = 1
        return x,y  

        
    
    # Scale coefficients not with bound value
    def scale_coef_with_no_bound_value(self, factor, A):
        m,n = A.shape
        A = A.tolil()
        for i in range(m):
            for j in range(n):
                if A[i,j]*factor > -499999999 and A[i,j]*factor < 499999999:
                    A[i,j] = A[i,j]*factor
        return A.tocsr().astype(np.int64).todense() 

    def gen_dzn_for_eq_inst(self,type_inst,inst,set_web,min_k,max_k, A, B,
               lowerBoundsMatA,upperBoundsMatA,lowerBoundsMatB,upperBoundsMatB, model_name):

        directory = "data_set/"+ set_web + "dzn_for_eq/"
        dzn_file = directory + "matrices_" + type_inst + "/" + model_name + "_" + inst + ".dzn"

        with open(dzn_file, "w") as file_:
            m,n = A.shape
            file_.write("m = " + str(m) + "; \n")
            file_.write("n = " + str(n) + "; \n")
            file_.write("min_k = " + str(min_k) + "; \n")
            file_.write("max_k = " + str(max_k) + "; \n")
            file_.write("test_scale_rows_only = 0 ; \n\n")
            if model_name == "theorem_model" :
                x,y = self.get_corresponding_rows_or_col(A, B, lowerBoundsMatA, lowerBoundsMatB)
                self.save_all_for_eq(file_,x, "x", None, None,"", "")
                self.save_all_for_eq(file_,y, "y", None, None,"", "")
            
            A = A.todense()
            B = B.todense()
            lowerBoundsMatA = lowerBoundsMatA.todense()
            lowerBoundsMatB = lowerBoundsMatB.todense()
            upperBoundsMatA = upperBoundsMatA.todense()
            upperBoundsMatB = upperBoundsMatB.todense()
            self.save_all_for_eq(file_,A, "A", lowerBoundsMatA, upperBoundsMatA,
                                                "lowerBoundsMatA", "upperBoundsMatA")
            self.save_all_for_eq(file_,B, "B", lowerBoundsMatB, upperBoundsMatB,
                                                "lowerBoundsMatB", "upperBoundsMatB")
            
    # Save the generated matrices of test for permutation equality or
    # proportionality
    def save_testing_matrices_in_npz(self,directory,type,rank,A,namA):
        
        
        
        # save the generated matrices with new name using 'num' to differ from
        # the old example
        
        rank_type = str(rank)+"_"+type
        sc.sparse.save_npz(directory+namA+rank_type+".npz",A)
        
        
        
    
    # get the num-th generated matrices of test for permutation equality or
    # proportionality
    def get_testing_matrices_from_npz(self,type_ins,set_ins,set_ins_tes, ins, namA):

    
        
        _, _, _,type_ins_f,set_web = self.collect_results_stats(type_inst = type_ins, set_inst = set_ins)
        # get the directory in which to save the results and statistics of one instance
        directory = self.prefix_path+"data_set/"+set_web+set_ins+"/"+set_ins_tes+set_ins+"_"+type_ins_f
        file = directory+ins
        

        # get the rank of the instance in the group of instances to test
        pickle_file_num = directory+"charac_"+type_ins+"_"+ins+".pickle"
        data = self.get_data_saved_in_pickle_format(pickle_file_num)
        num = data['rank']
        rank_type = str(num)+"_"+type_ins
        A = sc.sparse.load_npz(file+namA+rank_type+".npz")
        
        return A, data['senses_ctrs_A']
    
    def save_any_data_format_with_pikle(self,data_in_any_format,string_name_of_pickle_format_file):
         with open(string_name_of_pickle_format_file, "wb") as f: # "wb" for write binary
              pickle.dump(data_in_any_format, f)
    
    def get_data_saved_in_pickle_format(self,string_name_of_pickle_format_file) :
        with open(string_name_of_pickle_format_file, "rb") as f: # "rb" for read binary
             saved_data = pickle.load(f)
        return saved_data
    
    def record_characteristic(self,type_ins,set_ins,set_ins_tes, ins, characteristic,char_values):

        
        _, _, _,type_ins_f,set_web = self.collect_results_stats(type_inst = type_ins, set_inst = set_ins)
        # get the directory in which to save the results and statistics of one instance
        directory = self.prefix_path+"data_set/"+set_web+set_ins+"/"+set_ins_tes+set_ins+"_"+type_ins_f

        # get the rank of the instance in the group of instances to test
        pickle_file_num = directory+"charac_"+type_ins+"_"+ins+".pickle"
        if os.path.exists(pickle_file_num):
            data = self.get_data_saved_in_pickle_format(pickle_file_num)
            data[characteristic] = char_values
        else :
            data = {characteristic : char_values}
        
        self.save_any_data_format_with_pikle(data,pickle_file_num)
    
    # This function collect some stats of the experiment on testing equivalence between LPs.
    # It can also be used to identifie the right file in which the instances or results of
    # experiment are recorded
    def collect_results_stats(self, timeout = 0,execution_time = 0,result_ = "unknown"
                               ,type_inst = "unknown",set_inst = "unknown"):
        
        # collect the result of the test of equivalence between LP:
        if execution_time < (timeout*60) :
           yes_or_no = "no"
           if result_ =='True' or result_ == True :
              result = 'equivalent'
           elif result_ == "error occured during solving":
              result = "error occured during solving"
           elif result_ == "unknown due to timeout":
              result = "unknown due to timeout"
           elif result_ == "unknown":
              result = "unknown"
           else :
              result = 'not equivalent'
        else :
           yes_or_no = "yes"
           result = "unknown due to timeout"
        
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
        
        # identify the origin or the source of the LP instance
        if set_inst == 'collection':
           set_web = "miplib/"
        elif set_inst == 'benchmark':
           set_web = "miplib/"
        elif set_inst == 'exples':
           set_web = "synthetic/"
        elif set_inst == 'linear':
           set_web = "linear_fitting/"
        elif set_inst == 'polynomial':
           set_web = "polynomial_fitting/"
        elif set_inst == 'knapsack':
           set_web = "fractional_multidim_knapsack/"
        else :
           set_web = "netlib/"
        
        return yes_or_no, result, type_ins, type_ins_f, set_web
    
    # get the characteristics of the instance:   
    def get_chars_inst(self,type_inst,set_inst,set_inst_test,inst):
        
        # get the directory in which the generated instance is saved
        _, _, _,type_ins_f,set_web = self.collect_results_stats(type_inst = type_inst,
                                                       set_inst = set_inst)
        
        #directory0 = "/home/jovial/scratch/code_exp_postdoc_jov_/"
        directory0 = "/Users/admin/Documents/POSTDOC_MichealMorin/CODE_PostDoc/env_code_postdoc_jovial/for_fir/code_exp_postdoc_jov_/"
        directory = directory0+self.prefix_path+"data_set/"+set_web+set_inst+"/"+set_inst_test
        file0 = directory+set_inst+"_"+type_ins_f
        pickle_file = file0+"charac_"+type_inst+"_"+inst+".pickle"

        # get the characteristics to compute the number of modified coefficients
        # and scaled ctrs. 
        chars = self.get_data_saved_in_pickle_format(pickle_file)
        #num_scaled_ctrs = len(chars['list_scale_B'])
        num_scaled_ctrs = 0
        if chars['type_inst'] == "eq_pro" or chars['type_inst'] == "eq":
            num_modif_coefs = 0
            num_permut_coefs = 0
        else :
            num_modif_coefs =len(chars['list_modif_coef'])

            # compute the total number of permutation of coef
            num_permut_coefs = len(chars['list_couples_exch_sublin'])*chars['num_of_lines'] + len(chars['list_couples_exch_subcol'])*chars['num_of_columns']
   
        # get the number of permuted constraints 
        num_permut_ctrs = len(chars['list_couples_exch_lines'])

        # get the number of permuted variables
        num_permut_vars = len(chars['list_couples_exch_columns'])

        return num_permut_ctrs,num_permut_vars,num_permut_coefs,num_modif_coefs,num_scaled_ctrs

    
    # this function transforms an mps file in an augmented forms instance A,B and
    # store it in an npz format ready for testing equivalence between A and B
    def transf_mps_in_std_form(self):
        
        # read the mps file using gurobi package
        _, _, _,_,set_web = self.collect_results_stats(set_inst = self.set_inst)
        model = gp.read(self.prefix_path+"data_set/"+set_web+self.set_inst+"/"+self.inst+".mps")

        #model.optimize()

        # for an efficient manipulation of sparsity, 
        # use of lil_matrix format
        # get the constraint matrix in sparse csr_matrix format
        A = model.getA()
        #print("A00 = ", A)

        model_sense = model.getAttr("ModelSense")

        #print("model_sense = ", model_sense, gp.GRB.MINIMIZE)

        # get the right-hand side (RHS) values in list format
        rhs = model.getAttr("RHS", model.getConstrs())

        # get the objective coefficients in list format
        obj = model.getAttr("Obj", model.getVars())

        # Get the constraints senses (Sense) in list format
        senses = model.getAttr("Sense", model.getConstrs())
        #print("senses = ", senses)

        # Transform "=" ctr in two ctr of ">=" and "<=" 
        for i in range(len(senses)) :
            if senses[i] == "=" :
                senses[i] = ">"
                senses.append(">")
                rhs.append(-rhs[i])
                new_row_A = copy.deepcopy(A[i,:])
                new_row_A = new_row_A.multiply(-1)
                A = sc.sparse.vstack([A, new_row_A],"csr")

        # get coefs ctrs with the '<=' sense and transform them in the '>=' sense
        for ctr_rank in range(len(senses)):
            if senses[ctr_rank] == '<':
               A[ctr_rank, :] = A[ctr_rank, :].tolil().multiply(-1).tocsr()
               rhs[ctr_rank] = -1 * rhs[ctr_rank]
               senses[ctr_rank] = '>'

        # Get all constraints in the model
        i = 0
        for constraint in model.getConstrs():
            constraint.RHS = rhs[i]
            constraint.Sense = senses[i]
            j = 0
            for var in model.getVars():  
                model.chgCoeff(constraint, var, A[i,j])
                j = j + 1
            i = i + 1
        model.update()
        obj = model.getAttr("Obj", model.getVars())
        #print("obj = ", obj)
        #model.optimize()
        #print("A000 = ", model.getA())

        # get the standard form by concatenation with hstack and vstack
        rhs = sc.sparse.csr_matrix([0] + rhs).reshape((len(rhs)+1, 1))
        obj = sc.sparse.csr_matrix(obj)
        A = sc.sparse.hstack([sc.sparse.vstack([obj, A],"csr"),rhs],"csr")
        return A, model, senses

   
    
    
    def get_lower_upper_bound_matrices(self, model, A):
        # get lower bound matrix in sparse csr_matrix format

        m,n = A.shape
        print( "augmented rows = ", m,  "augmented cols = ", n) 
        #lowerBoundsMatA = np.zeros((n-1,n),dtype=np.int64)
        lowerBoundsMatA = np.zeros((n-1,n),dtype=np.float64)
        i = -1
        for var in model.getVars():
            i = i + 1
            lowerBound = var.getAttr('LB')
            #print("lowerBound = ", lowerBound)
            lowerBoundsMatA[i,i] = 1
            if lowerBound == float('-inf'):
                lowerBoundsMatA[i,n-1] = -499999999
                #lowerBoundsMatA[i,n-1] = lowerBound
            else :
                lowerBoundsMatA[i,n-1] = lowerBound
        lowerBoundsMatA = sc.sparse.csr_matrix(lowerBoundsMatA)
        

        # get upper bound matrix in sparse csr_matrix format
        #upperBoundsMatA = np.zeros((n-1,n),dtype=np.int64)
        upperBoundsMatA = np.zeros((n-1,n),dtype=np.float64)
        i = -1
        for var in model.getVars():
            i = i + 1
            upperBound = var.getAttr('UB')
            #print("upperBound = ", upperBound)
            upperBoundsMatA[i,i] = -1
            if upperBound == float('inf'):
                upperBoundsMatA[i,n-1] = -499999999
                #upperBoundsMatA[i,n-1] = -upperBound
            else :
                upperBoundsMatA[i,n-1] = -upperBound
        upperBoundsMatA = sc.sparse.csr_matrix(upperBoundsMatA)
        return lowerBoundsMatA, upperBoundsMatA
    

    def remove_rows_of_sparse_mat(self,rows_to_remove,csr_mat):
        # Create a boolean mask to select rows to keep
        rows_to_keep = np.ones(csr_mat.shape[0], dtype=bool)
        for row in rows_to_remove:
            rows_to_keep[row] = False

        # Apply the mask to the CSR matrix
        new_csr_mat = csr_mat[rows_to_keep]
        return new_csr_mat

    def update_main_matrix_lower_upper_bound_matrices(self,A,lowerBoundsMatA,upperBoundsMatA,senses):

        m, n = A.shape
        list_non_zeros = {}
        for i in range(1,m):
            list_non_zeros[i] = []
        for i in range(1,m):
            for j in range(n):
                if A[i,j] != 0:
                    list_non_zeros[i].append((j,A[i,j]))
            
        rows_to_remove = []
        for i in range(1,m):
            
            if len(list_non_zeros[i]) == 1 and A[i,n-1]!= 0:
                if A[i,n-1] > 0 :
                    return None, None, None
                else :
                    rows_to_remove.append(i)
            if len(list_non_zeros[i]) == 1 and A[i,n-1] == 0:
                #if senses[i-1] == '=':
                #    print("two two bounds --------0",i)
                rows_to_remove.append(i)
                j = list_non_zeros[i][0][0]
                if list_non_zeros[i][0][1] > 0 :
                    newBound = max(0,lowerBoundsMatA[j,n-1])
                    lowerBoundsMatA[j,n-1] = newBound
                    if senses[i-1] == '=':
                        upperBoundsMatA[j,n-1] = max(0,upperBoundsMatA[j,n-1])
                if list_non_zeros[i][0][1] < 0 :
                    newBound = max(0,upperBoundsMatA[j,n-1])
                    upperBoundsMatA[j,n-1] = newBound
                    if senses[i-1] == '=':
                        lowerBoundsMatA[j,n-1] = max(0,lowerBoundsMatA[j,n-1])


            if len(list_non_zeros[i]) == 2 and A[i,n-1]!= 0:
                #if senses[i-1] == '=':
                #    print("two two bounds --------1")
                rows_to_remove.append(i)
                j = list_non_zeros[i][0][0]
                if list_non_zeros[i][0][1] > 0 :
                    lowerBoundCtr = list_non_zeros[i][1][1]/list_non_zeros[i][0][1]
                    newBound = max(lowerBoundCtr,lowerBoundsMatA[j,n-1])
                    lowerBoundsMatA[j,n-1] = newBound
                    if senses[i-1] == '=':
                        upperBoundsMatA[j,n-1] = max(-lowerBoundCtr,upperBoundsMatA[j,n-1])
                if list_non_zeros[i][0][1] < 0 :
                    upperBoundCtr = list_non_zeros[i][1][1]/(-list_non_zeros[i][0][1])
                    newBound = max(upperBoundCtr,upperBoundsMatA[j,n-1])
                    upperBoundsMatA[j,n-1] = newBound
                    if senses[i-1] == '=':
                        lowerBoundsMatA[j,n-1] = max(-upperBoundCtr,lowerBoundsMatA[j,n-1])
        
        
        A = self.remove_rows_of_sparse_mat(rows_to_remove,A)
        
        return A,lowerBoundsMatA,upperBoundsMatA,rows_to_remove
    
    def get_senses_ctrs(self,senses,rows_to_remove):
        senses_ctrs0 = []
        i = 1
        for sens in senses:
            if sens in ['<', '>']:
               sens_ctr = 0
            if sens == '=':
               sens_ctr = 1
            if i not in rows_to_remove:  
                senses_ctrs0.append(sens_ctr)
            i = i + 1
        senses_ctrs = np.array(senses_ctrs0)
        return senses_ctrs
    
    def gen_associated_lp_matrix(self):

        A, model,senses = self.transf_mps_in_std_form()
        #print("A0 = ", A)
        #A = A * 1000
        #print("A = ", A)
        #print("m_int, n_int = ", A.shape)
        
        lowerBoundsMatA, upperBoundsMatA = self.get_lower_upper_bound_matrices(model, A)
        #print("lowerBoundsMatA = ", lowerBoundsMatA)
        #print("upperBoundsMatA = ", upperBoundsMatA)
        random_scales = []
        for _ in range(A.shape[0]):
            scale = random.randint(1,10)
            random_scales.append(scale)
        obj_scale = random_scales[0]
        ctrs_scales = random_scales[1:]
        #self.gen_gurobi_model(model, obj_scale, ctrs_scales) # A debugger !!!!!!

        rows_to_remove = []

        A,lowerBoundsMatA,upperBoundsMatA,rows_to_remove = self.update_main_matrix_lower_upper_bound_matrices(A,
                                                                      lowerBoundsMatA,upperBoundsMatA,senses)
        
        
        senses_ctrs = self.get_senses_ctrs(senses,rows_to_remove)
        
        return A,lowerBoundsMatA,upperBoundsMatA,senses_ctrs

class Solutions:
    def __init__(self, A, B, result, model, type_inst):

        self.solution = result.solution
        self.model = model
        self.type_inst = type_inst
        self.A = A
        self.B = B
        self.P = None
        self.Q = None
    
    def get_scaling_permutations_matrices(self):
        lines, columns =  self.A.shape
        if self.model == "quadratic_model":
            # scale the lines of matrices A and B
            for i in range(lines):
                self.A[i,:] = self.A[i,:].multiply(self.solution.k1[i])
                self.B[i,:] = self.B[i,:].multiply(self.solution.k2[i])
            
            # get the permutations matrices P and Q
            self.P = sc.sparse.csr_matrix(self.solution.P)
            self.Q = sc.sparse.csr_matrix(self.solution.Q)

        if self.model == "theorem_model":
            # scale the lines of matrices A and B
            for i in range(lines):
                self.A[i,:] = self.A[i,:].multiply(self.solution.k1[i])
                self.B[i,:] = self.B[i,:].multiply(self.solution.k2[i])

            # get the permutations matrices P and Q
            self.P = self.get_matrix_permutation_or_theorem_model(self.solution.l,lines,"lines")
            self.Q = self.get_matrix_permutation_or_theorem_model(self.solution.c,columns,"columns")
         

        if self.model in ["algo_test","enhanced_test","enhanced2_test"]:

            # get the permutations matrices P and Q
            self.P = self.solution[0].tocsr()
            self.Q = self.solution[1].tocsr()
           
    
    def get_matrix_permutation_or_theorem_model(self, l_or_c, lins_or_cols, lins_or_cols_str):
        Mat = sc.sparse.lil_matrix(np.zeros((lins_or_cols,lins_or_cols)))

        # leverage the fact that the line l_or_c[i] of A is the i_th line of B
        # or the column l_or_c[i] of A is the i_th column of B
        for i in range(lins_or_cols):
            if lins_or_cols_str == "lines":
                Mat[i, l_or_c[i]-1] = 1
            else :
                Mat[l_or_c[i]-1, i] = 1
        return Mat.tocsr()
    

    def validate_solution(self):

        if self.type_inst == "not_eq" :
            return False, "no solution to validate"
        
        assert(self.A.shape == self.B.shape)
        
        # assert that P is a permutation matrix
        #print("P = ",self.P.todense())
        for i in range(self.P.shape[0]):
            sp_i = 0
            sp_j = 0
            for j in range(self.P.shape[0]):
                sp_i = sp_i + self.P[i,j]
                sp_j = sp_j + self.P[j,i]
            assert(sp_i == 1)
            assert(sp_j == 1)
            
        #print("Q = ", self.Q.todense())
        # assert that Q is a permutation matrix
        for i in range(self.Q.shape[0]):
            sq_i = 0
            sq_j = 0
            for j in range(self.Q.shape[0]):
                sq_i = sq_i + self.Q[i,j]
                sq_j = sq_j + self.Q[j,i]
            assert(sq_i == 1)
            assert(sq_j == 1)
        

        # print("P.A[1,:]=",self.P.dot(self.A)[1,:])
        # print("Q[:,20]=", self.Q[:,20])
        PermutA = (self.P.dot(self.A)).dot(self.Q)
        # print("PermutA[:,20]=", PermutA[:,20])
        for i in range(self.A.shape[0]):
            for j in range(self.A.shape[1]):
                if PermutA[i,j] != self.B[i,j]:
                    print("PermutA",[i,j],"=",PermutA[i,j], ",","B",[i,j],"=",self.B[i,j])
                    return True, "approximate solution"
        return True, "valid solution"
    
