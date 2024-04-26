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
void initializeProcessptr(int id, int arrivaltime, int runningtime, int priority, struct process *p){
    p = malloc(sizeof(struct process));
    p->id = id;
    p->pid = getpid();
    p->arrivaltime = arrivaltime;
    p->runningtime = runningtime; // Corrected bursttime assignment
    p->priority = priority;
    p->remainingtime = runningtime;
    return;
}
struct process initializeProcess(int id, int arrivaltime, int runningtime, int priority) {
    struct process p;
    p.id = id;
    p.pid = getpid();
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

void skipLine(FILE* f){
    char ignore[IGNORE_LENGTH];
    fgets(ignore,IGNORE_LENGTH,f);
}
int getnoOfProcesses(FILE* f){
    int c;
    int count = 0;
    while ((c = fgetc(f)) != EOF) { if (c == '\n') { count++; } }
    fseek(f,0,SEEK_SET);
    return count;
}



#endif // HEADERS_H