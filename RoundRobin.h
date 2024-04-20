#include"headers.h"
struct CircularQueue
{
    int front, rear, size, current;
    int capacity;
    struct process *array;
};
struct process
{
    int id;
    int arrivaltime;
    int runningtime;
    int priority;
    int starttime;
    int endtime;
    int remainingtime;
    int waittime;
    int responsetime;
    int finishtime;
    int turnaroundtime;
    int lasttime;
    int flag;
};

/**
 * @brief Create a Queue object
 * 
 * @param capacity Enter the max number of processes that the queue can hold
 * @return struct CircularQueue*  
 */
struct CircularQueue *createQueue(int capacity)
{
    struct CircularQueue *queue = (struct CircularQueue *)malloc(sizeof(struct CircularQueue));
    queue->capacity = capacity;
    queue->front = queue->size = queue->current = queue->rear = 0;
    queue->array = (struct process *)malloc(queue->capacity * sizeof(struct process));
    return queue;
}

/**
 * @brief Checks if the queue is full
 * 
 * @param queue Queue to check if it's full
 * @return int 1 if the queue is full, 0 otherwise 
 */
int isFull(struct CircularQueue *queue)
{
    return (queue->size == queue->capacity);
}

/**
 * @brief Checks if the queue is empty
 * 
 * @param queue Queue to check if it's empty
 * @return int 1 if the queue is empty, 0 otherwise 
 */
int isEmpty(struct CircularQueue *queue)
{
    return (queue->size == 0);
}

/**
 * @brief Removes the first process in the queue
 * 
 * @param queue Queue to remove the process from
 * @param item Returns the removed process if the queue is not empty, NULL otherwise
 * @return -1 if the queue is empty, 0 if the process was removed successfully 
 */
int dequeue(struct CircularQueue *queue, struct process *item)
{
    if (isEmpty(queue)){
        item=NULL;
        return -1;
    }
    *item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return 0;
}

/**
 * @brief Inserts process item at the end of the queue
 * 
 * @param queue Queue to insert the process in
 * @param item Process being inserted in the queue
 * @return -1 if the queue is full, 0 if the process was inserted successfully 
 */
int enqueue(struct CircularQueue *queue, struct process item)
{
    if (isFull(queue))
        return -1;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    return 0;
}

/**
 * @brief Moves pointer to the next process in the queue
 * 
 * @param queue The queue we want to move the pointer in
 * @return 0 if the pointer was moved, -1 if the queue is empty 
 */
int changeCurrent(struct CircularQueue *queue){
    if(isEmpty(queue))
        return -1;
    if(queue->current==queue->rear)
        {queue->current= queue->front;}
    else        
        {queue->current= (queue->current+1)%queue->capacity;}
    return 0;
}

/**
 * @brief Returns the currently running process
 * 
 * @param queue The queue we want to get the current process from
 * @return struct process 
 */
struct process getCurrent(struct CircularQueue *queue)
{
    return queue->array[queue->current];
}

/**
 * @brief Destroys the queue and frees the memory
 * 
 * @param queue The queue we want to destroy
 */
void destroyQueue(struct CircularQueue *queue)
{
    free(queue->array);
    free(queue);
}
