
This page explains how to launch the experiments designed described in **Automated Equivalence of Linear Programs by Permutation and Scaling: Problem Definition and Exact Methods**.

  

## Introduction
  

Each problem instance consists in checking the equivalence or the non-equivalence of two linear programs with the same number of variables and constraints. The two linear programs are represented by two matrices, a ground truth matrix and a testing matrix, where the coefficients of the first rows belong to the objective function and the coefficients of remaining rows belong to the constraints. Each instance is dubbed by a name in source code and is classified in a specific directory depending on whether the linear problems are equivalent or not.

  

The instances are in **scipy** `.npz` files classified in four categories of linear problems:

-  **network flow** problems:
   - The dataset for equivalence is inside the directory `dataset/netlib/lp/lp_npz2/lp_equal_matrices`

		> An example of instance is the **afiro** instance with the two **scipy**  `.npz` matrices:
        `afiro_A1_eq.npz` and `afiro_B1_eq.npz`

    -  The dataset for non-equivalence is inside the directory `dataset/netlib/lp/lp_npz2/lp_not_equal_matrices`

		> An example of instance is the **afiro** instance with the two **scipy**  `.npz` matrices:
        `afiro_A1_not_eq.npz` and `afiro_B1_not_eq.npz`

-  **linear fitting** problems:
   - The dataset for equivalence is inside the directory `dataset/linear_fitting/linear/linear_npz/linear_equal_matrices`

		> An example of instance is the **Mat_linear3** instance with the two **scipy**`.npz` matrices:
        `Mat_linear3_A3_eq.npz` and `Mat_linear3_B3_eq.npz`

    - The dataset for non-equivalence is inside the directory `dataset/linear_fitting/linear/linear_npz/linear_not_equal_matrices`

		> An example of instance is the **Mat_linear3** instance with the two **scipy**  `.npz` matrices:
        `Mat_linear3_A3_not_eq.npz` and `Mat_linear3_B3_not_eq.npz`
  
-  **polynomial fitting** problems:
    - The dataset for equivalence is inside the directory `dataset/polynomial_fitting/polynomial/polynomial_npz/polynomial_equal_matrices`

		> An example of instance is the **Mat_polynomial3** instance with the two **scipy**  `.npz` matrices:
        `Mat_polynomial3_A3_eq.npz` and `Mat_polynomial3_B3_eq.npz`

    - The dataset for non-equivalence is inside the directory `dataset/polynomial_fitting/polynomial/polynomial_npz/polynomial_not_equal_matrices`

		> An example of instance is the **Mat_polynomial3** instance with the two **scipy**  `.npz` matrices:
         `Mat_polynomial3_A3_not_eq.npz` and `Mat_polynomial3_B3_not_eq.npz`

-  **fractional multidimensional knapsack** problems:
    - The dataset for equivalence is inside the directory `dataset/fractional_multidim_knapsack/knapsack/knapsack_npz/knapsack_equal_matrices`

		> An example of instance is the **Mat_knapsack3** instance with the two **scipy**  `.npz` matrices:
       `Mat_knapsack3_A3_eq.npz` and `Mat_knapsack3_B3_eq.npz`

    - The dataset for non-equivalence is inside the directory `dataset/fractional_multidim_knapsack/knapsack/knapsack_npz/knapsack_not_equal_matrices`

		> An example of instance is the **Mat_knapsack3** instance with the two **scipy**  `.npz` matrices:
        ` Mat_knapsack3_A3_not_eq.npz` and `Mat_knapsack3_B3_not_eq.npz`

  

## How to get the source code and the dataset?

  

Make sure you have the required dependencies (or equivalent):
- **C++, CMake 4.0, Eigen**
- **Python 3.11.9, pandas, scipy, gurobipy, numpy**
- **MiniZincIDE 2.9.3** with the solvers **Gecode, Or-Tools, Chuffed, Coinbc** available at https://www.minizinc.org/downloads/

Then, download the source code and the dataset at https://github.com/JovialCheukam/linearprogequiv2026_code

The download should contain the two folders: 

- `equivalence_linear_program_algo`
- `equivalence_linear_program_model`



## How to use the source code and the commands to check equivalence between two linear programs from the dataset?

To check equivalence between two linear programs, there ae two types of commands: 

1. the command to use algorithmic procedures
2. the command to use declarative methods

### Algorithmic procedure
  
      
The algorithmic procedures are **Algo**, **AlgoEnhanced** and **Luby** methods. To run them:

