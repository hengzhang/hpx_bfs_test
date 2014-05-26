The BFS algorithm using HPX API
============

The bfs algorithm implemented using hpx API.

bfs_simple
------------
A simple version of BFS algorithm using HPX API. 

We make the point behaviors (init, traverse) components of locality, and then we use them to make bfs algorithm in multi-localities.

bfs_standalone
-------------
Copyright by Matthew Anderson. I've modify some code to fit better test.

It don't use the HPX API. I implemente it on a standalone environment.

bfs_single_locality
----------------
This algorithm is implemented in one single locality using HPX API.

bfs_localities
---------------
A multi-localities using Parallel 2D top-down algorithm.

    <b>Input</b> G:graph represented by a boolean sparse adjacency matrix, r: source vertex id
    <b>Output</b> p: dense vector, where p[v] is the predecessor vertex on shortest path from s to v, or -1 if v is unreachable
    1: p(:) <- -1, p(s) <- s
    2: f(s) <- s      //f is the current frontier
    3: for all locality L(i, j) in paralle do
    4: while f is not NULL do
    5: TRANSPOSEVECTOR(fij)
