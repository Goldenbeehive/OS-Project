#include "headers.h"
#include "RoundRobin.h"
// Function to initialize a process pointer given its data
struct process* initializeProcess(int id, int arrivaltime, int runningtime, int priority, int starttime, int endtime, int remainingtime, int waitingtime, int responsetime, int turnaroundtime, int isFinished, int isRunning, int isStarted) {
    struct process* p = malloc(sizeof(struct process));
    p->id = id;
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

int main(int argc, char * argv[])
{
    //initClk();
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    //destroyClk(true);
}