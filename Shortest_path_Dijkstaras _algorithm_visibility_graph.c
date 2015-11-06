/*
 ============================================================================
 Name        : HW1_SaniyaSaifee.c
 Author      : Saniya Saifee
 Description : Given a set of line segment obstacles create a visibility graph
 				and find shortest path using start and target vertex and 
 				display the path on screen
 ============================================================================
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Example Window";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;
/*previous array used to keep track of shortest path from source to vertex*/
int *previous;
/*distanceFromSrc array will hold the shortest distance from source vertex to all vertices in graph*/
int *distanceFromSrc;
/*visitedVeretex array will keep track of visited vertices while finding shortest path from source to target*/
int *visitedVeretex; 

# define MAXOBSTACLES 1000
# define MAXVERTICES 2000

typedef struct {
	int x;
	int y;
}Point;
/*for co-ordinates*/
typedef struct {
	Point point1;
	Point point2;
}Obstacle;

int checkIntersection(Point p, Point q, Point r, Point s);
int getOrientation(Point p1, Point p2, Point p3);
int checkOnSegment(Point p, Point q, Point r);
int calEuclDistance(Point, Point);

int main(int argc, char **argv)
{
	FILE *fp;
	fp = fopen(argv[1], "r");
	Obstacle *obstacle;
	//array to store graph vertices
	Point *point;
	/*for dijkstra's algorithm visibility graph.*/
	int **adjacencyList;
	/*allocating memory to array of vertices*/
	point = (Point *) malloc(MAXVERTICES * sizeof(Point));
	if(point == NULL){
		fprintf(stderr, "out of memory\n");
		exit(0);
	}
	/*allocating memory to array of line segment obstacles*/
	obstacle = (Obstacle *)malloc(MAXOBSTACLES * sizeof(Obstacle));
	if(obstacle == NULL){
		fprintf(stderr, "out of memory\n");
		exit(0);
	}
	int oc =0;
	int vertex = 1;
	int a,b,c,d;
	int i = 0;
	Point startVertex;
	Point targetVertex;
	int buttonPressCount=0;
	/*reading obstacles from file*/
	while(fscanf(fp, "S (%d,%d) (%d,%d)\n", &a, &b, &c, &d)==4){
      	obstacle[oc].point1.x = a;
    	obstacle[oc].point1.y = b;
    	obstacle[oc].point2.x = c;
    	obstacle[oc].point2.y = d;
    	Point p1 = obstacle[oc].point1;
    	Point p2 = obstacle[oc].point2; 
    	point[vertex] = p1;
    	point[++vertex] = p2;
    	++oc;
    	++vertex;
    }
    adjacencyList = malloc((vertex+1) * sizeof(int *));
    if(adjacencyList == NULL){
		fprintf(stderr, "out of memory\n");
		exit(0);
	}
	for(i = 0; i <=vertex; i++){
		adjacencyList[i] = malloc((vertex+1) * sizeof(int));
		if(adjacencyList[i] == NULL){
			fprintf(stderr, "out of memory\n");
			exit(0);
		}
	}
  	/* opening display: basic connection to X Server */
  	if( (display_ptr = XOpenDisplay(display_name)) == NULL ){ 
  		printf("Could not open display. \n"); exit(-1);
  	}
  	printf("Connected to X server  %s\n", XDisplayName(display_name) );
  	screen_num = DefaultScreen( display_ptr );
  	screen_ptr = DefaultScreenOfDisplay( display_ptr );
  	color_map  = XDefaultColormap( display_ptr, screen_num );
  	display_width  = DisplayWidth( display_ptr, screen_num );
  	display_height = DisplayHeight( display_ptr, screen_num );

  	printf("Width %d, Height %d, Screen Number %d\n", 
           display_width, display_height, screen_num);
  	/* creating the window */
  	border_width = 10;
  	win_x = 0; win_y = 0;
  	win_width = display_width/2;
  	win_height = (int) (win_width / 1.7); /*rectangular window*/
  
  	win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num),
                            win_x, win_y, win_width, win_height, border_width,
                            BlackPixel(display_ptr, screen_num),
                            WhitePixel(display_ptr, screen_num) );
  	/* now try to put it on screen, this needs cooperation of window manager */
   size_hints = XAllocSizeHints();
   wm_hints = XAllocWMHints();
   class_hints = XAllocClassHint();
   if( size_hints == NULL || wm_hints == NULL || class_hints == NULL ){ 
		printf("Error allocating memory for hints. \n"); exit(-1);
	}
	size_hints -> flags = PPosition | PSize | PMinSize  ;
  	size_hints -> min_width = 60;
  	size_hints -> min_height = 60;
	
	XStringListToTextProperty( &win_name_string,1,&win_name);
  	XStringListToTextProperty( &icon_name_string,1,&icon_name);
  
  	wm_hints -> flags = StateHint | InputHint ;
  	wm_hints -> initial_state = NormalState;
  	wm_hints -> input = False;

  	class_hints -> res_name = "x_use_example";
  	class_hints -> res_class = "examples";

  	XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
                    size_hints, wm_hints, class_hints );

  	/* what events do we want to receive */
  	XSelectInput( display_ptr, win, 
            ExposureMask | StructureNotifyMask | ButtonPressMask );
  
  	/* finally: put window on screen */
  	XMapWindow( display_ptr, win );

  	XFlush(display_ptr);

  	/* create graphics context, so that we may draw in this window */
  	gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
  	XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
  	XSetLineAttributes( display_ptr, gc, 1, LineSolid, CapRound, JoinRound);
  	/* and three other graphics contexts, to draw in yellow and red and grey*/
  	gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
  	XSetLineAttributes(display_ptr, gc_yellow, 6, LineSolid,CapRound, JoinRound);
  	if( XAllocNamedColor( display_ptr, color_map, "yellow", &tmp_color1, &tmp_color2 ) == 0 ){
  		printf("failed to get color yellow\n"); exit(-1);
  	} 
  	else
    	XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );
  	gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
  	XSetLineAttributes( display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound);
  	if( XAllocNamedColor( display_ptr, color_map, "red", &tmp_color1, &tmp_color2 ) == 0 )
    {
    	printf("failed to get color red\n"); exit(-1);
    } 
  	else
    	XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
  	gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
  	if( XAllocNamedColor( display_ptr, color_map, "light grey", &tmp_color1, &tmp_color2 ) == 0 )
    {
    	printf("failed to get color grey\n"); exit(-1);
    } 
  	else
    	XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );

  	/* and now it starts: the event loop */
  	while(1)
    { 
    	XNextEvent( display_ptr, &report );
    
  
      	switch( report.type)
		{
			case Expose:
          		/* (re-)draw the example figure. This event happens
             	each time some part ofthe window gets exposed (becomes visible) */
             	for(i=0;i<oc;i++){
        			XDrawLine(display_ptr, win, gc_red, obstacle[i].point1.x, obstacle[i].point1.y, obstacle[i].point2.x, obstacle[i].point2.y);
				}
          	break;
        	case ConfigureNotify:
          		/* This event happens when the user changes the size of the window*/
          		win_width = report.xconfigure.width;
          		win_height = report.xconfigure.height;
          	break;
        	case ButtonPress:
          		/* This event happens when the user pushes a mouse button. I draw
            	a circle to show the point where it happened, but do not save 
            	the position; so when the next redraw event comes, these circles
	    		disappear again. */
          		{  
             		
             		if (report.xbutton.button == Button1 ){
             			int x, y;
             			buttonPressCount++;
             			if(buttonPressCount == 1){
  	     					startVertex.x = report.xbutton.x;
             				startVertex.y = report.xbutton.y;
             				point[0] = startVertex;
             				XDrawPoint(display_ptr,win,gc_red, startVertex.x, startVertex.y);
             			}
             			if(buttonPressCount ==2){
             				buttonPressCount = 0;
  	     					targetVertex.x = report.xbutton.x;
             				targetVertex.y = report.xbutton.y;
             				/*point array incluse all the vertices including the start and target vertices*/
             				point[vertex] = targetVertex;
             				XDrawPoint(display_ptr,win,gc, targetVertex.x ,targetVertex.y);
             				/*Calling createGraph() method that will create the visibilty graph*/
             				createGraph(point, obstacle, vertex,oc,adjacencyList);
             				/*Calling dijkstra() method that will find the shortest path between start and target vertices*/
             				dijkstra(adjacencyList, vertex+1);
             				/* Displaying shortest path on screen*/
							int j = previous[vertex];
							for(i=vertex; j!=-1; j=previous[i]){
								XDrawLine(display_ptr, win, gc, point[i].x, point[i].y, point[j].x, point[j].y);
								i=j;
							}
             			}
             			
	        		}
             		else{
	        		   XDestroyWindow(display_ptr, win);
    				   XCloseDisplay(display_ptr);
    				                          
					}	
				}	
				
          	break;
        	default:
	  			/* this is a catch-all for other events; it does not do anything.
             	One could look at the report type to see what the event was */ 
          	break;
		}
		
	}
  exit(0);
}

