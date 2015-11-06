// Find maximum flow in a directed graph, using the Ford-Fulkerson algorithm on capacities given as matrix
// Augmented path is found using BFS
//  Created by Saniya Saifee on 11/24/13.
//
#include <stdio.h>
#include <stdlib.h>

int bfs(int** resCap, int* parentArr, int s, int t,int n){
    int queue[n];
    int maxFlow[n];
    int vertexVisited[n];
    for(int i=0;i<n;i++){
        vertexVisited[i] = 0;
        parentArr[i] = -1;
    }
    int front = 0;
    int rear = 0;
    queue[rear++] = s ;//insert element in queue
    vertexVisited[s] = 1;
    parentArr[s] = -2;
    maxFlow[s]=10000000;
    while(front != rear){
        int u= queue[front++]; //get the first element in the queue ;
        for(int v=0; v<n;v++){
            if(vertexVisited[v]==0 && parentArr[v] == -1 && resCap[u][v]>0){
                parentArr[v] = u;
                if(maxFlow[u]<resCap[u][v])
                    maxFlow[v] = maxFlow[u];
                else
                    maxFlow[v] = resCap[u][v];
                queue[rear++] = v ;
                vertexVisited[v]=1;
            }
        }
    }
    if(vertexVisited[t]==1){
        return maxFlow[t];
    }
    return 0;
}
void maximum_flow(int n, int s, int t, int *capacity, int *flow){
    int bfsArr[n];
    int v,u,i,j;
    int **currResidCap; //an n × n matrix to hold the current residual capacities;
    currResidCap = (int**)malloc(n * sizeof(int*));
    for(i = 0; i < n; i++)
    {
		currResidCap[i] = (int*)malloc(n * sizeof(int));
    }
    for( i=0; i<n; i++)
        for( j=0;j<n; j++){
            currResidCap[i][j] = *(capacity + (i*n) + j);
            flow[i * n + j] = 0;
    }
    while(1){
        int maxFlow = bfs(currResidCap, bfsArr,s, t,n);
        if(maxFlow == 0)
            break;
        v = t;
        while(v != s){
            u = bfsArr[v];
            flow[u*n+v] = *(flow + u*n + v) + maxFlow;
            flow[v*n+u] = *(flow + v*n + u) - maxFlow;
            currResidCap[u][v] = currResidCap[u][v] - maxFlow;
            currResidCap[v][u] = currResidCap[v][u] + maxFlow;
            v = u;
        }
    }
}
