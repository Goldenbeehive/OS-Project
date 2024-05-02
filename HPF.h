#include "MinHeap.h"
#include "headers.h"
#include "time.h"
/**
 * @brief Clears the contents of the log file named "scheduler.log"
 *
 * @return void
 */
void clearLogFile()
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
void LogStarted(struct process proc)
{
    int clock = getClk();
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "a");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }
    fprintf(filePointer, "At time %d, process %d started. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
            clock, proc.id, proc.arrivaltime, proc.remainingtime, proc.runningtime, clock - proc.arrivaltime);
    fclose(filePointer);
}
/**
 * @brief Logs the finish time, turnaround time, and details of a process to the "scheduler.log" file
 *
 * @param proc The process structure containing details of the finished process
 * @param noOfProcesses The total number of processes
 * @return void
 */
void LogFinished(struct process proc, int noOfProcesses, int *runningTimeSum, float *WTASum, int *waitingTimeSum, float TAArray[], int *TAArrayIndex)
{
    int clock = getClk();
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "a");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }
    fprintf(filePointer, "At time %d, process %d finished. Arr: %d, remain: %d,Total:%d, wait: %d. TA %d WTA %.2f\n",
            clock, proc.id, proc.arrivaltime, proc.remainingtime, proc.runningtime, clock - proc.arrivaltime - proc.runningtime, clock - proc.arrivaltime, ((float)clock - proc.arrivaltime) / (float)proc.runningtime);
    *runningTimeSum += proc.runningtime;
    *WTASum += ((float)clock - proc.arrivaltime) / (float)proc.runningtime;
    *waitingTimeSum += clock - proc.arrivaltime - proc.runningtime;
    TAArray[*TAArrayIndex] = ((float)clock - proc.arrivaltime) / (float)proc.runningtime;
    *TAArrayIndex = *TAArrayIndex + 1;
    fclose(filePointer);
}

/**
 * @brief Inserts the arrived process in the MinHeap
 *
 * @param minHeap  The MinHeap to insert the process in
 * @param ReadyQueueID  The ID of the Ready Queue
 * @return true
 * @return false
 */
bool ReceiveProcessHPF(struct MinHeap *minHeap, int ReadyQueueID)
{
    struct process ArrivedProcess;
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 1;  
    nanosleep(&req, NULL);
    int received = msgrcv(ReadyQueueID, &ArrivedProcess, sizeof(ArrivedProcess), 0, IPC_NOWAIT);
    if (received != -1)
    {
        printf("Process with id %d has arrived\n", ArrivedProcess.id);
        char RunningTimeStr[12];
        sprintf(RunningTimeStr, "%d", ArrivedProcess.runningtime);
        char *args[3] = {"./process.out", RunningTimeStr, NULL};
        pid_t pid = fork();
        if (pid == 0)
        {
            execv(args[0], args);
        }
        ArrivedProcess.pid = pid;
        insertHPF(minHeap, ArrivedProcess);
        return true;
    }
    return false;
}

/**
 * @brief   The Highest Priority First Scheduling Algorithm
 *
 * @param noOfProcesses  The number of processes to be scheduled
 */
void HPF(int noOfProcesses)
{
    printf("HPF Running");
    clearLogFile();
    float TAArray[noOfProcesses];
    int TAArrayIndex = 0;
    int runningTimeSum = 0;
    float WTASum = 0.0f;
    int waitingTimeSum = 0;
    int remainingProcesses = noOfProcesses;
    struct MinHeap *minHeap = createMinHeap(noOfProcesses);
    int ReadyQueueID, SendQueueID, ReceiveQueueID;
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    bool firstarrived = true;
    struct process currentProcess;
    int clk = 0;
    while (remainingProcesses > 0)
    {
        clk = getClk();
        printf("Current clock = %d\n", clk);
        while (ReceiveProcessHPF(minHeap, ReadyQueueID))
            ;
        if (minHeap->heap_size > 0)
        {
            if (firstarrived)
            {
                currentProcess = getMin(minHeap);
                firstarrived = false;
                LogStarted(currentProcess);
            }
            struct msgbuff receivedmsg;
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 1; 
            nanosleep(&req, NULL);
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1)
            {
                currentProcess.remainingtime = receivedmsg.msg;
            }
            if (currentProcess.remainingtime == 0)
            {
                printf("Process with ID: %d has finished\n", currentProcess.id);
                LogFinished(currentProcess, noOfProcesses, &runningTimeSum, &WTASum, &waitingTimeSum, TAArray, &TAArrayIndex);
                Remove(minHeap, currentProcess);
                struct process Terminated = currentProcess;
                remainingProcesses--;
                wait(NULL);
                if (minHeap->heap_size != 0)
                {
                    currentProcess = getMin(minHeap);
                    LogStarted(currentProcess);
                }
            }
            if (minHeap->heap_size == 0)
            {
                continue;
            }
            struct msgbuff sendmsg;
            sendmsg.mtype = currentProcess.pid;
            sendmsg.msg = 1;
            // Send the turn to the current process
            int send = msgsnd(SendQueueID, &sendmsg, sizeof(sendmsg.msg), IPC_NOWAIT);

            printf("Process %d with pid = %d is running\n", currentProcess.id, currentProcess.pid);
        }
        while (clk == getClk())
            ;
    }
    FILE *perf;
    perf = fopen("scheduler.perf", "w");
    printf("%i", runningTimeSum);
    float CPUUtilization = (float)runningTimeSum / clk * 100;
    fprintf(perf, "CPU Utilization =  %.2f %% \n", CPUUtilization);
    float AVGWTA = (float)WTASum / (float)noOfProcesses;
    fprintf(perf, "Avg WTA =  %.2f  \n", AVGWTA);
    fprintf(perf, "Avg Waiting = %.2f \n", (float)waitingTimeSum / (float)noOfProcesses);
    double counter = 0.0f;
    for (int i = 0; i < noOfProcesses; i++)
    {
        counter += (TAArray[i] - AVGWTA) * (TAArray[i] - AVGWTA);
    }

    counter = counter / noOfProcesses;
    counter = sqrt(counter);
    fprintf(perf, "Std WTA = %.2f \n", counter);
    fclose(perf);
    destroy(minHeap);
}
