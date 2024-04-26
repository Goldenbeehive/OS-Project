#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularQueue.h"
int ProcessFinished = 0;
struct process *currentProcess;
struct CircularQueue *Running_Queue;
int CheckArrivedProcesses(struct CircularQueue *RunningQueue,struct process *ArrivedProcesses, int ReadyQueueID);
int ForkProcess(int RunningTime);

void MySigHandler(int signum)
{
    ProcessFinished = 1;
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
    signal(SIGCHLD, MySigHandler);
    int HasArrivedArray[processCount];
    for(int i=0;i<processCount;i++)
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
    struct process *ArrivedProcess =NULL;
    // Create necessary variables and queues
    printf("Creating Running Queue\n");
    Running_Queue = createQueue(processCount);
    int clk = getClk();
    int quantumCounter = 0;
    int remainingProcesses = processCount;
    int previousClk;
    // While there are still processes in the running queue
    while (remainingProcesses != 0)
    {
        clk = getClk();
        printf("Current Clk: %d\n",clk);
        // Check if there are any new processes
        ArrivedProcess = NULL;
        bool ay7aga = true;
        while(ay7aga)
        {
            printf("weslt\n");
            struct process rec;
            int received = msgrcv(ReadyQueueID, &rec, sizeof(rec), 0, IPC_NOWAIT);
            if(received!=-1)
            {
                ArrivedProcess = malloc(sizeof(struct process));
                ArrivedProcess->id = rec.id;
                ArrivedProcess->pid = getpid();
                ArrivedProcess->arrivaltime = rec.arrivaltime;
                ArrivedProcess->runningtime = rec.runningtime; // Corrected bursttime assignment
                ArrivedProcess->priority = rec.priority;
                ArrivedProcess->remainingtime = rec.runningtime;
                printf("Received Process with ID: %d\n",ArrivedProcess->id);
                ay7aga = true;
            }
            else
            {
                printf("No Process Received\n");
                ay7aga = false;
            }
            if (!isFull(Running_Queue) && ay7aga)
            {
                printf("Adding Process to Running Queue\n");
                enqueue(Running_Queue, ArrivedProcess);
                ArrivedProcess->pid= ForkProcess(ArrivedProcess->runningtime);
                kill(ArrivedProcess->pid, SIGSTOP);
                ArrivedProcess = NULL;
            }
            else
            {
                printf("ay 7aga tnya\n");
            }
        }      
        // If we have processes in running queue, We process them 
        if (!isEmpty(Running_Queue))
        {
            //Check if the process finished its quantum
            if (quantumCounter > quantum || ProcessFinished == 1) 
            {
                printf("Quantum Finished\n");
                quantumCounter = 0;
                if (!isEmpty(Running_Queue))
                {
                    currentProcess = getCurrent(Running_Queue);
                    if(!ProcessFinished){
                    kill(currentProcess->id, SIGSTOP);
                    }
                    else
                    {
                        ProcessFinished = 0;
                        printf("Process with ID: %d has finished\n",currentProcess->id);
                        struct process *FinishedProcess = RemoveCurrent(Running_Queue);
                        FinishedProcess->endtime = getClk();
                        remainingProcesses--;
                        free(FinishedProcess);
                    }
                    changeCurrent(Running_Queue);
                }
            }
            currentProcess = getCurrent(Running_Queue);
            if (currentProcess != NULL)
            {
                kill(currentProcess->id, SIGCONT);
                printf("Process with ID: %d is running\n", currentProcess->id);
                if(HasArrivedArray[currentProcess->id]==0)
                {
                    currentProcess->starttime = getClk();
                    HasArrivedArray[currentProcess->id]=1;
                }
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
    //set kaza b kaza;
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
        execv(args[0], args);
    }
    return pid;
}
#endif