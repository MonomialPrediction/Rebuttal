# Rebuttal
This is the repository for the rebuttal phase of of our paper submitted to Asiacrypt 2020.

## Contents

1.  [Code for recovering the superpoly for 840-, 841- and 842-round Trivium](https://github.com/MonomialPrediction/Rebuttal/tree/master/cube)
2. [Code for compute the exact degree of Trivium up to 834 rounds](https://github.com/MonomialPrediction/Rebuttal/tree/master/Degree)
3. [A Detailed description of the new detection algorithm in Section 6.4](https://github.com/MonomialPrediction/Rebuttal/blob/master/Detailed_New_Detection_Alg.pdf)
4. [The answer to the question about the PoolSearchMode of Gurobi](https://github.com/MonomialPrediction/Rebuttal/blob/master/PoolSearchMode.md)
5. [A demo for PoolSearchMode by a + b + c + d + e + f = 1 example](https://github.com/MonomialPrediction/Rebuttal/blob/master/demo_for_reviewer_a%2Bb%2Bc%2Bd%2Be%2Bf.py)

 ## Usage of the Codes in Cube and Degree

### Dependencies

To run our code, you should first install the [Gurobi solver](https://www.gurobi.com) and set the proper license. 

### Compile 

After you install the solver, then you need to edit Makefile to modify --lgurobixx to your own version. Then type 

`make`

to compilen our code.

### Run the Code

#### Compute the Exact Degree

After you compile the code, just type 

`./trivium`

to compute the degree of Trivium up to 834 rounds.

#### Recover the Superpoly

After you compile the code, type 

`./trivium [ROUND] [INDEX]`  

The possible combinations of (ROUND, INDEX) are listed as follows, 
1. ROUND = 840, INDEX = 1:
    Recover the superpoly for [0,1,...,79]/{70, 72, 74, 76, 78} of 840-round Trivium

2. ROUND = 840, INDEX = 2:
    Recover the superpoly for [0,1,...,79]/{72, 74, 76, 78} of 840-round Trivium

3. ROUND = 840, INDEX = 3:
    Recover the superpoly for [0,1,...,79]/{70, 74, 76, 78} of 840-round Trivium

4. ROUND = 841, INDEX = 1:
    Recover the superpoly for [0,1,...,79]/{70, 72, 76, 78} of 841-round Trivium

5. ROUND = 841, INDEX = 2:
    Recover the superpoly for [0,1,...,79]/{72, 76, 78} of 841-round Trivium

6. ROUND = 842, INDEX = 1:
    Recover the superpoly for [0,1,...,79]/{72, 74, 76, 78} of 842-round Trivium

7. ROUND = 842, INDEX = 2:
    Recover the superpoly for [0,1,...,79]/{74, 76, 78} of 842-round Trivium

