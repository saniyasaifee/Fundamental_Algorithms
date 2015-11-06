/*
 ============================================================================
 Name        : HW2_SaniyaSaifee.c
 Author      : Saniya Saifee
 ============================================================================
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
# define MAXRECTOBSTACLES 100
# define MAXVERTICES 400

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

GC gc, gc_yellow, gc_red, gc_blue, gc_green;
unsigned long background, valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_blue_values,
		gc_green_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;
int stopVerifying = 0;
int id = 0;
/*previous array used to keep track of shortest path from source to vertex*/
int *previous;
/*distanceFromSrc array will hold the shortest distance from source vertex to all vertices in graph*/
int *distanceFromSrc;
/*visitedVeretex array will keep track of visited vertices while finding shortest path from source to target*/
int *visitedVeretex;
int numberGreenNodes = 0;
int **adjacencyList;

typedef enum node_type {
	RED, GREEN, YELLOW
} node_type_t;

typedef struct quadtree_point {
	int x;
	int y;
} quadtree_point_t;

/*for co-ordinates*/
typedef struct {
	quadtree_point_t lowerLeft;
	quadtree_point_t upperRight;
	quadtree_point_t lowerRight;
	quadtree_point_t upperLeft;
	int width;
	int height;
} RectangleObstacle;

typedef struct {
	quadtree_point_t center;
	int radius;
} DiscRobot;

DiscRobot initPos, finalPos;

typedef struct quadtree_bounds {
	quadtree_point_t *nw;
	quadtree_point_t *se;
	int width;
	int height;
} quadtree_bounds_t;

typedef struct quadtree_node {
	struct quadtree_node *ne;
	struct quadtree_node *nw;
	struct quadtree_node *se;
	struct quadtree_node *sw;
	quadtree_bounds_t *bounds;
	node_type_t nt;
	int nodeId;
} quadtree_node_t;

typedef struct quadtree {
	quadtree_node_t *root;
} quadtree_t;

