# Description
Program for automated creation of train schedules.
[Link to the project website](https://www.st.fmph.uniba.sk/~kovalov3/)

# Building

- get or-tools for your OS
- replace `src/or-tools` with your version
- cd into `src/build`
- run `cmake ..`
- run `make clean && make`

# Running
Run `src/build/bin/RailSheduler`.
Program will need a file with text description of the track graph and routes you want to run in it.

## graph
Graph is represented as an adjacency list and a "conflict" list. The first number in the file is always the number of verticies in the graph. Verticies are represented as int64 numbers.

Conflicts are a feature that will make the scheduler to avoid scheduling two trains on two "conflicting" tracks(verticies of the graph) at the same time. This is useful to represent some more complex junctions like the parallel crossings.

Here is an example of a vertex 7:

```
7
2 10 11
1 3
```

In this case, it is adjacent to 10 and 11, and is in conflict with vertex 3. Lists must be lead by their size.

## routes
Normal routes are simply some path in the track graph. Routes also have a property called "overlap". It indicates how much time will it take to leave the previous vertex. So, if the overlap is 5, and route goes from vertex 1 to 2 and it must spend 10 in 1 and 15 in 2, then in the final schedule train on this route will occupy vertex 1 for \[0, 15\] and 2 for \[10, 25\].

```
5
3
1 15
4 10
3 20
12 25
11 100
```
The route has a length of 5 and overlap 3, goes through 1, 4, 3, 12, 11. Numbers after vertex numbers are time that the train must spend in the track(e.g. how long would it take to leave it).

Route descriptions must be lead by the amount of routes in the file.

## optroutes

optroutes are a simple description of the start point and destination. The program will automatically decide which path to take.

```
7
12
6 
```
This route goes from track 12 to 6 and has overlap of 7.

optroutes descriptions must be lead by the amount of optroutes in the file.
