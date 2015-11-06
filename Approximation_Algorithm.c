/*
 ============================================================================
 Name        : HW4_SaniyaSaifee.c
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
#include <stdlib.h>

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
char *win_name_string = "Orthogonal Corridor MST";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_blue, gc_red, gc_green;
unsigned long valuemask = 0;
XGCValues gc_values, gc_blue_values, gc_red_values, gc_green_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;
/*previous array used to keep track of shortest path from source to vertex*/
int *previous;
/*distanceFromSrc array will hold the shortest distance from source vertex to all vertices in graph*/
int *distanceFromSrc;
/*visitedVeretex array will keep track of visited vertices while finding shortest path from source to target*/
int *visitedVeretex;
int **adjacencyList;
int **adjacencyList_h;
int **adjacencyList_v;
# define MAXSEGMENTS 1000
# define MAXVERTICES 1000

typedef enum segment_type {
	VERTICAL, HORIZONTAL
} seg_type_t;
typedef struct {
	int x;
	int y;
}Point;
/*for co-ordinates*/
typedef struct {
	Point point1;
	Point point2;
	seg_type_t type;
}Segment;
typedef struct {
	Segment segment;
	int seg_no;
}h_segment;
typedef struct {
	Segment segment;
	int seg_no;
}v_segment;
typedef struct{
	Point point;
	int h_t;
	int v_t;
}IntersectionPoint;

int checkIntersection(Point p, Point q, Point r, Point s);
int getOrientation(Point p1, Point p2, Point p3);
int checkOnSegment(Point p, Point q, Point r);
Point LineSegmentIntersection(Point p, Point q, Point r, Point s);
int IsPointInBoundingBox(int x1, int y1, int x2, int y2, int px, int py);
int minKey(int key[], int mstSet[]);
int checkIntersection(Point p, Point q, Point r, Point s);
void createLowerBoundGraph();
int calEuclDistance(Point point1, Point point2);
void primMST();
void createHorizontalGraph();
void createVerticalGraph();
void kruskalMST_Horizontal();
void findIntersectonPoint_hg(int h1, int h2,int mst_h[]);
void kruskalMST_Vertical();
void findIntersectonPoint_vg(int v1, int v2);
int find(int i, int parent[]);
int uni(int i,int j, int parent[]);

