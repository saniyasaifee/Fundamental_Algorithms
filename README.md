# Fundamental_Algorithms
1. Shortest_path_Dijkstaras _algorithm_visibility_graph.c

The aim of this project is to create a program that takes a set of segment obstacles as
input, as well as a start and target point, and shows the shortest path between them. It
provides a visual feedback by drawing the points, obstacles, and the path on the screen
using the xlib interface.
The program takes one command-line argument, a file name. The following lines give
the set of line-segment obstacles in the format:
S (20,100) (55,63)

The point coordinates are integers. You can parse these obstacles easily with scanf. There
are less than 1000 obstacles.

As first stage, your read the input, and display the line-segments in a window. Then
you get two left mouseclick events, which give the start and target point of the shortest
path. Then you construct a graph; the vertices are the two points given by the
mouseclicks, and all end points of obstacles. Any pair (p; q) of these points forms an edge
of the graph if the line segment pq is not intersected by any of the obstacle line segments;
if (p; q) is an edge, then its length is the euclidean distance of p and q. Finally you run
a shortest-path algorithm on this graph, and display the resulting path on the screen.
Repeat this as long as the user inputs point pairs with left mouseclicks; when you receive
a right mouseclick, end the program.


2. Quadtree_Based_Path_Planning_Algorithm.c
The aim of this project is to create a program that takes a set of axis-aligned rectangle obstacles, 
and the start and final position of a disc-shaped robot, and finds a path that
moves the robot from start to target. The obstacles and the movement of the robot are
shown on the screen using the xlib interface; the robots motion is shown by showing his
position after suitable discrete timesteps.
The program takes one command-line argument, a file name. The following lines give
the set of rectangular obstacles in the format
R (20,100) (55,160)
The coordinates are integers; they are the coordinates of the lower left and upper right
endpoint. There are less than 100 obstacles. The path must be inside a 700 by 700
square.
As first stage, your read the input, and display the obstacles in a window. Then you
get two left mouseclicks which specify the initial stage of the disc (center and radius), and
a third mouseclick which gives the final position (the center). You compute and display
the motion that moves the initial to the final stage.
The configuration space of the robot are the centerpoints for which the disc is completely
inside the 700 by 700 square; to find the path, we subdivide this region (like a
quadtree). A square of the configuration space is red if each disc with a center in that
square intersects some obstacle; it is green if no disc with a center in that square intersects
some obstacle; it is yellow if it is neither green nor red. Only yellow discs need to
be subdivided. We stop the subdivision if we found a green path from the start position
to the target position, or if all yellow squares have a sidelength less than five.

3. K_Server_Problem.c
The aim of this project is to model an instance of the K-server problem for three servers with input 
given from screen. Initially the servers are located at the top left corner(yellow), top right (red) 
and middle of the bottom line (blue). The user specifies a sequence of request points by left mouseclicks 
in the window, terminated by a right mouseclick. After each mouseclick show a strategy of your choice 
(in thin yellow/red/blue lines) then after the final righ mouseclick compute and display the optimal 
strategy(in thick yellow/red/blue lines). Your program then prints the total length of both strategies 
and the competitiveness ration that you achieved.
Apply Dynamic programming type argument: If the input sequence was n request points long (p1,....,pn) 
you create a nxnxnxn array; the entry cost[t][i][j][k] is the cost of the cheapest sequence of moves 
that starts at the given starting position and serves the request upto time step t, and ends up with 
the yellow server at pi, the red server at pj, and the blue server at pk. One of IJK must be t, since 
the last request at pt was served, all other table entries have cost infinity.Any possible table entry 
must have been achieved by moving one of the three servers from a position at step t-1.

4. Approximation_Algorithm.c
The aim of this project is to approximate the shortest tree that joins all segments of a given arrangement of 
horizontal and vertical segments. The arrangement
will be given as an input file (as command line argument), with lines of the format
    v x, y1, y2
for a vertical line going from (x, y1) to (x, y2), and
    h y, x1, x2
for a horizontal line going from (x1, y) to (x2, y).
Your tree uses only edges along these segments.Computing this tree is known to be NP-complete, so you are 
asked to create your own approximation algorithm. 
For comparison, you have to compute the following lower bound: you define a graph Gh with the horizontal segments 
as vertices, and join two of these vertices by an edge if they are intersected by the same vertical segment; the 
length of this edge is their distance along the segment. Similar you define a graph Gv on the vertical segments. 
Then the length of the minimum spanning tree of Gh is a lower bound for the length of vertical connections any 
solution must have, and thelength of the minimum spanning tree of Gv is a lower bound for the length of horizontal 
connections any solution must have; their sum is a lower bound for the total length of any solution.
You compute both minimum spanning trees with any MST algorithm. Then you show on the screen (using xlib) the set of 
segments, your solution in red, and (with smaller width, so they don’t interfere with the lines of your solution) 
sets of segment pieces corresponding to the horizontal MST in green and the vertical MST in blue. Also you print the 
length of your solution, and the lower bound, and their ration on the screen.