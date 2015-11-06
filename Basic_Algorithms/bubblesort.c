#include <stdio.h>
#include <stdlib.h>

/* bubblesort1: sort integer array a[length] ; standard bubblesort */
/* sorts in increasing order */
void bubblesort1(int *a, int length)
{  int i, temp, finished =0; 
   while(!finished)
   {  finished =1;
      for( i = 0; i< length-1; i++)
      {  if(a[i] > a[i+1]) 
         {  temp = a[i]; a[i] = a[i+1]; a[i+1] = temp; /*exchange*/
	 finished = 0; /* not finished, some exchange done */
         } 
      }
   }
}

/*-------------------------------------------------------------------------*/


/* bubblesort2: sort integer array a[length] ; single-loop bubblesort */
void bubblesort2(int *a, int length)
{  int c, i, temp; c = length -1;
   for( i = 0; i < c*c; i++)
      if(a[i%c] > a[(i%c)+1]) 
      {  temp = a[i%c]; a[i%c] = a[(i%c)+1]; a[(i%c)+1] = temp;
      } 
}

/*-------------------------------------------------------------------------*/

/* bubblesort3: sort list of integers, list assumed to be NULL-terminated */

struct listnode { int value;
                  struct listnode *next;
                };

void bubblesort3(struct listnode *a)
{    int tmp, finished = 0;
     struct listnode *currentnode, *nextnode;
     if( a == NULL )
       return;
     while( !finished )
     {  currentnode = a; nextnode = currentnode->next;
        finished =1; 
        while( nextnode != NULL )
	  {  if(currentnode->value > nextnode->value)
	     {  tmp = currentnode->value; 
                currentnode->value = nextnode->value;
                nextnode->value = tmp;
                finished = 0; 
	     }
             currentnode = nextnode; nextnode = currentnode->next;
          } 
     }
}

