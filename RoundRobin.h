#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularQueue.h"
int ProcessFinished = 0;
void RoundRobin(int quantum, int processCount)
{
    // Attach to the ready queue
    key_t ReadyQueueKey;
    ReadyQueueKey= ftok("Funnyman",'A');
    int ReadyQueueID = msgget(ReadyQueueKey, 0666 | IPC_CREAT);
    if (ReadyQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    struct process *ArrivedProcess =NULL;
    // Create necessary variables and queues
    struct CircularQueue *Running_Queue = createQueue(processCount);
    int clk = getClk();
    int quantumCounter = 0;
    int remainingProcesses = processCount;
    int previousClk;
    struct process *currentProcess;
    // While there are still processes in the running queue
    while (remainingProcesses != 0)
    {
        clk = getClk();
        // Check if there are any new processes
        ArrivedProcess = NULL;
        while(CheckArrivedProcesses(Running_Queue,ArrivedProcess,ReadyQueueID))
        {
            if (!isFull(Running_Queue))
            {
                enqueue(Running_Queue, ArrivedProcess);
                remainingProcesses--;
                ArrivedProcess = NULL;
                ArrivedProcess->pid= ForkProcess(ArrivedProcess->runningtime);
                kill(ArrivedProcess->pid, SIGSTOP);
            }
            else
            {
                printf("Queue is full\n");
            }
        }      
        // If we have processes in running queue, We process them 
        if (!isEmpty(Running_Queue))
        {
            //Check if the process finished its quantum
            if (quantumCounter > quantum)
            {
                quantumCounter = 0;
                if (!isEmpty(Running_Queue))
                {
                    currentProcess = getCurrent(Running_Queue);
                    kill(currentProcess->id, SIGSTOP);
                    changeCurrent(Running_Queue);
                }
            }
            currentProcess = getCurrent(Running_Queue);
            if (currentProcess != NULL)
            {
                kill(currentProcess->id, SIGCONT);
            }
            quantumCounter++;
        }
        while (getClk() == clk)
        {
            // Do nothing
        }
    }
}

/**
 * @brief Checks if a process has arrived in the ready queue 
 * 
 * @param RunningQueue The Running queue of the scheduler
 * @param ArrivedProcesses Pointer to the process struct that will be filled with the arrived process
 * @param ReadyQueueID The ready queue id of message queue to be checked
 * @return int , 1 if a process has arrived, 0 if no process has arrived
 */
int CheckArrivedProcesses(struct CircularQueue *RunningQueue,struct process *ArrivedProcesses, int ReadyQueueID)
{
        int received = msgrcv(ReadyQueueID, ArrivedProcesses, sizeof(ArrivedProcesses), 0, IPC_NOWAIT);
    if(received!=-1 || ArrivedProcesses!=NULL)
    {
        printf("Received Process with ID: %d\n",ArrivedProcesses->id);
        return 1;
    }
    else
    {
        printf("No Process Received\n");
        return 0;
    }
}
/**
 * @brief Creates a new process and forks it
 * 
 * @param RunningTime The amount of time the process will run
 * @return int returns the pid of the forked process if successful, -1 if failed
 */
int ForkProcess(int RunningTime)
{
    int pid = fork();
    if (pid == -1)
    {
        perror("Error in forking the process");
    }
    if (pid == 0)
    {
        char RunningTimeStr[12];
        sprintf(RunningTimeStr, "%d", RunningTime);
        char *args[] = {"./process.out", RunningTimeStr, NULL};
        execvp(args[0], args);
    }
    return pid;
}
#endif