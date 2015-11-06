// Saniya Saifee
// StringMatching.c
// Fast string matching

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void createKmpTable(int *kmptable, char *t){
    int pos = 2;
    int cnd = 0;
    int patternSize = (int)strlen(t);
    kmptable[0] = -1;
    kmptable[1] = 0;
    while(pos < patternSize){
        if(t[pos - 1] == t[cnd]){
            cnd = cnd + 1;
            kmptable[pos] = cnd;
            pos = pos+1;
        }
        else if(cnd > 0){
            cnd = kmptable[cnd];
        }
        else{
            kmptable[pos] = 0;
            pos = pos +1;
        }
    }
}

int find_string(char *s, char *t){
    int m = 0;
    int i = 0;
    int patternSize = (int)strlen(t);
    int stringSize = (int)strlen(s);
    int kmptable[patternSize];
    createKmpTable(kmptable, t);
    while((m+i) < stringSize){
        if(t[i] == s[m+i]){
            if(i == (patternSize - 1)){
                return m;
            }
            i++;
        }
        else{
            m = m + i - kmptable[i];
            if(kmptable[i] > -1){
                i = kmptable[i];
            }
            else{
                i = 0;
            }

        }
    }
    return -1;
}