#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularQueue.h"
int ProcessFinished = 0;
struct process currentProcess;
struct CircularQueue *Running_Queue;
int remainingProcesses;
int quantumCounter;
void MySigHandler(int signum)
{
    printf("Process with ID: %d has finished\n",currentProcess.id);
    quantumCounter = 0;
    remainingProcesses--;
    struct process TerminatedProcess;
    RemoveCurrent(Running_Queue,&TerminatedProcess);
    printf("Process with ID: %d has been removed from the queue\n",TerminatedProcess.id);
    printQueue(Running_Queue);
}
/**
 * @brief The Round Robin Scheduler
 * 
 * @param quantum The quantum of the Round Robin Scheduler
 * @param processCount The number of processes to be scheduled
 */
void RoundRobin(int quantum, int processCount)
{
    printf("Round Robin Scheduler\n");
    signal(SIGUSR1,&MySigHandler);
    int HasArrivedArray[processCount+1];
    for(int i=0;i<=processCount;i++)
    {
        HasArrivedArray[i]=0;
    }
    // Attach to the ready queue
    key_t ReadyQueueKey;
    ReadyQueueKey= ftok("Funnyman",'A');
    int ReadyQueueID = msgget(ReadyQueueKey, 0666 | IPC_CREAT);
    printf("Ready Queue ID: %d\n",ReadyQueueID);
    if (ReadyQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    struct process ArrivedProcess;
    quantumCounter = 0;
    remainingProcesses = processCount;
    Running_Queue = createQueue(processCount);
    // While there are still processes in the running queue
    while (remainingProcesses > 0)
    {
        int clk = getClk();
        printf("Current Clk: %d\n",clk);
        // Check if there are any new processes
        
        
    
        bool StillArriving = true;
        while(StillArriving)
        {
            struct process rec;
            int received = msgrcv(ReadyQueueID, &rec, sizeof(rec), 0, IPC_NOWAIT);
            if (received != -1)
            {
                printf("Received Process with ID: %d\n",rec.id);
                ArrivedProcess = rec;
                ArrivedProcess.runningtime = rec.runningtime;
                ArrivedProcess.arrivaltime = rec.arrivaltime;
                ArrivedProcess.id = rec.id;
                ArrivedProcess.priority = rec.priority;
                printf("Arrival Time: %d\n",ArrivedProcess.arrivaltime);
                

                    pid_t pid = fork();
                    if (pid == -1) {
                        // Fork failed
                        perror("fork");
                    }
                    if (pid == 0) {
                        // Child process
                        char RunningTimeStr[20]; // Assuming 20 characters is enough for the string representation of currentProcess.runningtime
                        sprintf(RunningTimeStr, "%d", ArrivedProcess.runningtime);
                        printf("I'm child, my time is %s\n", RunningTimeStr);
                        char* args[] = {"./process.out", RunningTimeStr, NULL, NULL}; // NULL terminator required for the args array
                        execv(args[0], args);
                        // If execv returns, it means there was an error
                        perror("execv");
                        exit(EXIT_FAILURE); // Exit child process with failure
                    }



                HasArrivedArray[ArrivedProcess.id] = 1;
                printf("Process with PID: %d has been forked\n",pid);
                ArrivedProcess.pid = pid;
                printf("Process with ID: %d has been enqueued\n",ArrivedProcess.id);
                enqueue(Running_Queue, ArrivedProcess);
                kill(pid,SIGUSR1);
                StillArriving = true;
            }
            else
            {
                StillArriving = false;
                printf("No Process Received\n");
            }
        }
        if(!isEmpty(Running_Queue))
        {
            // Check if the quantum has finished
            if(quantumCounter == quantum)
            {
                printf("Quantum has finished\n");
                quantumCounter = 0;
                printf("Process with ID: %d has been stopped\n",currentProcess.pid);
                kill(currentProcess.pid,SIGUSR1);
                printQueue(Running_Queue);
                changeCurrent(Running_Queue);
            }
            else
            {   
                
                if (getCurrent(Running_Queue,&currentProcess))
                {
                    printf("Current Process ID: %d\n",currentProcess.id);
                    quantumCounter++;
                    kill(currentProcess.pid, SIGUSR2);
                    if (HasArrivedArray[currentProcess.id] == 0)
                    {
                        currentProcess.starttime = clk;
                        HasArrivedArray[currentProcess.id] = 1;
                    }
                }
            }
        }
        else
        {
            printf("No Processes in the running queue\n");
        }
        while (getClk() == clk)
        {
           // printf("Waiting for a tick\n");
        }
    }
}
#endif
int main()
{
    initClk();
    
    key_t ReadyQueueKey;
    ReadyQueueKey= ftok("Funnyman",'A');
    int ReadyQueueID = msgget(ReadyQueueKey, 0666 | IPC_CREAT);
    struct process dummy=initializeProcess(1,8,19,3);
    struct process dummy2=initializeProcess(2,10,1,1);
    msgsnd(ReadyQueueID, &dummy, sizeof(struct process),IPC_NOWAIT);
    msgsnd(ReadyQueueID, &dummy2, sizeof(struct process),IPC_NOWAIT);
    RoundRobin(4,2);
    msgctl(ReadyQueueID, IPC_RMID,NULL);
    destroyClk(true);
    return 0;
}