int vertical_segment_count=0;
int horizontal_segment_count=0;
Segment *segments;
IntersectionPoint *intersection_point;
v_segment *v_seg_t;
h_segment *h_seg_t;
int i_count = 0;
int * ip_segments;
int oc =0;
int mincost_gh=0;
int mincost_gv=0;
int mincost_opt=0;
float ratio;
int total;
int main(int argc, char **argv)
{
	FILE *fp;
	fp = fopen(argv[1], "r");
	//array to store graph vertices
	/*allocating memory to array of line segment obstacles*/
	segments = (Segment*) malloc(MAXSEGMENTS * sizeof(Segment));
	if(segments == NULL){
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	intersection_point = (IntersectionPoint*)malloc(MAXVERTICES * sizeof(IntersectionPoint));
	if(intersection_point == NULL){
		fprintf(stderr, "out of memory\n");
		exit(0);
	}

	int vertex = 1;
	char a[2];
	int b,c,d;
	int i = 0, j=0;
	/*reading obstacles from file*/
	while(fscanf(fp, "%s %d,%d,%d\n", a, &b, &c, &d)==4){
		if(strcmp(a,"v")==0){
			segments[oc].point1.x = b;
			segments[oc].point1.y = c;
			segments[oc].point2.x = b;
			segments[oc].point2.y = d;
			segments[oc].type = VERTICAL;
			vertical_segment_count++;
		}
		if(strcmp(a,"h")==0){
			segments[oc].point1.x = c;
			segments[oc].point1.y = b;
			segments[oc].point2.x = d;
			segments[oc].point2.y = b;
			segments[oc].type = HORIZONTAL;
			horizontal_segment_count++;
		}

    	++oc;

    }
	v_seg_t = (v_segment*)malloc(vertical_segment_count * sizeof(v_segment));
	h_seg_t = (h_segment*)malloc(horizontal_segment_count * sizeof(h_segment));
	ip_segments	= (int*)malloc(oc * sizeof(int));
	if(ip_segments == NULL){
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	for(i=0; i<oc; i++){
		ip_segments[i] =0;
	}
	int h=0, v=0;
	for(i=0; i<oc; i++){
		if(segments[i].type == VERTICAL){
			v_seg_t[v].segment = segments[i];
			v_seg_t[v].seg_no = i;
			v++;
		}
		else if(segments[i].type == HORIZONTAL){
			h_seg_t[h].segment = segments[i];
			h_seg_t[h].seg_no = i;
			h++;
		}
	}

	for(i=0;i<horizontal_segment_count;i++){
		for(j=0; j<vertical_segment_count; j++){
			Point p = h_seg_t[i].segment.point1;
			Point q = h_seg_t[i].segment.point2;
			Point r = v_seg_t[j].segment.point1;
			Point s = v_seg_t[j].segment.point2;

			int doIntersect = checkIntersection(p, q, r, s);
			if(doIntersect){
				Point ip = LineSegmentIntersection(p, q, r, s);
				if(ip.x!=-1 && ip.y!=-1){
					intersection_point[i_count].point = ip;
					intersection_point[i_count].h_t = i;
					intersection_point[i_count].v_t = j;
					i_count++;
				}
			}
		}
	}
	adjacencyList = (int**)malloc((i_count) * sizeof(int *));
	if(adjacencyList == NULL){
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	for(i = 0; i <i_count; i++){
		adjacencyList[i] = (int*)malloc((i_count) * sizeof(int));
		if(adjacencyList[i] == NULL){
			fprintf(stderr, "out of memory\n");
			exit(0);
		}
	}
	adjacencyList_h = (int**)malloc((horizontal_segment_count) * sizeof(int *));
	if(adjacencyList_h == NULL){
		fprintf(stderr, "out of memory\n");
		exit(0);
	}
	for(i = 0; i <horizontal_segment_count; i++){
		adjacencyList_h[i] = (int*)malloc((horizontal_segment_count) * sizeof(int));
		if(adjacencyList_h[i] == NULL){
			fprintf(stderr, "out of memory\n");
			exit(0);
		}
	}
	adjacencyList_v = (int**)malloc((vertical_segment_count) * sizeof(int *));
	if(adjacencyList_v == NULL){
		fprintf(stderr, "out of memory\n");
		exit(0);
	}
	for(i = 0; i <vertical_segment_count; i++){
		adjacencyList_v[i] = (int*)malloc((vertical_segment_count) * sizeof(int));
		if(adjacencyList_v[i] == NULL){
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
                            0,0,700,700, border_width,
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
  	gc_green = XCreateGC( display_ptr, win, valuemask, &gc_green_values);
  	XSetLineAttributes(display_ptr, gc_green, 1, LineSolid,CapRound, JoinRound);
  	if( XAllocNamedColor( display_ptr, color_map, "green", &tmp_color1, &tmp_color2 ) == 0 ){
  		printf("failed to get color ee\n"); exit(-1);
  	}
  	else
    	XSetForeground( display_ptr, gc_green, tmp_color1.pixel );
  	gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
  	XSetLineAttributes( display_ptr, gc_red, 4, LineSolid, CapRound, JoinRound);
  	if( XAllocNamedColor( display_ptr, color_map, "red", &tmp_color1, &tmp_color2 ) == 0 )
    {
    	printf("failed to get color red\n"); exit(-1);
    }
  	else
    	XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
    
  	gc_blue = XCreateGC( display_ptr, win, valuemask, &gc_blue_values);
    XSetLineAttributes( display_ptr, gc_blue, 1, LineSolid, CapRound, JoinRound);
  	if( XAllocNamedColor( display_ptr, color_map, "blue", &tmp_color1, &tmp_color2 ) == 0 )
    {
    	printf("failed to get color blue\n"); exit(-1);
    }
  	else
    	XSetForeground( display_ptr, gc_blue, tmp_color1.pixel );
    
   /* createLowerBoundGraph();
    XFlush(display_ptr); 
    total = mincost_gh + mincost_gv;
    printf("\n\tOptimal minimum spanning tree cost = %d\n",mincost_opt);
    printf("\n\tLower bound for length of vertical connections MST(Gh) = %d\n",mincost_gh);
    printf("\n\tLower bound for length of horizontal connections MST(Gv) = %d\n",mincost_gv);
    printf("\n\tTotal Lower bound for MST(Gh)+ MST(Gv) = %d\n",mincost_gh+mincost_gv);
    ratio= (float)mincost_opt/(float)total;
    printf("\n\t the optimal MST to lower bound ratio =%f\n",ratio);*/
   
    
  	/* and now it starts: the event loop */
  	while(1)
    {
    	XNextEvent( display_ptr, &report );


      	switch( report.type)
		{
			case Expose:
                for(i=0;i<oc;i++){
                    XDrawLine(display_ptr, win, gc, segments[i].point1.x, segments[i].point1.y, segments[i].point2.x, segments[i].point2.y);
                }
                total=0;
                mincost_opt = 0;
                mincost_gh=0;
                mincost_gv=0;
                createLowerBoundGraph();
                total = mincost_gh + mincost_gv;
                printf("\n\tOptimal minimum spanning tree cost = %d\n",mincost_opt);
                printf("\n\tLower bound for length of vertical connections MST(Gh) = %d\n",mincost_gh);
                printf("\n\tLower bound for length of horizontal connections MST(Gv) = %d\n",mincost_gv);
                printf("\n\tTotal Lower bound for MST(Gh)+ MST(Gv) = %d\n",mincost_gh+mincost_gv);
                ratio= (float)mincost_opt/(float)total;
                printf("\n\t the optimal MST to lower bound ratio =%f\n",ratio);
            break;
            
        	default:
	  			/* this is a catch-all for other events; it does not do anything.
             	One could look at the report type to see what the event was */
          	break;
		}

	}
 //exit(0);
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
	if((r.x>=minX && r.x<=maxX) && (r.y>=minY && r.y<=maxY)){
		return 1;
	}
	return 0;
}
Point LineSegmentIntersection(Point p, Point q, Point r, Point s){
	float intersection_X, intersection_Y;
	Point ip;
	ip.x = -1;
	ip.y = -1;
	int x1 = p.x;
	int y1 = p.y;
	int x2 = q.x;
	int y2 = q.y;
	int x3 = r.x;
	int y3 = r.y;
	int x4 = s.x;
	int y4 = s.y;
	int d = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
	if (d == 0)
		return ip;
	else{
		intersection_X = ((x3-x4)*(x1*y2-y1*x2)-(x1-x2)*(x3*y4-y3*x4))/d;
		intersection_Y = ((y3-y4)*(x1*y2-y1*x2)-(y1-y2)*(x3*y4-y3*x4))/d;
	}
	if(IsPointInBoundingBox(p.x, p.y, q.x, q.y, intersection_X, intersection_Y) == 1 && IsPointInBoundingBox(r.x, r.y, s.x, s.y, intersection_X, intersection_Y) == 1){

		ip.x = (int)intersection_X;
		ip.y = (int)intersection_Y;
		return ip;
	}
   	return ip;
}

int IsPointInBoundingBox(int x1, int y1, int x2, int y2, int px, int py){
	int left, top, right, bottom; // Bounding Box For Line Segment
	// For Bounding Box
	if(x1 < x2){
		left = x1;
		right = x2;
	}
	else{
		left = x2;
		right = x1;
	}
	if(y1 < y2){
		top = y1;
		bottom = y2;
	}
	else{
		top = y1;
		bottom = y2;
	}
	if( (px+0.01) >= (float)left && (px-0.01) <= (float)right && (py+0.01) >= (float)top && (py-0.01) <= (float)bottom ){
		return 1;
	}
	else
		return 0;
}
 void createLowerBoundGraph(){
	 int w,x,y,z;
	 int l=0;
	 IntersectionPoint *ip_temp = 0;
	 for(x = 0; x <i_count; x++) {
		for(y = 0; y<i_count; y++){
			adjacencyList[x][y] = -1;//Initially there is no edge between any vertices
		}
	 }
	 for(x = 0; x <i_count; x++) {
		for(y = 0; y<i_count; y++){
            if(x!=y){
                IntersectionPoint ip1 = intersection_point[x];
                IntersectionPoint ip2 = intersection_point[y];
                if(ip1.h_t == ip2.h_t || ip1.v_t == ip2.v_t){
                    int distance = calEuclDistance(ip1.point, ip2.point);
                    //printf("ip1.point.x = %d,ip1.point.y = %d \n", ip1.point.x, ip1.point.y);
                    //printf("ip2.point.x = %d,ip2.point.y = %d \n", ip2.point.x, ip2.point.y);
                    adjacencyList[x][y] = distance;
                    adjacencyList[y][x] = distance;
                    //printf("adjacencyList[%d][%d] = %d \n", x,y,adjacencyList[x][y]);
                }
            }
		}
	 }
     /*for(x = 0; x <i_count; x++) {
         for(y = 0; y<i_count; y++){
             printf("adjacencyList[%d][%d] = %d \n", x,y,adjacencyList[x][y]);
         }
     }*/
	 primMST();
     createHorizontalGraph();
	 

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

void primMST()
{
    //printf("i_count=%d\n", i_count);
    int parent[i_count]; // Array to store constructed MST
    int key[i_count];   // Key values used to pick minimum weight edge in cut
    int mstSet[i_count];  // To represent set of vertices not yet included in MST
    int vd[i_count];
    int vs[i_count];
    int i,j,u,v,count,a,b,c, d;
    // Initialize all keys as INFINITE
    for (i = 0; i < i_count; i++)
        key[i] = INT_MAX;
    for (i = 0; i < i_count; i++)
        mstSet[i] = 0;
    for (i = 0; i < i_count; i++)
        parent[i] = -2;
    for (i = 0; i < i_count; i++)
        vd[i] = 0;
    for (i = 0; i < i_count; i++)
        vs[i] = 0;
    // Always include first 1st vertex in MST.
    key[0] = 0;     // Make key 0 so that this vertex is picked as first vertex
    parent[0] = -1; // First node is always root of MST
    int degree = 0;
    // The MST will have V vertices
    for (count = 0; count < i_count; count++)
    {
        // Pick the minimum key vertex from the set of vertices
        // not yet included in MST
        u = minKey(key, mstSet);
        // Add the picked vertex to the MST Set
        if(u!=-1){
            int a = h_seg_t[intersection_point[u].h_t].seg_no;
            int b = v_seg_t[intersection_point[u].v_t].seg_no;
            // printf("a=%d b=%d, mst[%d] = %d mst[%d] = %d\n",a,b,a, mstSet[a], b,mstSet[b]);
            mstSet[u] = 1;
           // mstSet[b] = 1;
            // Update key value and parent index of the adjacent vertices of
            // the picked vertex. Consider only those vertices which are not yet
            // included in MST
            for (v = 0; v < i_count; v++){
                // adjacencyList[u][v] is non zero only for adjacent vertices of m
                // mstSet[v] is false for vertices not yet included in MST
                // Update the key only if adjacencyList[u][v] is smaller than key[v]
                c = h_seg_t[intersection_point[v].h_t].seg_no;
                d = v_seg_t[intersection_point[v].v_t].seg_no;
                if (adjacencyList[u][v]!= -1 && mstSet[v] == 0 && adjacencyList[u][v] <  key[v]){
                    parent[v]  = u, key[v] = adjacencyList[u][v];
                }
            }
        }
    }
    for (i = 1; i < i_count; i++){
        degree = 0;
        for(j=1;j<i_count;j++){
            if(i!=j){
                if(parent[j]==i){
                    degree++;
                    vd[i]= degree;
                }
            }
        }
    }
    for (i = 1; i < i_count; i++){
        for(j=1;j<i_count;j++){
            if(i!=j){
                if(intersection_point[i].h_t==intersection_point[parent[i]].h_t){
                    a = intersection_point[i].v_t;
                    b = intersection_point[j].v_t;
                    c = intersection_point[parent[i]].v_t;
                    d = intersection_point[parent[j]].v_t;
                    if((key[i]==key[j])&&((a == b && c == d)||(a==d && b==c))){
                       if(vd[i]==0)
                           key[i]=0;
                        else if(vd[j]==0)
                            key[j]=0;
                    }
                }
                else if(intersection_point[i].v_t==intersection_point[parent[i]].v_t){
                    a = intersection_point[i].h_t;
                    b = intersection_point[j].h_t;
                    c = intersection_point[parent[i]].h_t;
                    d = intersection_point[parent[j]].h_t;
                    if((key[i]==key[j])&&((a == b && c == d)||(a==d && b==c))){
                        if(vd[i]==0)
                           key[i]=0;
                        else if(vd[j]==0)
                           key[j]=0;
                    }
                }
            }
        }
    }
    for (i = 1; i < i_count; i++){
        if(key[i]!=0){
            XDrawLine(display_ptr, win, gc_red, intersection_point[i].point.x, intersection_point[i].point.y, intersection_point[parent[i]].point.x, intersection_point[parent[i]].point.y);
            //printf("parent[%d]=%d\n", i,parent[i]);
            mincost_opt = mincost_opt+ calEuclDistance(intersection_point[i].point,intersection_point[parent[i]].point);
        }
       
    }
    //printf("\nOptimal Minimum Spanning Tree cost = %d\n",mincost_opt);
   
}

int minKey(int key[], int mstSet[]){
    // Initialize min value
    int v,c,d;
    int min = INT_MAX, min_index =-1;
    for (v = 0; v < i_count; v++){
        c = h_seg_t[intersection_point[v].h_t].seg_no;
        d = v_seg_t[intersection_point[v].v_t].seg_no;
        if (mstSet[v] == 0  && key[v] < min)
            min = key[v], min_index = v;
    }
    return min_index;
}

 void createHorizontalGraph(){
	 int w,x,y,z;
	 int l=0;
	 IntersectionPoint *ip_temp = 0;
	 for(x = 0; x < horizontal_segment_count; x++) {
		for(y = 0; y< horizontal_segment_count; y++){
			adjacencyList_h[x][y] = INT_MAX;//Initially there is no edge between any vertices
		}
	 }
	 for(x = 0; x <i_count; x++) {
		for(y = 0; y<i_count; y++){
			IntersectionPoint ip1 = intersection_point[x];
			IntersectionPoint ip2 = intersection_point[y];
			if(ip1.v_t == ip2.v_t && ip1.h_t!=ip2.h_t){
				int distance = calEuclDistance(ip1.point, ip2.point);
				adjacencyList_h[ip1.h_t][ip2.h_t] = distance;
				adjacencyList_h[ip2.h_t][ip1.h_t] = distance;
			}
		}
	 }
     kruskalMST_Horizontal();
     createVerticalGraph();
     
 }

void createVerticalGraph(){
    int w,x,y,z;
    int l=0;
    IntersectionPoint *ip_temp = 0;
    for(x = 0; x < vertical_segment_count; x++) {
		for(y = 0; y< vertical_segment_count; y++){
			adjacencyList_v[x][y] = INT_MAX;//Initially there is no edge between any vertices
		}
    }
    for(x = 0; x <i_count; x++) {
		for(y = 0; y<i_count; y++){
			IntersectionPoint ip1 = intersection_point[x];
			IntersectionPoint ip2 = intersection_point[y];
			if(ip1.h_t == ip2.h_t && ip1.v_t!=ip2.v_t){
				int distance = calEuclDistance(ip1.point, ip2.point);
				adjacencyList_v[ip1.v_t][ip2.v_t] = distance;
				adjacencyList_v[ip2.v_t][ip1.v_t] = distance;
            }
		}
    }
    kruskalMST_Vertical();
}
void kruskalMST_Horizontal(){
    int i,j,k,a,b,u,v,ne=1;
    int min,mincost=0,parent[horizontal_segment_count];
    int mst_h[horizontal_segment_count];
    for(i=0; i<horizontal_segment_count;i++){
        parent[i] = i;
    }
    for (i = 0; i < horizontal_segment_count; i++){
        mst_h[i] = 0;
    }
    while(ne<horizontal_segment_count){
        for(i=0,min=INT_MAX;i<horizontal_segment_count;i++){
            for(j=0;j<horizontal_segment_count;j++){
                if(adjacencyList_h[i][j]<min){
                    min=adjacencyList_h[i][j];
                    a=u=i;
                    b=v=j;
                }
            }
        }
        u=find(u,parent);
        v=find(v, parent);
        if(uni(u,v, parent)){
            ne++;
            findIntersectonPoint_hg(a,b,mst_h);
            mincost_gh +=min;
        }
        adjacencyList_h[a][b]=adjacencyList_h[b][a]=INT_MAX;
    }
    
}
void findIntersectonPoint_hg(int h1, int h2, int mst_h[]){
    int i, j;
    for(i=0; i<i_count;i++){
        for(j=0;j<i_count;j++){
            if((intersection_point[i].h_t==h1 && intersection_point[j].h_t == h2)|| (intersection_point[i].h_t==h2 && intersection_point[j].h_t == h1)){
                if(intersection_point[i].v_t == intersection_point[j].v_t){
                    XDrawLine(display_ptr, win, gc_green, intersection_point[i].point.x, intersection_point[i].point.y, intersection_point[j].point.x, intersection_point[j].point.y);
                    return;
                }
            }
        }
    }
    printf("the end\n");
}
void kruskalMST_Vertical(){
    int i,j,k,a,b,u,v,ne=1;
    int min,mincost=0,parent[vertical_segment_count];
    int mst_v[vertical_segment_count];
    for(i=0; i<vertical_segment_count;i++){
        parent[i] = i;
    }
    while(ne<vertical_segment_count){
        for(i=0,min=INT_MAX;i<vertical_segment_count;i++){
            for(j=0;j<vertical_segment_count;j++){
				if(adjacencyList_v[i][j]<min){
                    min=adjacencyList_v[i][j];
                    a=u=i;
                    b=v=j;
                }
            }
        }
        u=find(u,parent);
        v=find(v, parent);
        if(uni(u,v, parent)){
            findIntersectonPoint_vg(a,b);
            ne++;
            mincost_gv +=min;
        }
        adjacencyList_v[a][b]=adjacencyList_v[b][a]=INT_MAX;
    }
    
}

void findIntersectonPoint_vg(int v1, int v2){
    int i, j;
    for(i=0; i<i_count;i++){
        for(j=0;j<i_count;j++){
            if((intersection_point[i].v_t==v1 && intersection_point[j].v_t == v2)|| (intersection_point[i].v_t==v2 && intersection_point[j].v_t == v1)){
                if(intersection_point[i].h_t == intersection_point[j].h_t){
                   XDrawLine(display_ptr, win, gc_blue, intersection_point[i].point.x, intersection_point[i].point.y, intersection_point[j].point.x, intersection_point[j].point.y);
                    return;
                }
            }
        }
    }
}

int find(int i, int parent[]){
	int x;
	while(parent[i]!=i)
		i = parent[i];
	return i;
}
int uni(int i,int j, int parent[]){
	if(i!=j){
		parent[j]=i;
		return 1;
	}
	return 0;
}

