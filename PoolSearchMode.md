# Is the `PoolSearchMode` reliable?

This is indeed an important question since several previous works (e.g., [HLM+20]) also used PSM. Firstly, we take Gurobi to generate the exact ANFs of TRIVIUM from 1 to 300 rounds, which have been fully verified by a SAGE script. Secondly, we confirmed with Zonghao Gu, the co-founder of Gurobi, that for models with *only* binary / integer variables, Gurobi can count the exact number of all solutions. For models with continuous variables, the results may be unreliable. In fact, We suspect that Gurobi implements the method from IPCO 2007 (Generating Multiple Solutions for MIP problems). We are still trying to confirm this with the Gurobi support team. We noted that one reviewer encountered situations where the same solution is counted twice due to rounding errors. We would be very grateful if the reviewer can offer us some sample models after this rebuttal.

Besides, one can practically verify the specific superpolies we obtained by the following procedure:

1. For a monomial x^u with n monomial trails x^u \rightsquigarrow z, call Gurobi to output the set  X of all solutions with |X| = n
2. Verify the solutions one by one outside the Gurobi context (like verifying differential characteristics)
3. Add constraints to eliminate all solutions in X
4. If the resulting model is infeasible, we compute its Irreducible Infeasible Subsystem (IIS, a short evidence that can be verified manually). If the IIS is valid, the original model indeed contains exactly n solutions. 

Finally, the code for the a+b+c+d+e+f example can be found at https://github.com/MonomialPrediction/Rebuttal. 
