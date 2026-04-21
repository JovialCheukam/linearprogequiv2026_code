% Verify that matrices A and B are the same up to rows and columns 
% permutations with scaling on individual rows.
% In this example, A and B are int matrices. 

include "globals.mzn";
%include "data_set/netlib/dzn_for_eq/matrices_eq_pro/theorem_model_afiro.dzn";

int: n;
int: m;
int: min_k;
int: max_k;
int: test_scale_rows_only;


% First and second matrices
array[1..m,1..n] of int: A;
array[1..m,1..n] of int: B;

array[1..m,1..n] of var int: A1;
array[1..m,1..n] of var int: B1;

% Bound matrices
array[1..n-1,1..n] of int: lowerBoundsMatA;
array[1..n-1,1..n] of int: upperBoundsMatA;
array[1..n-1,1..n] of int: lowerBoundsMatB;
array[1..n-1,1..n] of int: upperBoundsMatB;

% Equality for the rows
array[1..m, 1..m] of {0,1}: x;
array[1..m] of var 1..m : q;

% Equality for the columns
array[1..n, 1..n] of {0,1}: y;
array[1..n] of var 1..n : r;

% Scaling of the rows
%array[1..m] of var min_k..max_k: k1;
%array[1..m] of var min_k..max_k: k2;
array[1..m] of var 1..1: k1;
array[1..m] of var 1..1: k2;
var bool: rows_only;

% Scaling A and B
constraint forall(i in 1..m, j in 1..n)(A[i,j]*k1[i] = A1[i,j]);
constraint forall(i in 1..m, j in 1..n)(B[i,j]*k2[i] = B1[i,j]);

% The i-th line of B is permuted to the j-th line of A 
%constraint forall(i in 1..m, j in 1..m)(roots(B1[i,1..n],1..n ,array2set(A1[j,1..n])) <-> x[i,j]==1);
constraint forall(i in 1..m, j in 1..m)( x[i,j]== 0 -> q[i] != j );
constraint forall(i in 1..m, j in 1..m)( q[i] == j -> x[i,j]== 1);
constraint forall(i in 1..m)(sum(j in 1..m)(x[i,j]*(q[i]==j)) = 1);
constraint forall(j in 1..m)(sum(i in 1..m)(x[i,j]*(q[i]==j)) = 1);


% The i-th column of B is permuted to the j-th column of A
constraint bool2int(rows_only) = test_scale_rows_only;
%constraint if not rows_only then (forall(i in 1..n, j in 1..n)(roots(B1[1..m,i],1..m, array2set(A1[1..m,j])) <-> y[i,j]==1))
%            else true endif;
constraint if not rows_only then forall(i in 1..n, j in 1..n)( y[i,j]== 0 -> r[i] != j )  else true endif;
constraint if not rows_only then forall(i in 1..n, j in 1..n)( r[i] == j -> y[i,j]== 1)  else true endif;
constraint if not rows_only then forall(i in 1..n)(sum(j in 1..n)(y[i,j]*(r[i]==j)) = 1) else true endif;
constraint if not rows_only then forall(j in 1..n)(sum(i in 1..n)(y[i,j]*(r[i]==j)) = 1) else true endif;
    
% Link between x,y,q,r 
constraint forall(i in 1..m, j in 1..m)(q[i] == j -> (x[i,j] == 1 /\ x[i,q[i]] == 1));
constraint forall(i in 1..n, j in 1..n)(r[i] == j -> (y[i,j] == 1 /\ y[i,r[i]] == 1));

% The i-th line of lowerBoundsMatB (resp. upperBoundsMatB) is permuted to the r_i-th line of 
% lowerBoundsMatA (resp. upperBoundsMatA) and
% the j-th column of lowerBoundsMatB (resp. upperBoundsMatB) is permuted to the r_j-th column of 
% lowerBoundsMatA (resp. upperBoundsMatA)
constraint forall(i in 1..n-1, j in 1..n)(lowerBoundsMatB[i,j] = lowerBoundsMatA[r[i],r[j]]);
constraint forall(i in 1..n-1, j in 1..n)(upperBoundsMatB[i,j] = upperBoundsMatA[r[i],r[j]]);

constraint alldifferent(q);
constraint alldifferent(r);

% The values of l are the first m non zero integers:
constraint sort(q,1..m);
constraint sort(r,1..n);

% The sum of components of q is m(m+1)/2
constraint sum(i in 1..m)(q[i]) = m*(m+1) div 2;

% The sum of components of r is n(n+1)/2
constraint sum(i in 1..n)(r[i]) = n*(n+1) div 2;

% constraint for Prop 5 of the theorem
constraint forall(i in 1..m, j in 1..n)(B[i,j]*k2[q[i]] = k1[q[i]] * A[q[i],r[j]] /\ x[i,q[i]] = 1 /\ y[j,r[j]] = 1);
constraint forall(i in 1..m, j in 1..n)((x[i,q[i]] == 1 /\ y[j,r[j]] == 1) <-> (A1[q[i],r[j]] == B1[i,j]));
constraint sum(i in 1..m, j in 1..n)(A1[q[i],r[j]] == B1[i,j]) >= m*n;
constraint sum(i in 1..m, j in 1..n)(x[i,q[i]] == 1 /\ y[j,r[j]] == 1) >= m*n;


% The scaling should be higher than 0
constraint forall(i in 1..m)(k1[i] >= 0);
constraint forall(i in 1..m)(k2[i] >= 0);

solve satisfy;

output [
        "k1 = \n" ++ concat([show(k1[i]) ++ " " | i in 1..m]) ++ "\n\n" 
        ++ "k2 = \n" ++ concat([show(k2[i]) ++ " " | i in 1..m]) ++ "\n\n"
        ++ "l =  \n" ++ concat([show(q[i]) ++ " " | i in 1..m]) ++ "\n\n" 
        ++ "c =  \n" ++ concat([show(r[i]) ++ " " | i in 1..n])  ++ "\n\n"
    ];