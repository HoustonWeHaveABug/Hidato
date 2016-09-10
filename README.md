# Hidato
Hidato solver with sample grids

Solution to challenge https://www.reddit.com/r/dailyprogrammer/comments/4zog32/20160826_challenge_280_hard_free_flow_solver/

The solver is a backtracker written in C, at each step it selects the most constrained number among those that remain to be put on the grid (i. e. the value that fits in the smallest number of cells).

The first solution found is displayed, at the end of execution the search space size (nodes) and the total number of solutions found are displayed.
