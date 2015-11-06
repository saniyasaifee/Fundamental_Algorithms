/*
 ============================================================================
 Name        : HW3_SaniyaSaifee.c
 Author      : Saniya Saifee
 ============================================================================
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
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
char *win_name_string = "Dynamc Programming";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_red, gc_red_thick, gc_blue, gc_blue_thick, gc_green, gc_green_thick;
unsigned long background, valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_blue_values, gc_green_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;


typedef struct point {
	int x;
	int y;
} Point;

typedef enum server {
	RED, GREEN, BLUE
} Server;
typedef struct MinOptCostAtT{
	int prevT,prevI, prevJ, prevK;
}min_opt_cost_t;
typedef struct drawOptPath{
	int i, j, k;
}draw_opt_path_t;

min_opt_cost_t ****moct;
draw_opt_path_t *dopt;
int calEuclDistance(Point point1, Point point2);
void MoveClosestServer(Point p);
Point serverRed, serverGreen, serverBlue, *points;
int onlineAlgoCost = 0;
int pointsCount = 0;
int ****cost;
float cRatio = 0.0;
int buttonPressCount = 0;
int overallminDis =INT_MAX;
int I,J,K, finalI, finalJ,finalK;
int main(int argc, char **argv) {
	//int a, b, c, d;
	//int i = 0;
	int buttonPressCount = 0;
	points = (Point*) malloc(100 * sizeof(Point));
	serverRed.x = 150;
	serverRed.y = 200;
	serverGreen.x = 500;
	serverGreen.y = 200;
	serverBlue.x = 350;
	serverBlue.y = 500;
	points[pointsCount++] = serverRed;
	points[pointsCount++] = serverGreen;
	points[pointsCount++] = serverBlue;
	printf("pointsCount=%d\n",pointsCount);
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
	border_width = 5;
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
    XSetLineAttributes( display_ptr, gc, 2, LineSolid, CapRound, JoinRound);
	/* and three other graphics contexts, to draw in yellow and red and grey*/

	gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
	//XSetLineAttributes( display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound);
	if (XAllocNamedColor(display_ptr, color_map, "red", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color red\n");
		exit(-1);
	}
	else
		XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
	gc_red_thick = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
	XSetLineAttributes( display_ptr, gc_red_thick, 6, LineSolid, CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "red", &tmp_color1, &tmp_color2 ) == 0 ){
		printf("failed to get color red\n"); exit(-1);
	}
	else
		XSetForeground( display_ptr, gc_red_thick, tmp_color1.pixel );
	gc_blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
	if (XAllocNamedColor(display_ptr, color_map, "blue", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color blue\n");
		exit(-1);
	} else
		XSetForeground(display_ptr, gc_blue, tmp_color1.pixel);

	gc_blue_thick = XCreateGC( display_ptr, win, valuemask, &gc_blue_values);
	XSetLineAttributes( display_ptr, gc_blue_thick, 6, LineSolid, CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "blue", &tmp_color1, &tmp_color2 ) == 0 ){
		printf("failed to get color blue\n");
		exit(-1);
	}
	else
		XSetForeground( display_ptr, gc_blue_thick, tmp_color1.pixel );
	gc_green = XCreateGC(display_ptr, win, valuemask, &gc_green_values);
	if (XAllocNamedColor(display_ptr, color_map, "green", &tmp_color1,
			&tmp_color2) == 0) {
		printf("failed to get color green\n");
		exit(-1);
	} else
		XSetForeground(display_ptr, gc_green, tmp_color1.pixel);
	gc_green_thick = XCreateGC( display_ptr, win, valuemask, &gc_green_values);
	XSetLineAttributes( display_ptr, gc_green_thick, 6, LineSolid, CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "green", &tmp_color1, &tmp_color2 ) == 0 ){
		printf("failed to get color green\n");
		exit(-1);
	}
	else
		XSetForeground( display_ptr, gc_green_thick, tmp_color1.pixel );
	/* and now it starts: the event loop */
	while (1) {
		XNextEvent(display_ptr, &report);
		switch (report.type) {
			case Expose:
				/* (re-)draw the example figure. This event happens
				 each time some part ofthe window gets exposed (becomes visible) */
				XFillArc(display_ptr, win, gc_red,serverRed.x - 5 ,serverRed.y - 5,10,10,0,360*64);
				XFillArc(display_ptr, win, gc_green,serverGreen.x - 5 ,serverGreen.y - 5,10,10,0,360*64);
				XFillArc(display_ptr, win, gc_blue,serverBlue.x - 5 ,serverBlue.y - 5,10,10,0,360*64);
				printf("Online Algorithm Strategy\n");
				break;
			case ConfigureNotify:
				/* This event happens when the user changes the size of the window*/
				win_width = report.xconfigure.width;
				win_height = report.xconfigure.height;
				break;
			case ButtonPress:
				{
					/* This event happens when the user pushes a mouse button. I draw
					 a circle to show the point where it happened, but do not save
					 the position; so when the next redraw event comes, these circles
					 disappear again. */
					if (report.xbutton.button == Button1 ){
						buttonPressCount++;
						int x, y;
						x = report.xbutton.x;
						y = report.xbutton.y;
						buttonPressCount++;
						Point p;
						p.x = x;
						p.y = y;
                        XDrawPoint(display_ptr, win, gc,p.x,p.y);
						points[pointsCount++] = p;
						MoveClosestServer(p);
					}
					else if(report.xbutton.button == Button3){
						// find optimum cost
						//create 4d array
						buttonPressCount = 0;
						int i,j,k,l,t;
                        dopt = (draw_opt_path_t*) malloc(pointsCount * sizeof (draw_opt_path_t));
						moct = (min_opt_cost_t****) malloc(pointsCount * sizeof (min_opt_cost_t***));
                        if (moct == NULL)
						{
                            fprintf(stderr,"Out Of Memory");
                            exit(1);
						}
						for(i = 0;i < pointsCount;i++)
						{
							moct[i] = (min_opt_cost_t***)malloc(pointsCount * sizeof(min_opt_cost_t**));
							if(moct[i] == NULL)
							{
								fprintf(stderr,"Out Of Memory");
								exit(EXIT_FAILURE);
							}
                            
							for(j = 0;j < pointsCount;j++)
							{
								moct[i][j] = (min_opt_cost_t**)malloc(pointsCount * sizeof(min_opt_cost_t*));
								if(moct[i][j] == NULL)
								{
									fprintf(stderr,"Out Of Memory");
									exit(EXIT_FAILURE);
								}
								for(k = 0;k < pointsCount;k++)
								{
									moct[i][j][k] = (min_opt_cost_t*)malloc(pointsCount * sizeof(min_opt_cost_t));
									if(moct[i][j][k] == NULL)
									{
										fprintf(stderr,"Out Of Memory");
										exit(EXIT_FAILURE);
									}
								}
							}
						}
						cost = (int****)malloc(pointsCount * sizeof(int***));
						if (cost == NULL)
						{
						        fprintf(stderr,"Out Of Memory");
						        exit(1);
						}
						for(i = 0;i < pointsCount;i++)
						{
							cost[i] = (int***)malloc(pointsCount * sizeof(int**));
							if(cost[i] == NULL)
							{
								fprintf(stderr,"Out Of Memory");
								exit(EXIT_FAILURE);
							}

							for(j = 0;j < pointsCount;j++)
							{
								cost[i][j] = (int**)malloc(pointsCount * sizeof(int*));
								if(cost[i][j] == NULL)
								{
									fprintf(stderr,"Out Of Memory");
									exit(EXIT_FAILURE);
								}
								for(k = 0;k < pointsCount;k++)
								{
									cost[i][j][k] = (int*)malloc(pointsCount * sizeof(int));
									if(cost[i][j][k] == NULL)
									{
										fprintf(stderr,"Out Of Memory");
										exit(EXIT_FAILURE);
									}
								}
							}
						}
						for(t=0; t<pointsCount; t++){
							for(i=0; i<pointsCount; i++){
								for(j=0; j<pointsCount; j++){
									for(k=0; k<pointsCount; k++){
										cost[t][i][j][k] = INT_MAX;
									}
								}
							}
						}
						cost[2][0][1][2]=0;
						int optimalCost=INT_MAX;
						int minDistance;
						int minCostRed = INT_MAX, minCostGreen = INT_MAX, minCostBlue = INT_MAX;
						for(t=3; t<pointsCount; t++){
							for(i=0; i<=t; i++){
								for(j=0; j<=t;j++){
									for(k=0;k<=t;k++){
										if(i==t||j==t||k==t){
											minDistance = INT_MAX;
											for(l=0;l<t;l++){
												if(cost[t-1][l][j][k]<INT_MAX){
                                                    minCostRed = cost[t-1][l][j][k] + calEuclDistance(points[l],points[i]);
													if(minCostRed < minDistance){
														minDistance = minCostRed;
														I=l;
														J=j;
														K=k;
													}
												}
												if(cost[t-1][i][l][k]<INT_MAX){
                                                    minCostGreen = cost[t-1][i][l][k] + calEuclDistance(points[l],points[j]);
													if(minCostGreen < minDistance){
														minDistance = minCostGreen;
														I=i;
														J=l;
														K=k;
													}
												}
												if(cost[t-1][i][j][l]<INT_MAX){
                                                    minCostBlue = cost[t-1][i][j][l] + calEuclDistance(points[l],points[k]);
													if(minCostBlue < minDistance){
														minDistance = minCostBlue;
														I=i;
														J=j;
														K=l;
													}
												}
                                                if(minDistance<INT_MAX){
                                                    cost[t][i][j][k] = minDistance;
                                                    moct[t][i][j][k].prevT = t-1;
                                                    moct[t][i][j][k].prevI = I;
                                                    moct[t][i][j][k].prevJ = J;
                                                    moct[t][i][j][k].prevK = K;
                                                    if(t==pointsCount-1 && minDistance < overallminDis){
                                                        overallminDis = minDistance;
                                                        finalI = i;
                                                        finalJ = j;
                                                        finalK = k;
                                                        dopt[t].i = finalI;
                                                        dopt[t].j = finalJ;
                                                        dopt[t].k = finalK;
                                                    }
                                                }
                                            
											}
										}
									}

								}
							}
						}
                        printf("end\n");
                        t=pointsCount-1;
                        int tempt,tempi, tempj, tempk;
                        while(t!=2){
                            tempt = moct[t][finalI][finalJ][finalK].prevT;
                            tempi = moct[t][finalI][finalJ][finalK].prevI;
                            tempj = moct[t][finalI][finalJ][finalK].prevJ;
                            tempk = moct[t][finalI][finalJ][finalK].prevK;
                            dopt[tempt].i = tempi;
                            dopt[tempt].j = tempj;
                            dopt[tempt].k = tempk;
                            t = tempt;
                            finalI = tempi;
                            finalJ = tempj;
                            finalK = tempk;
                        }
                        
						Point pRed = points[0];
						Point pGreen = points[1];
						Point pBlue = points[2];
						printf("Offline/Optimal Algorithm Strategy:\n");
						for(t=3; t<pointsCount; t++){
							if(t== dopt[t].i){
								XDrawLine(display_ptr, win, gc_red_thick, pRed.x, pRed.y, points[t].x, points[t].y);
								printf("Request(%d, %d) - P%d served by Red server. Server Red(P0) = %d, Server Green(P1) = %d, Server Blue(P2) = %d, Cost = %d\n",points[t].x, points[t].y,t, dopt[t].i, dopt[t].j, dopt[t].k, cost[t][dopt[t].i][dopt[t].j][dopt[t].k]);
								pRed =  points[t];
							}
							else if(t== dopt[t].j){
								XDrawLine(display_ptr, win, gc_green_thick, pGreen.x, pGreen.y, points[t].x, points[t].y);
                                printf("Request(%d, %d) - P%d served by Green server. Server Red(P0) = %d, Server Green(P1) = %d, Server Blue(P2) = %d, Cost = %d\n",points[t].x, points[t].y,t, dopt[t].i, dopt[t].j, dopt[t].k, cost[t][dopt[t].i][dopt[t].j][dopt[t].k]);
								pGreen =  points[t];
							}
							else if(t== dopt[t].k){
								XDrawLine(display_ptr, win, gc_blue_thick, pBlue.x, pBlue.y, points[t].x, points[t].y);
                                printf("Request(%d, %d) - P%d served by Blue server. Server Red(P0) = %d, Server Green(P1) = %d, Server Blue(P2) = %d, Cost = %d\n",points[t].x, points[t].y,t, dopt[t].i, dopt[t].j, dopt[t].k, cost[t][dopt[t].i][dopt[t].j][dopt[t].k]);
								pBlue = points[t];
							}

						}
						optimalCost = overallminDis;
                        printf("online Algorithm Cost = %d\n", onlineAlgoCost);
                        printf("optimal Algorithm Cost = %d\n", optimalCost);
						cRatio = (float)onlineAlgoCost/(float)optimalCost;
						printf("The competitive ratio is:%f\n", cRatio );
					}
				}
			break;
			default:
				/* this is a catch-all for other events; it does not do anything.
				One could look at the report type to see what the event was */
			break;
		}
	}
}
void MoveClosestServer(Point p) {
	int distRed, distGreen, distBlue;
	distRed = calEuclDistance(p, serverRed);
	distGreen = calEuclDistance(p, serverGreen);
	distBlue = calEuclDistance(p, serverBlue);
	int minDistance = 0;
	if (distRed < distGreen && distRed < distBlue) {
		XDrawLine(display_ptr, win, gc_red, serverRed.x, serverRed.y, p.x, p.y);
		serverRed = p;
		minDistance = distRed;
		printf("Request %d (%d, %d) served by red server, cost = %d\n", pointsCount-1, p.x, p.y,minDistance );
	}
	else if (distGreen < distRed && distGreen < distBlue) {
		XDrawLine(display_ptr, win, gc_green, serverGreen.x, serverGreen.y, p.x, p.y);
		serverGreen = p;
		minDistance = distGreen;
		printf("Request %d (%d, %d) served by green server, cost = %d\n", pointsCount-1, p.x, p.y, minDistance);
	}
	else if (distBlue < distRed && distBlue < distGreen) {
		XDrawLine(display_ptr, win, gc_blue, serverBlue.x, serverBlue.y, p.x, p.y);
		serverBlue = p;
		minDistance = distBlue;
		printf("Request %d (%d, %d) served by blue server, cost = %d\n", pointsCount-1, p.x, p.y, minDistance);
	}
	onlineAlgoCost = onlineAlgoCost + minDistance;
}

int calEuclDistance(Point point1, Point point2) {
	int diffx = point1.x - point2.x;
	int diffy = point1.y - point2.y;
	int diffx_sqr = pow(diffx, 2);
	int diffy_sqr = pow(diffy, 2);
	int distance = sqrt(diffx_sqr + diffy_sqr);
	return (int) distance;
}