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
/**
 * @brief Function to initialize a process pointer given its data
 * 
 * @param id Process id after it gets forked
 * @param arrivaltime Arrival time that was set by the process generator
 * @param runningtime The time the process needs to run
 * @param priority Takes a priority value between 1 and 10, 1 being the highest priority and 10 being the lowest
 * @param starttime The time the process started running for the first time
 * @param endtime The time when the process finishes its running time
 * @param remainingtime The time remaining for the process to finish incase of pre-emption
 * @param waitingtime A calculated value for the time the process spent waiting in the ready queue
 * @param responsetime Difference between the time the process started and the time it was first scheduled
 * @param turnaroundtime Time take for the process to finish from the time it was created
 * @param isFinished Flag for the process to indicate if it has finished running
 * @param isRunning Flag for the process to indicate if it is currently running
 * @param isStarted Flag for the process to indicate if it has started running
 * @return struct process* returns a pointer to the process created
 */
struct process* initializeProcess(int id, int arrivaltime, int runningtime, int priority, int starttime, int endtime, int remainingtime, int waitingtime, int responsetime, int turnaroundtime, int isFinished, int isRunning, int isStarted) {
    struct process* p = malloc(sizeof(struct process));
    p->id = id;
    p->pid = getpid();
    p->arrivaltime = arrivaltime;
    p->runningtime = runningtime; // Corrected bursttime assignment
    p->priority = priority;
    p->starttime = starttime;
    p->endtime = endtime;
    p->remainingtime = remainingtime;
    p->waittime = waitingtime; // Added assignment for waittime
    p->responsetime = responsetime;
    p->turnaroundtime = turnaroundtime;
    p->flag = isFinished; // Renamed isFinished for consistency
    p->lasttime = isRunning; // Added assignment for lasttime
    p->flag = isStarted; // Added assignment for flag
    return p;
}
#endif // HEADERS_H