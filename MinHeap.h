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
struct process extractMin(struct MinHeap* minHeap,bool SRTN);
//void decreaseKey(struct MinHeap* minHeap, int i, int new_val);
struct process getMin(struct MinHeap* minHeap);
//void deleteKey(struct MinHeap* minHeap, int i);
void insertKeySRTN(struct MinHeap* minHeap, struct process k);
void insertKeyHPF(struct MinHeap* minHeap, struct process k);
void destroy(struct MinHeap* minHeap);

void destroy(struct MinHeap* minHeap){
    free(minHeap->harr);
    free(minHeap);
}

struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->heap_size = 0;
    minHeap->capacity = capacity;
    minHeap->harr = (struct process*)malloc(capacity * sizeof(struct process));
    return minHeap;
}

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

int parent(int i){ return (i - 1) / 2; }

int left(int i){ return (2 * i + 1); }

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

// void decreaseKey(struct MinHeap* minHeap, int i, int new_val)
// {
//     minHeap->harr[i] = new_val;
//     while (i != 0 && minHeap->harr[parent(i)] > minHeap->harr[i])
//     {
//         struct process temp = minHeap->harr[i];
//         minHeap->harr[i] = minHeap->harr[parent(i)];
//         minHeap->harr[parent(i)] = temp;
//         i = parent(i);
//     }
// }

struct process getMin(struct MinHeap* minHeap){ return minHeap->harr[0]; }

// void deleteKey(struct MinHeap* minHeap, int i)
// {
//     decreaseKey(minHeap, i, INT_MIN);
//     extractMin(minHeap);
// }

void insertKeySRTN(struct MinHeap* minHeap, struct process k)
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

void insertKeyHPF(struct MinHeap* minHeap, struct process k)
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