int createGraph(Point *point, Obstacle *obstacle, int verticesNum, int obstacleNum, int **adjacencyList){
	int i =0;
	int j = 0;
	int k=0;
	int vgIndex = 0;
	int x, y;
	int **visitedSegments;
	visitedSegments = malloc((verticesNum+1) * sizeof(int *));
	if(visitedSegments == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(0);
	}
	for(i = 0; i <=verticesNum; i++)
	{
		visitedSegments[i] = malloc(verticesNum * sizeof(int));
		if(visitedSegments[i] == NULL)
		{
			fprintf(stderr, "out of memory\n");
			exit(0);
		}
	}
	for(x = 0; x <=verticesNum; x++) {
    	for(y = 0; y<=verticesNum; y++){
    		visitedSegments[x][y] = 0;
    		adjacencyList[x][y] = -1;//Initially there is no edge between any vertices
    	}
	}	
	for(i=0;i<=verticesNum;i++){
		for(j=0; j<=verticesNum; j++){
			int blocked = 0;
			if(i!=j &&(visitedSegments[i][j]==0)){
				for(k = 0; k < obstacleNum; k++){
					Point p = point[i];
					Point q = point[j];
					Point r = obstacle[k].point1;
					Point s = obstacle[k].point2;
					//to avoid adding obstacles in visibilty graph
					if(p.x==r.x && p.y==r.y && q.x==s.x && q.y==s.y){
						visitedSegments[i][j] = 1;
				    	visitedSegments[j][i] = 1;
				    	blocked = 1;
						break;
					}
					int doIntersect = checkIntersection(p, q, r, s);
					if(doIntersect == 1){
						blocked =1;
						break;
					}
					
				}
				/*add edge to adjacencyList if it doesn't intersect any obstacle*/
				if(blocked == 0){
					int distance = calEuclDistance(point[i], point[j]);
					adjacencyList[i][j] = adjacencyList[j][i]=distance;
					visitedSegments[i][j] = 1;
					visitedSegments[j][i] = 1;
				}
			}
		}
		
	}
    return 1;
}
/* Check if edge pq intersect obstacle rs*/
int  checkIntersection(Point p, Point q, Point r, Point s){
	int orient1 = getOrientation(p,q,r);
	int orient2 = getOrientation(p, q, s);
	int orient3 = getOrientation(r,s,p);
	int orient4 = getOrientation(r, s, q);
	if(((orient1>0 && orient2<0) || (orient1<0 && orient2>0)) && ((orient3>0 && orient4<0) || (orient3<0 && orient4>0))){
		return 1;
	}
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (orient1 == 0 && checkOnSegment(p, q, r)) 
    	return 1;
 
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (orient2 == 0 && checkOnSegment(p, q, s)) 
    	return 1;
 
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (orient3 == 0 && checkOnSegment(r,s,p)) 
    	return 1;
 
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (orient4 == 0 && checkOnSegment(r,s,q)) 
    	return 1;
 
    return 0; // Doesn't fall in any of the above cases
}

