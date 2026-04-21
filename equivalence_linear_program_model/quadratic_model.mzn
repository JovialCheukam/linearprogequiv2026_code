% Verify that matrices A and B are the same up to rows and columns 
% permutations with scaling on individual rows.
% Unfeasible will be returned if there exist no vP and vQ matrices that
% can transform A in to B.
% In this example, A and B are int, so is vX. 

%include "data_set/netlib/dzn_for_eq/matrices_eq_pro/quadratic_model_afiro.dzn";


int: n;
int: m;
int: min_k;
int: max_k;
int: test_scale_rows_only;

% First and second matrices
array[1..m,1..n] of int: A;
array[1..m,1..n] of int: B;

% Bound matrices
array[1..n-1,1..n] of int: lowerBoundsMatA;
array[1..n-1,1..n] of int: upperBoundsMatA;
array[1..n-1,1..n] of int: lowerBoundsMatB;
array[1..n-1,1..n] of int: upperBoundsMatB;


% Permutation matrix for the rows
array[1..m,1..m] of var {0,1}: vP;
             
% Permutation matrix for the columns
array[1..n,1..n] of var {0,1}: vQ;

% Intermediate result after a permutation of the rows
array[1..m,1..n] of var int: vX;

% Intermediate result after a permutation of the rows of Bound matrices
array[1..n-1,1..n] of var int: interLowerBoundsMat;
array[1..n-1,1..n] of var int: interUpperBoundsMat;

% Scaling of the rows
%array[1..m] of var min_k..max_k: vS1;
%array[1..m] of var min_k..max_k: vS2;
array[1..m] of var 1..1: vS1;
array[1..m] of var 1..1: vS2;
var bool: rows_only;
           
% Permutation of the rows
constraint forall(i in 1..m, j in 1..n)(
sum(k in 1..m)(vP[i,k] * A[k,j] * vS1[k]) = vX[i,j]
);

% Permutation of the rows of Bound matrices
%constraint forall(i in 1..n-1, j in 1..n)(
%sum(k in 1..n-1)(vQ[i,k] * lowerBoundsMatA[k,j]) = interLowerBoundsMat[i,j]
%);

%constraint forall(i in 1..n-1, j in 1..n)(
%sum(k in 1..n-1)(vQ[i,k] * upperBoundsMatA[k,j]) = interUpperBoundsMat[i,j]
%);

constraint forall(i in 1..n-1, j in 1..n-1)(vQ[i,j] == 1 -> lowerBoundsMatA[j,n] == lowerBoundsMatB[i,n]);
constraint forall(i in 1..n-1, j in 1..n-1)(vQ[i,j] == 1 -> upperBoundsMatA[j,n] == upperBoundsMatB[i,n]);

% Permutation of the columns with scaling
constraint bool2int(rows_only) = test_scale_rows_only;
constraint if not rows_only then forall(i in 1..m, j in 1..n)(
sum(k in 1..n)(vX[i,k] * vQ[k,j]) = B[i,j] * vS2[i]
) else forall(i in 1..m, j in 1..n)(vX[i,j] * vS1[i] = B[i,j] * vS2[i]) endif;

% Permutation of the columns of Bound matrices
%constraint forall(i in 1..n-1, j in 1..n)(
%sum(k in 1..n)(interLowerBoundsMat[i,k] * vQ[k,j]) = lowerBoundsMatB[i,j]
%);

%constraint forall(i in 1..n-1, j in 1..n)(
%sum(k in 1..n)(interUpperBoundsMat[i,k] * vQ[k,j]) = upperBoundsMatB[i,j]
%);

% The rows and columns of the row-permutation matrix should sum to 1
constraint forall(j in 1..m)(
sum(i in 1..m)(vP[i,j]) = 1
);
constraint forall(i in 1..m)(
sum(j in 1..m)(vP[i,j]) = 1
);

% The rows and columns of the column-permutation matrix should sum to 1
constraint if not rows_only then forall(j in 1..n)(
sum(i in 1..n)(vQ[i,j]) = 1
) else true endif ;
constraint if not rows_only then forall(i in 1..n)(
sum(j in 1..n)(vQ[i,j]) = 1
) else true endif ;

% The scaling should be higher than 0.0
constraint forall(i in 1..m)(vS1[i] >= 0);
constraint forall(i in 1..m)(vS2[i] >= 0);

solve satisfy;

output [
        "k1 = \n" ++ concat([show(vS1[i]) ++ " " | i in 1..m]) ++ "\n\n" 
        ++ "k2 = \n" ++ concat([show(vS2[i]) ++ " " | i in 1..m]) ++ "\n\n"
        ++ "P =  \n" ++ concat([concat([show(vP[r,c]) ++ " " | r in 1..m]) ++ "\n"| c in 1..m]) ++ "\n\n" 
        ++ "Q =  \n" ++ concat([concat([show(vQ[r,c]) ++ " " | r in 1..n]) ++ "\n"| c in 1..n]) ++ "\n\n"
    ];