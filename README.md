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

    **Input G:graph represented by a boolean sparse adjacency matrix, s: source vertex id
    **Output  p: dense vector, where p[v] is the predecessor vertex on
    **           shortest path from s to v, or -1 if v is unreachable
    1: p(:) <- -1, p(s) <- s
    2: f(s) <- s      //f is the current frontier
    3: for all locality L(i, j) in paralle do
    4: while f is not NULL do
    5: TRANSPOSEVECTOR(f_ij)
    6: f_i <- AllGather(f_ij, P(:,j))
    7: t_i <- NULL
    8: for each f_i(u) != 0 do    //u is in frontier
    9:    adj(u) <- Indices(G_ij(:,u))
    10:   t_i <- t_i | Pair(adj(u),u)
    11:  t_ij <- AllToAllV(t_i, P(i,:))
    12:  for (v,u) in t_ij do
    13:   if p_ij(v) != -1 then
    14:     p_ij(v) <- u
    15:     f_ij(v) <- v
    16:   else      //remove if discovered before
    17:     t_ij <- t_ij\(u,v)


Graph500_HPX
--------------
Replace Graph500 OpenMP implementation using HPX.