typedef struct stack {
	quadtree_node_t *node;
	struct stack *next;
} leaf_node_t;
quadtree_point_t* quadtree_point_new(int x, int y);
quadtree_bounds_t* quadtree_bounds_new(int minx, int miny, int maxx, int maxy);
void quadtree_bounds_extend(quadtree_bounds_t *bounds, int x, int y);
quadtree_node_t* quadtree_node_new();
int quadtree_node_ispointer(quadtree_node_t *node);
int quadtree_node_isempty(quadtree_node_t *node);
int quadtree_node_isleaf(quadtree_node_t *node);
quadtree_node_t* quadtree_node_with_bounds(int minx, int miny, int maxx, int maxy);
quadtree_t* quadtree_new(int minx, int miny, int maxx, int maxy);
quadtree_node_t* get_quadrant_(quadtree_node_t *root, quadtree_point_t point);
void init();
void push(quadtree_node_t* node);
void verifyNodes(RectangleObstacle* obstacle, int obstacleSize,quadtree_node_t* root);
void classifyNodes(RectangleObstacle* obstacle, int obstacleSize,quadtree_node_t* node);
leaf_node_t* top;
quadtree_node_t *green_cell;
int main(int argc, char **argv) {
	init();
	int a, b, c, d;
	int i = 0;
	int buttonPressCount = 0;
	FILE *fp;
	fp = fopen(argv[1], "r");
	int oc = 0;
	RectangleObstacle *obstacle;
	obstacle = (RectangleObstacle *) malloc(
			MAXRECTOBSTACLES * sizeof(RectangleObstacle));
	if (obstacle == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(0);
	}

	/*allocating memory to array of line segment obstacles*/

	while (fscanf(fp, "R (%d,%d) (%d,%d)\n", &a, &b, &c, &d) == 4) {
		obstacle[oc].upperLeft.x = a;
		obstacle[oc].upperLeft.y = b;
		obstacle[oc].lowerRight.x = c;
		obstacle[oc].lowerRight.y = d;
		obstacle[oc].upperRight.x = c;
		obstacle[oc].upperRight.y = b;
		obstacle[oc].lowerLeft.x = a;
		obstacle[oc].lowerLeft.y = d;
		obstacle[oc].width = calEuclDistance(obstacle[oc].upperRight, obstacle[oc].upperLeft);
		obstacle[oc].height = calEuclDistance(obstacle[oc].lowerRight,obstacle[oc].upperRight);
		++oc;
	}
	/* opening display: basic connection to X Server */
	if ((display_ptr = XOpenDisplay(display_name)) == NULL) {
		printf("Could not open display. \n");
		exit(-1);
	}
	printf("Connected to X server  %s\n", XDisplayName(display_name));
	screen_num = DefaultScreen(display_ptr);
	screen_ptr = DefaultScreenOfDisplay(display_ptr);
	color_map = XDefaultColormap(display_ptr, screen_num);
	display_width = DisplayWidth(display_ptr, screen_num);
	display_height = DisplayHeight(display_ptr, screen_num);

	printf("Width %d, Height %d, Screen Number %d\n", display_width,
			display_height, screen_num);
	/* creating the window */
	border_width = 10;
	win_x = 0;
	win_y = 0;
	win_width = 700; //display_width/2;
	win_height = 700; //(int) (win_width / 1.7); /*rectangular window*/
	XAllocNamedColor(display_ptr, color_map, "yellow", &tmp_color1,
			&tmp_color2);
	background = tmp_color1.pixel;
	win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
			win_x, win_y, win_width, win_height, border_width,
			BlackPixel(display_ptr, screen_num), background);
	/* now try to put it on screen, this needs cooperation of window manager */
	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();
	if (size_hints == NULL || wm_hints == NULL || class_hints == NULL) {
		printf("Error allocating memory for hints. \n");
		exit(-1);
	}

	size_hints->flags = PPosition | PSize | PMinSize;
	size_hints->min_width = 60;
	size_hints->min_height = 60;

	XStringListToTextProperty(&win_name_string, 1, &win_name);
	XStringListToTextProperty(&icon_name_string, 1, &icon_name);

	wm_hints->flags = StateHint | InputHint;
	wm_hints->initial_state = NormalState;
	wm_hints->input = False;

	class_hints->res_name = "x_use_example";
	class_hints->res_class = "examples";

	XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc,
			size_hints, wm_hints, class_hints);

	/* what events do we want to receive */
	XSelectInput(display_ptr, win,
	ExposureMask | StructureNotifyMask | ButtonPressMask);

	/* finally: put window on screen */
	XMapWindow(display_ptr, win);

	XFlush(display_ptr);

	/* create graphics context, so that we may draw in this window */
	gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
	XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
	// XSetLineAttributes( display_ptr, gc, 0, LineSolid, CapRound, JoinRound);
	/* and three other graphics contexts, to draw in yellow and red and grey*/
	gc_yellow = XCreateGC(display_ptr, win, valuemask, &gc_yellow_values);
	//  XSetLineAttributes(display_ptr, gc_yellow, 1, LineSolid,CapRound, JoinRound);
	if (XAllocNamedColor(display_ptr, color_map, "yellow", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color yellow\n");
		exit(-1);
	} else
		XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
	gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
	//XSetLineAttributes( display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound);
	if (XAllocNamedColor(display_ptr, color_map, "red", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color red\n");
		exit(-1);
	} else
		XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
	gc_blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
	if (XAllocNamedColor(display_ptr, color_map, "blue", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color grey\n");
		exit(-1);
	} else
		XSetForeground(display_ptr, gc_blue, tmp_color1.pixel);

	gc_green = XCreateGC(display_ptr, win, valuemask, &gc_green_values);
	if (XAllocNamedColor(display_ptr, color_map, "green", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color green\n");
		exit(-1);
	} else
		XSetForeground(display_ptr, gc_green, tmp_color1.pixel);

	/* and now it starts: the event loop */
	while (1) {
		XNextEvent(display_ptr, &report);
		switch (report.type) {
		case Expose:
			/* (re-)draw the example figure. This event happens
			 each time some part ofthe window gets exposed (becomes visible) */
			//XDrawRectangle(display_ptr, win, gc_yellow, 0,0,700,700);
			for (i = 0; i < oc; i++) {
				if (obstacle[i].width == 0 || obstacle[i].height == 0)
					XDrawLine(display_ptr, win, gc_blue,
							obstacle[i].upperLeft.x, obstacle[i].upperLeft.y,
							obstacle[i].lowerRight.x, obstacle[i].lowerRight.y);
				else
					XFillRectangle(display_ptr, win, gc_blue,
							obstacle[i].upperLeft.x, obstacle[i].upperLeft.y,
							obstacle[i].width, obstacle[i].height);
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
			int x, y;
			x = report.xbutton.x;
			y = report.xbutton.y;
			buttonPressCount++;
			if (buttonPressCount == 1) {
				XClearArea(display_ptr,win,0, 0, 700,700,1);
				top = NULL;
				numberGreenNodes = 0;
				stopVerifying = 0;
				id = 0;
				initPos.center.x = report.xbutton.x;
				initPos.center.y = report.xbutton.y;
				//XDrawPoint(display_ptr,win,gc_red, startVertex.x, startVertex.y);
			}
			if (buttonPressCount == 2) {
				quadtree_point_t tempPoint;
				tempPoint.x = report.xbutton.x;
				tempPoint.y = report.xbutton.y;
				initPos.radius = calEuclDistance(initPos.center, tempPoint);
				//XDrawArc(display_ptr, win, gc,  initPos.center.x - initPos.radius, initPos.center.y-initPos.radius, 2*initPos.radius, 2*initPos.radius,0, 360*64);

			}
			if (buttonPressCount == 3) {
				buttonPressCount = 0;
				finalPos.center.x = report.xbutton.x;
				finalPos.center.y = report.xbutton.y;
				finalPos.radius = initPos.radius;
				XDrawRectangle(display_ptr, win, gc, finalPos.radius, finalPos.radius, 700 - (2 * finalPos.radius),700 - (2 * finalPos.radius));
				quadtree_t* tree = quadtree_new(initPos.radius, initPos.radius, 700 - initPos.radius, 700 - initPos.radius);
				split_node_(tree->root);
				verifyNodes(obstacle, oc, tree->root);
				leaf_node_t *current;
				int i = 0;
				// Check if start position and target position is in one connected component by DFS
				current = top;
				if (current != NULL) {
					do {
						if (current->node->nt == 0) {
							XDrawRectangle(display_ptr, win, gc_red,current->node->bounds->nw->x,current->node->bounds->nw->y,current->node->bounds->width, current->node->bounds->height);
						}
						if (current->node->nt == 1) {
							numberGreenNodes++;
							XFillRectangle(display_ptr, win, gc_green,current->node->bounds->nw->x,current->node->bounds->nw->y,current->node->bounds->width,current->node->bounds->height);
							XDrawLine(display_ptr, win, gc,current->node->bounds->nw->x,current->node->bounds->nw->y,current->node->bounds->nw->x,current->node->bounds->se->y);
							XDrawLine(display_ptr, win, gc,current->node->bounds->nw->x,current->node->bounds->se->y,current->node->bounds->se->x,current->node->bounds->se->y);
							XDrawLine(display_ptr, win, gc,current->node->bounds->se->x,current->node->bounds->se->y,current->node->bounds->se->x,current->node->bounds->nw->y);
							XDrawLine(display_ptr, win, gc,current->node->bounds->se->x,current->node->bounds->nw->y,current->node->bounds->nw->x, current->node->bounds->nw->y);
						}
						if (current->node->nt == 2) {
							XDrawRectangle(display_ptr, win, gc,current->node->bounds->nw->x,current->node->bounds->nw->y,current->node->bounds->width,current->node->bounds->height);
						}
						current = current->next;
					} while (current != NULL);
				} else {
					printf("The Stack is empty\n");
					exit(0);
				}
				XFillArc(display_ptr, win, gc,initPos.center.x - initPos.radius,initPos.center.y - initPos.radius, 2 * initPos.radius,2 * initPos.radius, 0, 360 * 64);
				XFillArc(display_ptr, win, gc_red,finalPos.center.x - finalPos.radius,finalPos.center.y - finalPos.radius,2 * finalPos.radius, 2 * finalPos.radius, 0, 360 * 64);
				quadtree_node_t* initPosQuadrant = tree->root;
				adjacencyList = malloc(id * sizeof(int*));
				int j = 0;
				if (adjacencyList == NULL) {
					fprintf(stderr, "out of memory\n");
					exit(0);
				}

				for (j = 0; j < id; j++) {
					adjacencyList[j] = malloc((id) * sizeof(int));
					if (adjacencyList[j] == NULL) {
						fprintf(stderr, "out of memory\n");
						exit(0);
					}
				}

				do {
					initPosQuadrant = get_quadrant_(initPosQuadrant, initPos.center);
					if (!initPosQuadrant) {
						printf("The disc robot initial position is not within configuration space. Please try again\n");
						XCloseDisplay(display_ptr);
						exit(1);
					}
				} while (!quadtree_node_isempty(initPosQuadrant));
				//if (initPosQuadrant != NULL)
				//	printf("initPosQuadrant bounds ne.x = %d, ne.y = %d, sw.x = %d, sw.y = %d, nt = %d, id = %d \n",initPosQuadrant->bounds->se->x,initPosQuadrant->bounds->nw->y,initPosQuadrant->bounds->nw->x,initPosQuadrant->bounds->se->y, initPosQuadrant->nt,initPosQuadrant->nodeId);
				quadtree_node_t* finalPosQuadrant = tree->root;
				do {
					finalPosQuadrant = get_quadrant_(finalPosQuadrant, finalPos.center);
					if (!finalPosQuadrant) {
						printf("The disc robot final position is not within configuration space.Please try again.");
						XCloseDisplay(display_ptr);
						exit(1);
					}
				} while (!quadtree_node_isempty(finalPosQuadrant));
				//if (finalPosQuadrant != NULL)
				//	printf("finalPosQuadrant bounds ne.x = %d, ne.y = %d, sw.x = %d, sw.y = %d, nt = %d, id = %d \n ",finalPosQuadrant->bounds->nw->x,finalPosQuadrant->bounds->nw->y,finalPosQuadrant->bounds->se->x, finalPosQuadrant->bounds->se->y,finalPosQuadrant->nt, finalPosQuadrant->nodeId);
				if (initPosQuadrant->nt == 0 || finalPosQuadrant->nt == 0) {
					printf("Path cannot be found as either the start or final position of disc robot is in red square\n");
					
					//XCloseDisplay(display_ptr);
				}

				green_cell = (quadtree_node_t*) malloc(numberGreenNodes * sizeof(quadtree_node_t));
				green_cell[0] = *initPosQuadrant;
				green_cell[numberGreenNodes - 1] = *finalPosQuadrant;
				i = 1;
				current = top;
				if (current != NULL) {
					do {
						if (current->node->nt == 1 && current->node->nodeId != initPosQuadrant->nodeId && current->node->nodeId != finalPosQuadrant->nodeId) {
							green_cell[i] = *current->node;
							i++;
						}
						current = current->next;
					} while (current != NULL);
				}
				verifyNeighbors();
				dijkstra(adjacencyList, numberGreenNodes);
				j = previous[numberGreenNodes - 1];
				quadtree_node_t *p1;
				quadtree_node_t *p2;
				int is_s_t_reachable = 1;
				int prevI = INT_MAX;
				int prevJ = INT_MAX;
				for (i = numberGreenNodes - 1; j != -1; j = previous[i]) {
					if (j == prevI) {
						is_s_t_reachable = 0;
						printf("No path can be found between the start and target position as they are in separate connected component\n");
						break;
					}
					prevI = i;
					prevJ = j;
					i = j;
				}
				if (is_s_t_reachable) {
					j = previous[numberGreenNodes - 1];
					XDrawLine(display_ptr, win, gc, initPos.center.x, initPos.center.y,(initPosQuadrant->bounds->nw->x + initPosQuadrant->bounds->se->x) / 2,(initPosQuadrant->bounds->nw->y + initPosQuadrant->bounds->se->y) / 2);
					for (i = numberGreenNodes - 1; j != -1; j = previous[i]) {
						p1 = &green_cell[i];
						p2 = &green_cell[j];
						//printf("i->id = %d, j->id = %d\n", p1->nodeId, p2->nodeId);
						int p1x = (p1->bounds->nw->x + p1->bounds->se->x) / 2;
						int p1y = (p1->bounds->nw->y + p1->bounds->se->y) / 2;
						int p2x = (p2->bounds->nw->x + p2->bounds->se->x) / 2;
						int p2y = (p2->bounds->nw->y + p2->bounds->se->y) / 2;
						XDrawLine(display_ptr, win, gc, p1x, p1y, p2x, p2y);
						i = j;
					}

					XDrawLine(display_ptr, win, gc, finalPos.center.x,finalPos.center.y,(finalPosQuadrant->bounds->nw->x + finalPosQuadrant->bounds->se->x) / 2, (finalPosQuadrant->bounds->nw->y + finalPosQuadrant->bounds->se->y) / 2);
				}
			}
			break;
			default:
			/* this is a catch-all for other events; it does not do anything.
			 One could look at the report type to see what the event was */
			break;
		}

		}
		//exit(0);
	}
}

void verifyNeighbors() {
	int i = 0;
	int j = 0;
	for (i = 0; i < id; i++)
		for (j = 0; j < id; j++)
			adjacencyList[i][j] = -1;
	leaf_node_t *current;
	leaf_node_t *sweep;
	current = top;
	sweep = top;
	while (current != NULL) {
		while (sweep != NULL) {
			if (current != sweep
					&& (current->node->nt == GREEN && sweep->node->nt == GREEN)
					&& (sideNeighbor(current->node, sweep->node)
							|| frontBackNeighbor(current->node, sweep->node))) {
				int currentMidX = (current->node->bounds->se->x
						+ current->node->bounds->nw->x) / 2;
				int currentMidY = (current->node->bounds->se->y
						+ current->node->bounds->nw->y) / 2;
				int sweepMidX = (sweep->node->bounds->se->x
						+ sweep->node->bounds->nw->x) / 2;
				int sweepMidY = (sweep->node->bounds->se->y
						+ sweep->node->bounds->nw->y) / 2;
				quadtree_point_t c;
				quadtree_point_t s;
				c.x = currentMidX;
				c.y = currentMidY;
				s.x = sweepMidX;
				s.y = sweepMidY;
				int distance = calEuclDistance(c, s);
				//printf("current->node->nodeId = %d, sweep->node->nodeId = %d\n", current->node->nodeId, sweep->node->nodeId);
				adjacencyList[current->node->nodeId][sweep->node->nodeId] =
						distance;
				adjacencyList[sweep->node->nodeId][current->node->nodeId] =
						distance;
				//sweep = sweep->next;
			}
			sweep = sweep->next;
		}
		current = current->next;
		sweep = top;
	}
}

int sideNeighbor(quadtree_node_t *current, quadtree_node_t *sweep) {
	if ((sweep->bounds->se->x == current->bounds->nw->x
			|| sweep->bounds->nw->x == current->bounds->se->x)
			&& ((sweep->bounds->nw->y >= current->bounds->nw->y
					&& sweep->bounds->se->y <= current->bounds->se->y)
					|| (current->bounds->nw->y >= sweep->bounds->nw->y
							&& current->bounds->se->y <= sweep->bounds->se->y)))
		return 1;
	return 0;
}

int frontBackNeighbor(quadtree_node_t *current, quadtree_node_t *sweep) {
	if ((sweep->bounds->nw->y == current->bounds->se->y
			|| sweep->bounds->se->y == current->bounds->nw->y)
			&& ((sweep->bounds->se->x <= current->bounds->se->x
					&& sweep->bounds->nw->x >= current->bounds->nw->x)
					|| (current->bounds->se->x <= sweep->bounds->se->x
							&& current->bounds->nw->x >= sweep->bounds->nw->x)))
		return 1;
	return 0;
}

void verifyNodes(RectangleObstacle* obstacle, int obstacleSize, quadtree_node_t* root) {
	while (!stopVerifying) {
		stopVerifying = 1;
		classifyNodes(obstacle, obstacleSize, root);
	}

}
void classifyNodes(RectangleObstacle* obstacle, int obstacleSize, quadtree_node_t* root) {
	preOrder(obstacle, obstacleSize, root);
}

void preOrder(RectangleObstacle* obstacle, int obstacleSize,quadtree_node_t* node) {
	if (node == NULL) {
		return;
	}
	int colorReturned = verifyingYellow(node, obstacle, obstacleSize);
	if (colorReturned == 0 || colorReturned == 1) {
		push(node);
	} else if (node->nw != NULL && node->ne != NULL && node->sw != NULL && node->se != NULL) {
		preOrder(obstacle, obstacleSize, node->nw);
		preOrder(obstacle, obstacleSize, node->ne);
		preOrder(obstacle, obstacleSize, node->sw);
		preOrder(obstacle, obstacleSize, node->se);
	} else {
		if (node->nt == YELLOW) {
			if (node->bounds->width >= 5) {
				//printf("HI\n");
				split_node_(node);
			} else
				push(node);

		}
	}
}

int verifyingYellow(quadtree_node_t* quadrant, RectangleObstacle* obstacle,int obstaclesSize) {
	int i = 0;
	if (quadrant->nt != YELLOW && quadrant->nt != RED) {
		for (i = 0; i < obstaclesSize; i++) {
			if (obstacleContainsCell(quadrant, obstacle[i])) {
				quadrant->nt = RED;
				return 0;
			} else if (intersects(quadrant, obstacle[i])) {
				quadrant->nt = YELLOW;
				return 2;
			}

		}
		int intersectionFlag = 0;
		quadtree_point_t *p;
		p = (quadtree_point_t*) malloc(4 * sizeof(quadtree_point_t));
		p[0].x = quadrant->bounds->nw->x;
		p[0].y = quadrant->bounds->nw->y;
		p[1].x = quadrant->bounds->se->x;
		p[1].y = quadrant->bounds->nw->y;
		p[2].x = quadrant->bounds->nw->x;
		p[2].y = quadrant->bounds->se->y;
		p[3].x = quadrant->bounds->se->x;
		p[3].y = quadrant->bounds->se->y;
		p[4].x = (quadrant->bounds->nw->x + quadrant->bounds->se->x) / 2;
		p[4].y = (quadrant->bounds->nw->y + quadrant->bounds->se->y) / 2;
		int r = initPos.radius;
		int dist1, dist2, dist3, dist4;
		int j = 0;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < obstaclesSize; j++) {
				dist1 = distanceToSegment(p[i], obstacle[j].upperLeft,obstacle[j].lowerLeft);
				dist2 = distanceToSegment(p[i], obstacle[j].lowerLeft, obstacle[j].lowerRight);
				dist3 = distanceToSegment(p[i], obstacle[j].lowerRight, obstacle[j].upperRight);
				dist4 = distanceToSegment(p[i], obstacle[j].upperRight, obstacle[j].upperLeft);
				if (dist1 || dist2 || dist3 || dist4) {
					intersectionFlag++;
					break;
				}
			}
		}
		if (intersectionFlag == 4) {
			//printf()
			for (j = 0; j < obstaclesSize; j++) {
				dist1 = distanceToSegment(p[4], obstacle[j].upperLeft, obstacle[j].lowerLeft);
				dist2 = distanceToSegment(p[4], obstacle[j].lowerLeft, obstacle[j].lowerRight);
				dist3 = distanceToSegment(p[4], obstacle[j].lowerRight, obstacle[j].upperRight);
				dist4 = distanceToSegment(p[4], obstacle[j].upperRight, obstacle[j].upperLeft);
				if (dist1 || dist2 || dist3 || dist4) {
					intersectionFlag++;
				}
			}
			if (intersectionFlag > 4) {
				quadrant->nt = RED;
			} else {
				quadrant->nt = YELLOW;
			}

		} else if (intersectionFlag > 0 && intersectionFlag < 4)
			quadrant->nt = YELLOW;
	}
	if (quadrant->nt == YELLOW)
		return 2;
	if (quadrant->nt == GREEN)
		return 1;
	if (quadrant->nt == RED)
		return 0;
}

/* this function checks if the cell is entirely contained within obstacle */
int obstacleContainsCell(quadtree_node_t* quadrant, RectangleObstacle* obstacle) {
	int X = quadrant->bounds->nw->x;
	int Y = quadrant->bounds->nw->y;
	int W = quadrant->bounds->width;
	int H = quadrant->bounds->height;
	int x = obstacle->upperLeft.x;
	int y = obstacle->upperLeft.y;
	int w = obstacle->width;
	int h = obstacle->height;
	return ((X >= x) && (Y >= y) && ((X + W) <= (x + w)) && ((Y + H) <= (y + h)));
}
int intersects(quadtree_node_t* quadrant, RectangleObstacle* obstacle) {
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;
	//Calculate the sides of rect A
	leftA = quadrant->bounds->nw->x;
	rightA = leftA + quadrant->bounds->width;
	topA = quadrant->bounds->nw->y;
	bottomA = topA + quadrant->bounds->height;
	//Calculate the sides of rect B
	leftB = obstacle->upperLeft.x;
	rightB = obstacle->upperRight.x;
	topB = obstacle->upperLeft.y;
	bottomB = obstacle->lowerLeft.y;
	//If any of the sides from A are outside of B
	if (bottomA < topB) {
		return 0;
	}
	if (topA > bottomB) {
		return 0;
	}
	if (rightA < leftB) {
		return 0;
	}
	if (leftA > rightB) {
		return 0;
	}
	//If none of the sides from A are outside B
	return 1;
}

/**
 * Returns the distance of p3 to the segment defined by p1,p2;
 *
 * @param p1
 *                First point of the segment
 * @param p2
 *                Second point of the segment
 * @param p3
 *                Point to which we want to know the distance of the segment
 *                defined by p1,p2
 * @return The distance of p3 to the segment defined by p1,p2
 */

int distanceToSegment(quadtree_point_t circle, quadtree_point_t p1, quadtree_point_t p2) {
	int r = initPos.radius;
	int diffX = p2.x - p1.x;
	int diffY = p2.y - p1.y;
	if ((diffX == 0) && (diffY == 0)) {
		diffX = circle.x - p1.x;
		diffY = circle.y - p1.y;

		return ((diffX * diffX + diffY * diffY) <= r * r);
	}

	float t = ((circle.x - p1.x) * diffX + (circle.y - p1.y) * diffY)
			/ (float) (diffX * diffX + diffY * diffY);

	if (t < 0) {
		//point is nearest to the first point i.e x1 and y1
		diffX = circle.x - p1.x;
		diffY = circle.y - p1.y;
	} else if (t > 1) {
		//point is nearest to the end point i.e x2 and y2
		diffX = circle.x - p2.x;
		diffY = circle.y - p2.y;
	} else {
		//if perpendicular line intersect the line segment.
		diffX = circle.x - (p1.x + t * diffX);
		diffY = circle.y - (p1.y + t * diffY);
	}

	//returning shortest distance
	return ((diffX * diffX + diffY * diffY) <= r * r);

}

int calEuclDistance(quadtree_point_t point1, quadtree_point_t point2) {
	int diffx = point1.x - point2.x;
	int diffy = point1.y - point2.y;
	int diffx_sqr = pow(diffx, 2);
	int diffy_sqr = pow(diffy, 2);
	int distance = sqrt(diffx_sqr + diffy_sqr);
	return (int) distance;
}

quadtree_t* quadtree_new(int minx, int miny, int maxx, int maxy) {
	quadtree_t *tree;
	if (!(tree = (quadtree_t*) malloc(sizeof(quadtree_t))))
		return NULL;
	tree->root = quadtree_node_with_bounds(minx, miny, maxx, maxy);
	if (!(tree->root))
		return NULL;
	return tree;
}

quadtree_node_t* quadtree_node_with_bounds(int minx, int miny, int maxx, int maxy) {
	quadtree_node_t* node;
	if (!(node = quadtree_node_new()))
		return NULL;
	if (!(node->bounds = quadtree_bounds_new(minx, miny, maxx, maxy)))
		return NULL;
	return node;
}

quadtree_node_t* quadtree_node_new() {
	quadtree_node_t *node;
	if (!(node = (quadtree_node_t*) malloc(sizeof(quadtree_node_t))))
		return NULL;
	node->ne = NULL;
	node->nw = NULL;
	node->se = NULL;
	node->sw = NULL;
	//node->point = NULL;
	node->bounds = NULL;
	node->nt = GREEN;
	node->nodeId = id;
	id = id + 1;
	return node;
}
quadtree_bounds_t* quadtree_bounds_new(int minx, int miny, int maxx, int maxy) {
	quadtree_bounds_t *bounds;
	if ((bounds = (quadtree_bounds_t*) malloc(sizeof(quadtree_bounds_t))) == NULL)
		return NULL;
	bounds->se = quadtree_point_new(maxx, maxy);
	bounds->nw = quadtree_point_new(minx, miny);
	bounds->width = bounds->se->x - bounds->nw->x;
	bounds->height = bounds->se->y - bounds->nw->y;
	return bounds;
}

quadtree_point_t* quadtree_point_new(int x, int y) {
	quadtree_point_t* point;
	if (!(point = (quadtree_point_t*) malloc(sizeof(quadtree_point_t))))
		return NULL;
	point->x = x;
	point->y = y;
	return point;
}
void quadtree_bounds_extend(quadtree_bounds_t* bounds, int x, int y) {
	bounds->se->x = fmax(x, bounds->se->x);
	bounds->se->y = fmax(y, bounds->se->y);
	bounds->nw->x = fmin(x, bounds->nw->x);
	bounds->nw->y = fmin(y, bounds->nw->y);
	bounds->width = bounds->se->x - bounds->nw->x;
	bounds->height = bounds->se->y - bounds->nw->y;
}

int quadtree_node_ispointer(quadtree_node_t *node) {
	return node->nw != NULL && node->ne != NULL && node->sw != NULL
			&& node->se != NULL && !quadtree_node_isleaf(node);
}

int quadtree_node_isempty(quadtree_node_t *node) {
	return node->nw == NULL && node->ne == NULL && node->sw == NULL
			&& node->se == NULL;
}

int quadtree_node_isleaf(quadtree_node_t *node) {
	if (node->nt == RED || node->nt == GREEN
			|| (node->nt == YELLOW && node->bounds->width < 5)) {
		printf("%d", node->nt);
		printf("\n");
		return 1;
	} else
		return 0;
}
int split_node_(quadtree_node_t *node) {
	quadtree_node_t *nw;
	quadtree_node_t *ne;
	quadtree_node_t *sw;
	quadtree_node_t *se;
	int maxX = node->bounds->se->x;
	int maxY = node->bounds->se->y;
	int minX = node->bounds->nw->x;
	int minY = node->bounds->nw->y;
	int midX = (maxX + minX) / 2;
	int midY = (maxY + minY) / 2;
	if (!(nw = quadtree_node_with_bounds(minX, minY, midX, midY)))
		return 0;
	if (!(ne = quadtree_node_with_bounds(midX, minY, maxX, midY)))
		return 0;
	if (!(sw = quadtree_node_with_bounds(minX, midY, midX, maxY)))
		return 0;
	if (!(se = quadtree_node_with_bounds(midX, midY, maxX, maxY)))
		return 0;
	node->nw = nw;
	node->ne = ne;
	node->sw = sw;
	node->se = se;
	node->nt = YELLOW;
	stopVerifying = 0;
	return 1;
}

quadtree_node_t* get_quadrant_(quadtree_node_t *root, quadtree_point_t point) {
	if (node_contains_(root->nw, point)) {
		return root->nw;
	}
	if (node_contains_(root->ne, point)) {
		return root->ne;
	}
	if (node_contains_(root->sw, point)) {
		return root->sw;
	}
	if (node_contains_(root->se, point)) {
		return root->se;
	}
	return NULL;
}

int node_contains_(quadtree_node_t *outer, quadtree_point_t it) {
	return outer->bounds != NULL && outer->bounds->nw->x <= it.x
			&& outer->bounds->se->y >= it.y && outer->bounds->se->x >= it.x
			&& outer->bounds->nw->y <= it.y;
}

/*
 init the stack
 */
void init() {
	top = NULL;
}

void push(quadtree_node_t* node) {
	int alreadyExist = search(node);
	if (!alreadyExist) {
		leaf_node_t* tmp = (leaf_node_t*) malloc(sizeof(leaf_node_t));
		if (tmp == NULL) {
			exit(0);
		}
		tmp->node = node;
		tmp->next = top;
		top = tmp;
	}
}

int search(quadtree_node_t* node) {
	leaf_node_t *current;
	current = top;
	int exist = 0;
	if (current != NULL) {
		do {
			if (current->node->nodeId == node->nodeId) {
				exist = 1;
				break;
			}
			current = current->next;
		} while (current != NULL);
	}
	return exist;
}
/*
 pop an element from the stack
 */
leaf_node_t* pop() {
	quadtree_node_t* node = NULL;
	leaf_node_t* tmp = top;
	node = top->node;
	top = top->next;
	free(tmp);
	return node;
}
/*
 returns 1 if the stack is empty, otherwise returns 0
 */
int empty() {
	return top == NULL ? 1 : 0;
}

/*
 display the stack content
 */
int display() {
	int size = 0;
	leaf_node_t *current;
	current = top;
	if (current != NULL) {
		printf("Stack: ");
		do {
			size++;
			current = current->next;
		} while (current != NULL);
		printf("\n");
	} else {
		printf("The Stack is empty\n");
	}
	return size;

}

/* This function apply Dijkstaras algorithm to the visibilty graph
 passed in the form of 2D adjacencyList and find the shortest path
 between start and target vertex.*/
void dijkstra(int **adjacencyList, int vertices) {
	distanceFromSrc = (int *) malloc(sizeof(int) * vertices);
	previous = (int *) malloc(sizeof(int) * vertices);
	visitedVeretex = (int *) malloc(sizeof(int) * vertices);
	int i = 0;
	int u = 0;
	for (i = 0; i < vertices; i++) {
		distanceFromSrc[i] = INT_MAX;
		visitedVeretex[i] = 0;
		previous[i] = -1;
	}
	// Distance of source vertex from itself is always 0
	distanceFromSrc[0] = 0;

	// Find shortest path for all vertices
	int count = 0;
	for (count = 0; count < vertices; count++) {
		/*find the vertex with minimum distance value, from the set of vertices not
		 yet visited*/
		u = -1;
		for (i = 0; i < vertices; ++i)
			if (!visitedVeretex[i]
					&& ((u == -1) || (distanceFromSrc[i] < distanceFromSrc[u]))) {
				u = i;
			}
		// Mark the picked vertex as visited
		visitedVeretex[u] = 1;
		int v = 0;
		// Update dist value of the adjacent vertices of the visited vertex 'u'.
		for (v = 0; v < vertices; v++) {

			/* Update dist[v] only if v is not visited and there is an edge from
			 u to v, and total distance from src to  v through u is
			 smaller than current value of dist[v]*/
			quadtree_node_t* uNode = &green_cell[u];
			quadtree_node_t* vNode = &green_cell[v];
			int uNodeId = uNode->nodeId;
			int vNodeId = vNode->nodeId;
			// 	printf("u = %d, v=%d, uNodeId=%d, vNodeId=%d\n", u, v, uNodeId, vNodeId);
			if (adjacencyList[uNodeId][vNodeId] != -1)
				if (distanceFromSrc[u] + adjacencyList[uNodeId][vNodeId]
						< distanceFromSrc[v]) {
					distanceFromSrc[v] = distanceFromSrc[u]
							+ adjacencyList[uNodeId][vNodeId];
					previous[v] = u;
				}
		}
	}

}