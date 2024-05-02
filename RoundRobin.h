#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularList.h"
#include <math.h>
#include"time.h"
/**
 * @brief Clears the contents of the log file named "scheduler.log"
 *
 * @return void
 */
void clearLogFileRR()
{
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "w");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }

    fclose(filePointer);
}
/**
 * @brief Logs the start time and details of a process to the "scheduler.log" file
 *
 * @param proc The process structure containing details of the process being logged
 * @return void
 */
void LogStartedRR(struct process proc)
{

    int clock = getClk();
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "a");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }
    if (proc.remainingtime != proc.runningtime)
    {
        fprintf(filePointer, "At time %d, process %d resumed. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
                clock, proc.id, proc.arrivaltime, proc.remainingtime, proc.runningtime, clock - proc.arrivaltime - proc.runningtime + proc.remainingtime);
    }
    else
    {
        fprintf(filePointer, "At time %d, process %d started. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
                clock, proc.id, proc.arrivaltime, proc.remainingtime, proc.runningtime, clock - proc.arrivaltime);
    }

    fclose(filePointer);
}

/**
 * @brief Logs the finish time, turnaround time, and details of a process to the "scheduler.log" file
 *
 * @param proc The process structure containing details of the finished process
 * @param noOfProcesses The total number of processes
 * @return void
 */
