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