int getOrientation(Point p1, Point p2, Point p3){
	int determinant = p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
	return determinant;
}

/*Given three colinear points p, q, r, the function checks if point q lies on line segment 'pr'*/
int checkOnSegment(Point p, Point q, Point r){
	int minX = 0;
	int maxX = 0;
	int maxY = 0;
	int minY = 0;
	if(p.x<q.x){
		minX = p.x;
		maxX = q.x;
	}
	else{
		minX = q.x;
		maxX = p.x;
	}
	if(p.y<q.y){
		minY = p.y;
		maxY = q.y;
	}
	else{
		minY = q.y;
		maxY = p.y;
	}	
	if((r.x>minX && r.x<maxX) && (r.y>minY && r.y<maxY)){
		return 1;
	}
	return 0;
}
/*This function calcualte euclidean distance between two vertices p1 and p2 */
int calEuclDistance(Point point1, Point point2){  
	int diffx = point1.x - point2.x;
	int diffy = point1.y - point2.y;
	int diffx_sqr = pow(diffx,2);
	int diffy_sqr = pow(diffy,2);
	double distance = sqrt(diffx_sqr + diffy_sqr);
	return (int)distance;
}

/* This function apply Dijkstaras algorithm to the visibilty graph
   passed in the form of 2D adjacencyList and find the shortest path
   between start and target vertex.*/
void dijkstra(int **adjacencyList, int vertices)
{
	 distanceFromSrc = (int *)malloc(sizeof(int)*vertices);
     previous = (int *)malloc(sizeof(int)*vertices);
     visitedVeretex = (int *)malloc(sizeof(int)*vertices);
	 int i = 0;
     int u =0;
     for (i = 0; i <vertices; i++){
        distanceFromSrc[i] = INT_MAX; 
        visitedVeretex[i] = 0;
        previous[i] = -1; 
 	 }
     // Distance of source vertex from itself is always 0
     distanceFromSrc[0] = 0;
 
     // Find shortest path for all vertices
     int count = 0;
     for (count = 0; count <vertices; count++)
     {
     	/*find the vertex with minimum distance value, from the set of vertices not 
     	yet visited*/
        u = -1;
		for (i = 0; i <vertices; ++i)
			if (!visitedVeretex[i] && ((u == -1) || (distanceFromSrc[i] < distanceFromSrc[u])))
				u = i;
			// Mark the picked vertex as visited
	    	visitedVeretex[u] = 1;
 	    int v = 0;
        // Update dist value of the adjacent vertices of the visited vertex 'u'.
       for (v = 0; v <vertices; v++){
      		/* Update dist[v] only if v is not visited and there is an edge from
         	 u to v, and total distance from src to  v through u is
         	 smaller than current value of dist[v]*/
      		 if (adjacencyList[u][v]!=-1)
				if (distanceFromSrc[u] + adjacencyList[u][v] < distanceFromSrc[v]) {
					distanceFromSrc[v] = distanceFromSrc[u] + adjacencyList[u][v];
					previous[v] = u;
				}
			}
     }
 }