void LogFinishedRR(struct process proc, int noOfProcesses, int *runningTimeSum, float *WTASum, int *waitingTimeSum, float TAArray[], int *TAArrayIndex)
{

    int clock = getClk();
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "a");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }
    if (proc.remainingtime == 0)
    {

        fprintf(filePointer, "At time %d, process %d finished. Arr: %d, remain: %d,Total:%d, wait: %d. TA %d WTA %.2f\n",
                clock, proc.id, proc.arrivaltime, proc.remainingtime, proc.runningtime, clock - proc.arrivaltime - proc.runningtime, clock - proc.arrivaltime, ((float)clock - proc.arrivaltime) / (float)proc.runningtime);
        *runningTimeSum += proc.runningtime;
        *WTASum += ((float)clock - proc.arrivaltime) / (float)proc.runningtime;
        *waitingTimeSum += clock - proc.arrivaltime - proc.runningtime;
        TAArray[*TAArrayIndex] = ((float)clock - proc.arrivaltime) / (float)proc.runningtime;
        *TAArrayIndex = *TAArrayIndex + 1;
    }
    else
    {
        fprintf(filePointer, "At time %d, process %d stopped. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
                clock, proc.id, proc.arrivaltime, proc.remainingtime, proc.runningtime, clock - proc.arrivaltime - proc.runningtime + proc.remainingtime);
    }
    fclose(filePointer);
}
void RoundRobin(int quantum, int processCount)
{
    printf("Round Robin Scheduler\n");
    clearLogFileRR();
    float TAArray[processCount];
    int TAArrayIndex = 0;
    int runningTimeSum = 0;
    float WTASum = 0.0f;
    int waitingTimeSum = 0;
    int HasStartedArray[processCount + 1];
    for (int i = 0; i <= processCount; i++)
    {
        HasStartedArray[i] = 0;
    }
    // Initialize Ready queue to receive processes from process generator
    int ReadyQueueID, SendQueueID, ReceiveQueueID;
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    int quantumCounter = 0;
    int remainingProcesses = processCount;
    struct CircularList *Running_List = createCircularList();
    int clk = 0;
    // Main processing loop, keeps running until all processes are finished
    while (remainingProcesses > 0)
    {
        clk = getClk();
        // Prints the current cycle
        printf("Current Clk: %d\n", clk);
        // Check if there are any new processes
        bool StillArriving = true;
        while (StillArriving)
        {
            struct process rec;
            // Checks for processes arriving from the process generator
            printf("Checking for new processes\n");
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 1;
            nanosleep(&req, NULL);
            int received = msgrcv(ReadyQueueID, &rec, sizeof(rec), 0, IPC_NOWAIT);
            if (received != -1)
            {
                // If process was received, add it to the running list and Fork it to start its execution
                printf("Process with ID: %d has arrived\n", rec.id);
                pid_t pid = fork();
                if (pid == -1)
                {
                    // Fork failed
                    perror("fork");
                }
                if (pid == 0)
                {
                    // This is the code executed by the child, which will be replaced by the process
                    char RunningTimeStr[20]; // Assuming 20 characters is enough for the string representation of currentProcess.runningtime
                    sprintf(RunningTimeStr, "%d", rec.runningtime);
                    // printf("I'm child, my time is %s\n", RunningTimeStr);
                    char *args[] = {"./process.out", RunningTimeStr, NULL, NULL}; // NULL terminator required for the args array
                    execv(args[0], args);
                    // If execv returns, it means there was an error
                    perror("execv");
                    exit(EXIT_FAILURE); // Exit child process with failure
                }
                rec.pid = pid; // Assign the PID of the child process to the process struct
                insertAtEnd(Running_List, rec);
                displayList(Running_List);
            }
            else
            {
                StillArriving = false;
            }
        }
        if (!isEmpty(Running_List))
        {
            // Checks for remaining time from the process
            struct msgbuff receivedmsg;
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1)
            {
                // printf("Received remaining time %d from process with PID: %ld\n",receivedmsg.msg,receivedmsg.mtype);
                struct process p;
                getCurrent(Running_List, &p);
                // Updates the remaining time of the process
                p.remainingtime = receivedmsg.msg;
                changeCurrentData(Running_List, p);
                if (p.remainingtime == 0)
                {
                    // If the process has finished, remove it from the running list
                    printf("Process with ID: %d has finished\n", p.id);
                    LogFinishedRR(p, processCount, &runningTimeSum, &WTASum, &waitingTimeSum, TAArray, &TAArrayIndex);
                    struct process Terminated;
                    removeCurrent(Running_List, &Terminated);
                    displayList(Running_List);
                    remainingProcesses--;
                    quantumCounter = 0;
                    // LogStartedRR(Running_List->current->data);
                    wait(NULL);
                }
            }
            if (quantumCounter == quantum)
            {
                // If the quantum has finished, change the current process
                // printf("Quantum has finished\n");
                quantumCounter = 0;
                struct process temp = Running_List->current->data;
                Running_List->current->data.flag = 0;
                changeCurrent(Running_List);
                struct process newCurrent = Running_List->current->data;
                if (temp.id != newCurrent.id)
                {

                    LogFinishedRR(temp, processCount, &runningTimeSum, &WTASum, &waitingTimeSum, TAArray, &TAArrayIndex);
                    // LogStartedRR(newCurrent);
                }
                else
                {
                    Running_List->current->data.flag = 1;
                }
            }
            struct msgbuff sendmsg;
            // Check to handle the last process in the list
            if (isEmpty(Running_List))
            {
                continue;
            }
            sendmsg.mtype = Running_List->current->data.pid;
            sendmsg.msg = 1;
            // Send the turn to the current process
            int send = msgsnd(SendQueueID, &sendmsg, sizeof(sendmsg.msg), IPC_NOWAIT);
            if (send == -1)
            {
                perror("Error in send message");
                exit(-1);
            }
            struct process currentProcess;
            getCurrent(Running_List, &currentProcess);
            if (currentProcess.flag == 0)
            {
                currentProcess.flag = 1;
                changeCurrentData(Running_List, currentProcess);
                LogStartedRR(currentProcess);
            }
            if (HasStartedArray[currentProcess.id] == 0)
            {
                HasStartedArray[currentProcess.id] = 1;
                currentProcess.starttime = clk;
                changeCurrentData(Running_List, currentProcess);
            }
            quantumCounter++;
        }
        // Waits till the next clock cycle
        while (clk == getClk())
        {
        };
    }
    FILE *perf;
    perf = fopen("scheduler.perf", "w");
    printf("%i", runningTimeSum);
    float CPUUtilization = (float)runningTimeSum / clk * 100;
    fprintf(perf, "CPU Utilization =  %.2f %% \n", CPUUtilization);
    float AVGWTA = (float)WTASum / (float)processCount;
    fprintf(perf, "Avg WTA =  %.2f  \n", AVGWTA);
    fprintf(perf, "Avg Waiting = %.2f \n", (float)waitingTimeSum / (float)processCount);
    double counter = 0.0f;
    for (int i = 0; i < processCount; i++)
    {
        counter += (TAArray[i] - AVGWTA) * (TAArray[i] - AVGWTA);
    }

    counter = counter / processCount;
    counter = sqrt(counter);
    fprintf(perf, "Std WTA = %.2f \n", counter);
    fclose(perf);
    destroyList(Running_List);
}
#endif