#include <stdio.h>
#include <stdlib.h>

/* insertionsort1: sort integer array a[length] ; standard insertionsort */
/* sorts in increasing order, finds smallest item first */
void insertionsort1(int *a, int length)
{  int i, j, temp;
   for( i=0; i< length-1; i++)
   {  for( j=i+1;  j< length; j++)
      {  if(a[i] > a[j]) 
         {  temp = a[i]; a[i] = a[j]; a[j] = temp; /*exchange*/
         } 
      }
   }
}

/*-------------------------------------------------------------------------*/


/* insertionsort2: sort integer array a[length] ; recursive insertionsort */
/* sorts in increasing order, finds largest item first */
void insertionsort2(int *a, int length)
{  int j, temp;
   for( j = 0; j< length-1; j++)
      if(a[length-1] < a[j]) 
      {  temp = a[j]; a[j] = a[length-1]; a[length-1] = temp;
      }
   if( length > 1) 
      insertionsort2(a,length-1);
}

/*-------------------------------------------------------------------------*/
/*---demonstratrion test code ---------------------------------------------*/

main()
{  int b[10000], c[10000]; int i;
   for(i=0; i< 10000; i++)
   {  b[(37*i+1234)%10000] = 3*i;
      c[(41*i+2341)%10000] = 5*i;
   }
   printf("prepared arrays. running insertionsort1. ");
   insertionsort1(b,10000); printf("running insertionsort2.\n");
   insertionsort2(c,10000); printf("completed sorting, now testing.\n");
   for(i=0; i<10000; i++)
     if( b[i] != 3*i )
       {  printf("insertionsort1 failed.\n");
          break;
       }
   for(i=0; i<10000; i++)
     if( c[i] != 5*i )
       {  printf("insertionsort2 failed.\n");
          break;
       }
   printf("finished test.\n");
}
