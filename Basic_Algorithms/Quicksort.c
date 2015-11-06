
/* quicksort  for data given as linked list instead of as array.*/
/* @author: Saniya Saifee */

struct listnode *quicksort(struct listnode *list){
    struct listnode *pivot = list, *smallerElemList = NULL, *largerElemList=NULL, *next = NULL, *end = NULL, *tmp = list;
    /*for duplicate elements*/
    struct listnode *pivotHead = NULL,  *dupPivot = NULL;
    long length = 1, pivotIndex, i=1;
    /*return NULL if list to be sorted is empty */
    if(list == NULL){
        return NULL;
    }
    /*return the list if it contains only 1 item*/
    if(list->next == NULL){
        return list;
    }
    /* ﬁnd the length of the list*/
    while(tmp->next != NULL){
        length++;
        tmp = tmp->next;
    }
    /*find pivot element index*/
    pivotIndex = rand() % length;
    /*get the pivot element*/
    for (; i < pivotIndex; i++)
    {
        pivot=pivot->next;
    }
    /*Divide the list into smaller and larger elemnet array by traversing the list and comparing each element with pivot element */
    while(list!=NULL){
        next = list->next;
        if(list->value < pivot->value){
            list->next = smallerElemList;
            smallerElemList = list;
        }
        else if(list->value > pivot->value){
            list->next = largerElemList;
            largerElemList = list;
        }
        //duplicate elements
        else if(list->value == pivot->value){
                list->next = dupPivot;
                dupPivot = list;
        }
        list = next;
    }
    /*sort the lists in recursive calls */
    smallerElemList = quicksort(smallerElemList);
    largerElemList = quicksort(largerElemList);
    
    /* merge the sorted list and comparison element */
    if(smallerElemList != NULL)
    {
        end = smallerElemList;
        while(end->next != NULL){
            end=end->next;
        }
        pivotHead = dupPivot;
        while(dupPivot->next != NULL){
            dupPivot = dupPivot->next;
        }
        dupPivot->next = largerElemList;
        end->next = pivotHead;
        return smallerElemList;
    }
    else{
        pivotHead = dupPivot;
        while(dupPivot->next != NULL){
            dupPivot = dupPivot->next;
        }
        dupPivot->next = largerElemList;
        return pivotHead;
    }
}