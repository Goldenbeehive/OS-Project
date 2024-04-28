#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularQueue.h"
char RunningTimeStr[20]; // Assuming 20 characters is enough for the string representation of currentProcess.runningtime
int ProcessFinished = 0;
struct process currentProcess;
struct CircularQueue *Running_Queue;
int remainingProcesses;
int quantumCounter;
pid_t pid;
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
                

                    pid = fork();
                    if (pid == -1) {
                        // Fork failed
                        perror("fork");
                    }
                    if (pid == 0) {
                        // Child process
                        RunningTimeStr[0] = '\0';
                        sprintf(RunningTimeStr, "%d", ArrivedProcess.runningtime);
                        printf("I'm child, my time is %s\n", RunningTimeStr);
                        char* args[] = {"./process.out", RunningTimeStr,NULL}; // NULL terminator required for the args array
                        execv(args[0], args);
                        printf("Execv failed\n");
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
                    printf("Current Process ID: %d,PID=%d\n",currentProcess.id,currentProcess.pid);
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


#include "headers.h"
#include "CircularQueue.h"

int main(int argc, char * argv[])
{
    
    printf("Scheduler\n");
    if (argc != 3)
    {
        perror("Invalid number of arguments");
        return -1;
    }
    printf("Number of arguments: %d\n", argc);
    for(int i=0;i<argc;i++)
    {
        printf("Argument %d: %s\n",i,argv[i]);
    }
    initClk();
    printf("Clock initialized in scheduler\n");
    //TODO implement the scheduler :)
    int quantum = atoi(argv[2]);
    printf("Quantum: %d\n", quantum);
    int processCount = atoi(argv[1]);
    printf("Process Count: %d\n", processCount);
    int SelectedAlgorithm = atoi(argv[0]);
    printf("Selected Algorithm: %d\n", SelectedAlgorithm);
    if (SelectedAlgorithm == 1)
    {
        RoundRobin(quantum, processCount);
    }
    else if (SelectedAlgorithm == 2)
    {
        //TODO implement the other algorithms here
    }
    else if (SelectedAlgorithm == 3)
    {
        //TODO implement the other algorithms here
    }
    else
    {
        printf("Invalid Algorithm\n");
    }

    //upon termination release the clock resources.
    
    destroyClk(true);
}