- Open a **Terminal**
- Set the current directory to `equivalence_linear_program_algo/EquivLinProg` which contains the `CMakeLists.txt` file.
- In `CMakeLists.txt`, replace the string **< Home Directory >** by your own home directory (or the directory where the code was extracted).
- Set the current directory to `equivalence_linear_program_algo/EquivLinProg/build`
- Excute the configuration command `cmake ..`
- Execute the build command `cmake --build .`
- Execute the equivalence checking command `./EquivLinProg ` with the following parameters (in order):

	- Type of algorithm : `algo` or `algo_enhanced`

	- Timeout in minutes : a positif integer

	- Type of instance :
        1. If it is the search of equivalence : `eq`
        2. If it is the search of non-equivalence : `not_eq`

	- The site web directory :
       1. For the network flow problem, the site web directory is `netlib/`
       2. For the linear fitting problem, the site web directory is `linear_fitting/`
       3. For the polynomial fitting problem, the site web directory is `polynomial_fitting/`
       4. For the knapsack problem, the site web directory is `fractional_multidim_knapsack/`

	- The problem directory :
      1. For the network flow problem, the problem directory is `lp`
      2. For the linear fitting problem, the problem directory is `linear`
      3. For the polynomial fitting problem, the problem directory is `polynomial`
      4. For the knapsack problem, the problem directory is `knapsack`

	- The instances directory :
       1. For the network flow problem, the instances directory is `lp_npz2/`
       2. For the linear fitting problem, the instances directory is `linear_npz/`
       3. For the polynomial fitting problem, the instances directory is `polynomial_npz/`
       4. For the knapsack problem, the instances directory is `knapsack_npz/`

	- The name of the instance :
       1. For the network flow problem, an example of name of an intance is `afiro`
       2. For the linear fitting problem, an example of name of an intance is `Mat_linear3`
       3. For the polynomial fitting problem, an example of name of an intance is `Mat_polynomial3`
       4. For the knapsack problem, an example of name of an intance is `Mat_knapsack4`

	- The multiplicative constante of Luby : an integer
        1. If it is the Luby algorithm method used, the first parameter which is the the type of algorithm should be `algo_enhanced`.
        2. If it it is not the Luby algorithm method used, the multiplicative constante of Luby is 0

	- The random seed for the Luby method : an integer
       1. If it it is not the Luby algorithm method used, any integer is correct

	- The flag value if it is a Luby method or not :
       1. If it is a Luby method the parameter is `luby`
       2. If it is not a Luby method the parameter is `not_luby`

  

Here are example of complete commands:
 ```
./EquivLinProg algo_enhanced 2880 eq netlib/ lp lp_npz2/ afiro 0 9 not_luby

./EquivLinProg algo 2880 eq netlib/ lp lp_npz2/ adlittle 0 9 not_luby

./EquivLinProg algo_enhanced 2880 eq netlib/ lp lp_npz2/ afiro 1024 9 luby
```

  
### Declarative methods

The declarative methods use two constraints programming models named **TheoMod** and **QuadraMod**. To run a suitable solver on these models, proceed as follows:

- Set the current directory to `equivalence_linear_program_model`
- In `model_mat_permut.py`, replace the string **< MiniZinc Directory >** by the absolute path to the directory where **MiniZinc** is installed
- Execute the equivalence checking command `python3 run_one_thread.py ` with the following parameters (in order):

	- The type of method :
       1. If the method is **TheoMod**, the parameter is `theorem_model`
       2. If the method is **QuadraMod**, the parameter is `quadratic_model`

	- The solver used among the four availables and which are : `cp-sat`, `chuffed`, `gecode` and `coinbc`

	- The maximum scale of rows of the matrices of the linear program of the instance : a positif integer

	- Timeout in minutes : a positif integer

	- Type of instance : 
	   1. If it is the search of equivalence, the parameter is `eq`
       2. If it is the search of non-equivalence, the parameter is `not_eq`

	- The problem directory :
       1. For the network flow problem, the problem directory is `lp`
       2. For the linear fitting problem, the problem directory is `linear`
       3. For the polynomial fitting problem, the problem directory is `polynomial`
       4. For the knapsack problem, the problem directory is `knapsack`

	- The instances directory :
       1. For the network flow problem, the instances directory is `lp_npz2/`
       2. For the linear fitting problem, the instances directory is `linear_npz/`
       3. For the polynomial fitting problem, the instances directory is `polynomial_npz/`
       4. For the knapsack problem, the instances directory is `knapsack_npz/`

	- The name of the instance :
       1. For the network flow problem, an example of name of an intance is `afiro`
       2. For the linear fitting problem, an example of name of an intance is `Mat_linear3`
       3. For the polynomial fitting problem, an example of name of an intance is `Mat_polynomial3`
       4. For the knapsack problem, an example of name of an intance is `Mat_knapsack4`

	- The ID number of the instance : an arbitrary integer

Here are examples of complete commands:
```
python3 run_one_thread.py quadratic_model cp-sat 1 30 eq linear linear_npz/ Mat_linear3 1

python3 run_one_thread.py theorem_model chuffed 1 30 not_eq knapsack knapsack_npz/ Mat_knapsack3 1
```

The results of the test are recorded as a `.pickle` file in the above directory of the dataset.
