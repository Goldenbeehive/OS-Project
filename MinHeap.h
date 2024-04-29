#ifndef MINHEAP_H
#define MINHEAP_H
#include "headers.h"

struct MinHeap {
    struct process *harr;
    int capacity;
    int heap_size;
};

struct MinHeap* createMinHeap(int capacity);
void MinHeapifySRTN(struct MinHeap* minHeap, int i);
void MinHeapifyHPF(struct MinHeap* minHeap, int i);
int parent(int i);
int left(int i);
int right(int i);
void Remove(struct MinHeap* minHeap, struct process currentProcess);
struct process extractMin(struct MinHeap* minHeap,bool SRTN);
struct process getMin(struct MinHeap* minHeap);
void insertSRTN(struct MinHeap* minHeap, struct process k);
void insertHPF(struct MinHeap* minHeap, struct process k);
void destroy(struct MinHeap* minHeap);

/**
 * @brief Destroy the MinHeap object
 * 
 * @param minHeap  The MinHeap to be destroyed
 */
void destroy(struct MinHeap* minHeap){
    free(minHeap->harr);
    free(minHeap);
}

/**
 * @brief Create a Min Heap object
 * 
 * @param capacity The capacity of the heap 
 * @return struct MinHeap* 
 */
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->heap_size = 0;
    minHeap->capacity = capacity;
    minHeap->harr = (struct process*)malloc(capacity * sizeof(struct process));
    return minHeap;
}

/**
 * @brief The SRTN MinHeapify function
 * 
 * @param minHeap  The MinHeap to be heapified 
 * @param i  The index to start heapifying from
 */
void MinHeapifySRTN(struct MinHeap* minHeap, int i)
{
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < minHeap->heap_size && minHeap->harr[l].remainingtime < minHeap->harr[i].remainingtime)
        smallest = l;
    if (r < minHeap->heap_size && minHeap->harr[r].remainingtime < minHeap->harr[smallest].remainingtime)
        smallest = r;
    if (smallest != i){
        struct process temp = minHeap->harr[i];
        minHeap->harr[i] = minHeap->harr[smallest];
        minHeap->harr[smallest] = temp;
        MinHeapifySRTN(minHeap, smallest);
    }
}

/**
 * @brief  The HPF MinHeapify function
 * 
 * @param minHeap  The MinHeap to be heapified
 * @param i  The index to start heapifying from
 */
void MinHeapifyHPF(struct MinHeap* minHeap, int i){
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < minHeap->heap_size && minHeap->harr[l].priority < minHeap->harr[i].priority)
        smallest = l;
    if (r < minHeap->heap_size && minHeap->harr[r].priority < minHeap->harr[smallest].priority)
        smallest = r;
    if (smallest != i){
        struct process temp = minHeap->harr[i];
        minHeap->harr[i] = minHeap->harr[smallest];
        minHeap->harr[smallest] = temp;
        MinHeapifyHPF(minHeap, smallest);
    }
}
/**
 * @brief  Finds the index of the parent node given the index of the child node
 * 
 * @param i The index of the child node 
 * @return int 
 */
int parent(int i){ return (i - 1) / 2; }

/**
 * @brief  Finds the index of the left child node given the index of the parent node
 * 
 * @param i  The index of the parent node
 * @return int 
 */
int left(int i){ return (2 * i + 1); }

/**
 * @brief  Finds the index of the right child node given the index of the parent node
 * 
 * @param i  The index of the parent node
 * @return int 
 */
int right(int i){ return (2 * i + 2); }

struct process extractMin(struct MinHeap* minHeap,bool SRTN){
    if (minHeap->heap_size == 1)
    {
        minHeap->heap_size--;
        return minHeap->harr[0];
    }
    struct process root = minHeap->harr[0];
    minHeap->harr[0] = minHeap->harr[minHeap->heap_size - 1];
    minHeap->heap_size--;
    if (SRTN){ MinHeapifySRTN(minHeap, 0); }
    else { MinHeapifyHPF(minHeap, 0); }
    return root;
}

/**
 * @brief  Remove a process from the MinHeap
 * 
 * @param minHeap  The MinHeap to remove the process from
 * @param currentProcess  The process to be removed
 */
void Remove(struct MinHeap* minHeap, struct process currentProcess){
    int i = 0;
    for (i = 0; i < minHeap->heap_size; i++) {
        if (minHeap->harr[i].id == currentProcess.id)
            break;
    }
    minHeap->harr[i] = minHeap->harr[minHeap->heap_size - 1];
    minHeap->heap_size--;
    MinHeapifyHPF(minHeap,0);
}

/**
 * @brief Get the Min object
 * 
 * @param minHeap  The MinHeap to get the minimum from
 * @return struct process 
 */
struct process getMin(struct MinHeap* minHeap){ return minHeap->harr[0]; }


/**
 * @brief  Insert a process in the MinHeap
 * 
 * @param minHeap  The MinHeap to insert the process in
 * @param k  The process to be inserted
 */
void insertSRTN(struct MinHeap* minHeap, struct process k)
{
    if (minHeap->heap_size == minHeap->capacity)
    {
        printf("\nOverflow: Could not insertKey\n");
        return;
    }
    minHeap->heap_size++;
    int i = minHeap->heap_size - 1;
    minHeap->harr[i] = k;
    while (i != 0 && minHeap->harr[parent(i)].remainingtime > minHeap->harr[i].remainingtime)
    {
        struct process temp = minHeap->harr[i];
        minHeap->harr[i] = minHeap->harr[parent(i)];
        minHeap->harr[parent(i)] = temp;
        i = parent(i);
    }
}

/**
 * @brief  Insert a process in the MinHeap
 * 
 * @param minHeap  The MinHeap to insert the process in
 * @param k  The process to be inserted
 */
void insertHPF(struct MinHeap* minHeap, struct process k)
{
    if (minHeap->heap_size == minHeap->capacity)
    {
        printf("\nOverflow: Could not insertKey\n");
        return;
    }
    minHeap->heap_size++;
    int i = minHeap->heap_size - 1;
    minHeap->harr[i] = k;
    while (i != 0 && minHeap->harr[parent(i)].priority > minHeap->harr[i].priority)
    {
        struct process temp = minHeap->harr[i];
        minHeap->harr[i] = minHeap->harr[parent(i)];
        minHeap->harr[parent(i)] = temp;
        i = parent(i);
    }
}
#endif // MINHEAP_H