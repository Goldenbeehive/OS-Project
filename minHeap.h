#ifndef MINHEAP_H
#define MINHEAP_H 

#include "headers.h"
struct minHeap
{
    struct process ** harr;
    int capacity;
    int heap_size;
};
void insertKey(process* k , struct minHeap * minHeap,int selector);
/**
 * @brief minHeap constructor 
 * 
 * @param capacity the maximum number of Nodes i want to handle 
 * @return struct minHeap* pointer to the created minHeap 
 */
struct minHeap* createMinHeap(int capacity){ 
    if(capacity = 0){
        printf("createMainHeap FAILED\n not valid capacity \n");
        exit(1);
    }
    struct minHeap *minHeap = (struct minHeap *)malloc(sizeof(struct minHeap));
    minHeap->capacity = capacity;
    minHeap->heap_size = 0;
    minHeap->harr = (struct process **)malloc(sizeof(struct process*));
}
struct minHeap * createMinHeap(int capacity , struct process ** harrG,int harrGSize,int selector){
    if(capacity = 0){
        printf("createMainHeap FAILED\n not valid capacity \n");
        exit(1);
    }
    struct minHeap *minHeap = (struct minHeap *)malloc(sizeof(struct minHeap));
    minHeap->capacity = capacity;
    minHeap->heap_size = 0;
    minHeap->harr = (struct process **)malloc(sizeof(struct process*));
    for(int i = 0; i < harrGSize; i++){
        insertKey(harrG[i],minHeap,selector);
    }
}
/**
 * @brief Utility Function to Swap 2 processes 
 * 
 * @param x First process 
 * @param y Second process
 */
void swap(process* x,process* y){
    process* temp = (process*)malloc(sizeof(process));
    temp = x;
    x = y;
    y = temp;
    free(temp);
}
/**
 * @brief Checks if the minHeap is empty
 * 
 * @param minHeap The heap to be checked
 * @return Returns 1 if the heap is empty, otherwise 0
 */
int isEmpty(minHeap* minHeap){
    if(minHeap->heap_size == 0){
        return 1;
    }  
    return 0;
}
int isEmpty(minHeap* minHeap){
    if(minHeap->heap_size == 0){
        return 1;
    }  
    return 0;
}
/**
 * @brief Checks if the minHeap is full
 * 
 * @param minHeap The heap to be checked
 * @return Returns 1 if the heap is full, otherwise 0
 */
int isFull(minHeap* minHeap){
    if(minHeap->heap_size == minHeap->capacity){
        return 1;
    }  
    return 0;
}
/**
 * @brief Calculates the index of the parent node of a given node in the heap.
 * 
 * @param i The index of the child node.
 * @return The index of the parent node.
 */
int parent(int i){return (i-1)/2;}

/**
 * @brief Calculates the index of the left child node of a given node in the heap.
 * 
 * @param i The index of the parent node.
 * @return The index of the left child node.
 */
int left(int i ){return (2*i+1);}

/**
 * @brief Calculates the index of the right child node of a given node in the heap.
 * 
 * @param i The index of the parent node.
 * @return The index of the right child node.
 */
int right(int i ){return (2*i+2);}


/**
 * @brief Heapify the tree from root to leaf Recursively 
 * 
 * @param minHeap The Heap i need to operate on 
 * @param i recursive index 
 * @param selector Priority or remaining time 
 */
void minHeapify(struct minHeap *minHeap,int i,int selector){
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if(selector == 0){
    if(l < minHeap->heap_size && minHeap->harr[l]->remainingtime< minHeap->harr[i]->remainingtime){
    smallest = l;
    }
    if(r < minHeap->heap_size && minHeap->harr[r]->remainingtime< minHeap->harr[i]->remainingtime){
    smallest = r;
    }
    }
    else if(selector == 1 ){   
    if(l < minHeap->heap_size && minHeap->harr[l]->priority> minHeap->harr[i]->priority){
    smallest = l;
    }
    if(r < minHeap->heap_size && minHeap->harr[r]->priority> minHeap->harr[i]->priority){
    smallest = r;
    }

    }
    if(smallest!= i ){   
        swap(minHeap->harr[i],minHeap->harr[smallest]);
        minHeapify(minHeap,smallest,selector);
    }
}
/**
 * @brief The function Insert a new node to the Heap and ensure it wasn't messed up 
 * 
 * @param k The inserted Process
 * @param minHeap The heap i want to insert into
 * @param selector based on Priority or remaining time
 */
void insertKey(process* k , struct minHeap * minHeap,int selector){
    if(minHeap->heap_size == minHeap->capacity){
        printf("Heap Is Full No Please No\n");
        exit(1);
    }
    minHeap->heap_size +=1;
    int i = minHeap->heap_size - 1;
    minHeap->harr[i] = k;
    if(selector==0){
    while (i != 0 && minHeap->harr[parent(i)]->remainingtime > minHeap->harr[i]->remainingtime) 
    { 
        swap(minHeap->harr[i], minHeap->harr[parent(i)]); 
        i = parent(i); 
    } 
    }
    else if(selector==0){
        while (i != 0 && minHeap->harr[parent(i)]->priority < minHeap->harr[i]->priority) 
        { 
            swap(minHeap->harr[i], minHeap->harr[parent(i)]); 
            i = parent(i); 
        } 
    }
}
/**
 * @brief (For SRTN) its job is to update the remaining time of a certain Node (most of the time it will be the root )
 * 
 * @param i The index of the Node you want to update (0 for root)
 * @param newValue the value you want to decrease it 
 * @param minHeap The heap you want to operate this function on 
 */
void ConSumeKeyValue(int i , int newValue,struct minHeap* minHeap){
    minHeap->harr[i]->remainingtime -= newValue;
    while (i != 0 && minHeap->harr[parent(i)]->remainingtime > minHeap->harr[i]->remainingtime) 
    { 
        swap(minHeap->harr[i], minHeap->harr[parent(i)]); 
        i = parent(i); 
    } 
}
/**
 * @brief Extracts the smallest process from the heap and updates the heap accordingly.
 * 
 * @param minHeap The heap from which the smallest process will be extracted.
 * @param selector The priority or remaining time of the processes.
 * @return The smallest process in the heap.
 */
struct process* extractMin(minHeap* minHeap,int selector){
    if(minHeap->heap_size <= 0)
        return NULL;
    
    if(minHeap->heap_size == 1){
        minHeap->heap_size = 0;
        return minHeap->harr[0];
    }
    struct process* root = (struct process*)malloc(sizeof(struct process));
    root = minHeap->harr[0];
    minHeap->harr[0] = minHeap->harr[minHeap->heap_size - 1];
    free(minHeap->harr[minHeap->heap_size - 1]);
    minHeap->heap_size -=1;
    minHeapify(minHeap,0,selector);
    return root;
}
#endif MINHEAP_H
