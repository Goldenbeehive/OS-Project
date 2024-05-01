#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef short bool;
#define true 1
#define false 0

#define IGNORE_LENGTH 128
#define MAX_SIZE 1024
#define SHKEY 300


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
// This is our process struct, it encapsulates all the necessary data to describe a process
struct process
{
    int id;
    pid_t pid;
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
struct msg
{
    struct process * proc; 
};
struct msgbuff
{
    long mtype;
    int msg;
};
/**
 * @brief Function to initialize a process pointer given its data
 * 
 * @param id Process id after it gets forked
 * @param arrivaltime Arrival time that was set by the process generator
 * @param runningtime The time the process needs to run
 * @param priority Takes a priority value between 1 and 10, 1 being the highest priority and 10 being the lowest
 * @param memsize The memory that the process needs.
 * @return struct process* returns a pointer to the process created
 */
struct process initializeProcess(int id, int arrivaltime, int runningtime, int priority) {
    struct process p;
    p.id = id;
    p.arrivaltime = arrivaltime;
    p.runningtime = runningtime; // Corrected bursttime assignment
    p.priority = priority;
    p.remainingtime = runningtime;
    return p;
}

void testerfunction(struct process* p){
    printf("%d %d %d %d %d",p->id,p->arrivaltime,p->runningtime,p->remainingtime,p->priority);
    printf("\n");
}

/**
 * @brief Skips the first line of the file
 * 
 * @param f  The file pointer to the file you want to read from
 */
void skipLine(FILE* f){
    char ignore[IGNORE_LENGTH];
    fgets(ignore,IGNORE_LENGTH,f);
}

/**
 * @brief Gets the number of processes in the file
 * 
 * @param f The file pointer to the file you want to read from
 * @return int The number of processes in the file
 */
int getnoOfProcesses(FILE* f){
    int c;
    int count = 0;
    while ((c = fgetc(f)) != EOF) { if (c == '\n') { count++; } }
    fseek(f,0,SEEK_SET);
    return count;
}

/**
 * @brief  Define the keys for the message queues
 * 
 * @param ReadyQueueID  The ID of the Ready Queue
 * @param SendQueueID  The ID of the Send Queue
 * @param ReceiveQueueID  The ID of the Receive Queue
 */
void DefineKeys(int* ReadyQueueID, int* SendQueueID, int* ReceiveQueueID){
    key_t ReadyQueueKey;
    ReadyQueueKey= ftok("keys/Funnyman",'A');
    *ReadyQueueID = msgget(ReadyQueueKey, 0666 | IPC_CREAT);
    if (*ReadyQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //Initialize Send queue to send turn to process
    key_t SendQueueKey;
    SendQueueKey= ftok("keys/Sendman",'A');
    *SendQueueID = msgget(SendQueueKey, 0666 | IPC_CREAT);
    if (*SendQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //Initialize Receive queue to receive remaining time from process
    key_t ReceiveQueueKey;
    ReceiveQueueKey= ftok("keys/Receiveman",'A');
    *ReceiveQueueID = msgget(ReceiveQueueKey, 0666 | IPC_CREAT);
    if (*ReceiveQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
}
/**
 * @brief  Define the keys for the message queues
 * 
 * @param SendQueueID  The ID of the Send Queue 
 * @param ReceiveQueueID  The ID of the Receive Queue
 */
void DefineKeysProcess(int* SendQueueID, int* ReceiveQueueID){

    //Initialize Send queue to send turn to process
    key_t SendQueueKey;
    SendQueueKey= ftok("keys/Sendman",'A');
    *SendQueueID = msgget(SendQueueKey, 0666 | IPC_CREAT);
    if (*SendQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //Initialize Receive queue to receive remaining time from process
    key_t ReceiveQueueKey;
    ReceiveQueueKey= ftok("keys/Receiveman",'A');
    *ReceiveQueueID = msgget(ReceiveQueueKey, 0666 | IPC_CREAT);
    if (*ReceiveQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
}


#endif // HEADERS_H