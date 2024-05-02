#include "MinHeap.h"
#include "headers.h"
/**
 * @brief Clears the contents of the log file named "scheduler.log"
 *
 * @return void
 */
void clearLogFileSRTN()
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
void LogStartedSRTN(struct process *proc)
{
    if (proc == NULL)
    {
        return;
    }
    int clock = getClk();
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "a");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }
    if (proc->remainingtime != proc->runningtime)
    {
        fprintf(filePointer, "At time %d, process %d Resumed. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
                clock, proc->id, proc->arrivaltime, proc->remainingtime, proc->runningtime, clock - proc->arrivaltime-proc->runningtime+proc->remainingtime);
    }
    else
    {
        fprintf(filePointer, "At time %d, process %d started. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
                clock, proc->id, proc->arrivaltime, proc->remainingtime, proc->runningtime, clock - proc->arrivaltime);
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
void LogFinishedSRTN(struct process *proc, int noOfProcesses)
{
    if (proc == NULL)
    {
        printf("NULL");
        return;
    }
    int clock = getClk();
    FILE *filePointer;
    filePointer = fopen("scheduler.log", "a");
    if (filePointer == NULL)
    {
        printf("Unable to open scheduler.log.\n");
        return;
    }
    if (proc->remainingtime == 0)
    {

        fprintf(filePointer, "At time %d, process %d Finished. Arr: %d, remain: %d,Total:%d, wait: %d. TA %d WTA %.2f\n",
                clock, proc->id, proc->arrivaltime, proc->remainingtime, proc->runningtime, clock - proc->arrivaltime - proc->runningtime, clock - proc->arrivaltime, ((float)clock - proc->arrivaltime) / (float)proc->runningtime);

    }
    else
    {
        fprintf(filePointer, "At time %d, process %d Stopped. Arr: %d, remain: %d,Total:%d, wait: %d.\n",
                clock, proc->id, proc->arrivaltime, proc->remainingtime, proc->runningtime, clock - proc->arrivaltime-proc->runningtime+proc->remainingtime);
    }
    fclose(filePointer);
}

/**
 * @brief  Receive a process from the Ready Queue
 *
 * @param minHeap  The MinHeap to insert the process in
 * @param ReadyQueueID  The ID of the Ready Queue
 * @return true
 * @return false
 */
bool ReceiveProcess(struct MinHeap *minHeap, int ReadyQueueID)
{
    struct process ArrivedProcess;
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
        insertSRTN(minHeap, ArrivedProcess);
        return true;
    }
    return false;
}

/**
 * @brief  Run the Shortest Remaining Time Next Algorithm
 *
 * @param noOfProcesses  The number of processes
 */
void SRTN(int noOfProcesses)
{
    printf("STRN Running\n");
    int nonIdle = 0;
    int sumTA = 0;
    int waiting = 0;
    clearLogFileSRTN();
    int remainingProcesses = noOfProcesses;
    struct MinHeap *minHeap = createMinHeap(noOfProcesses);
    int ReadyQueueID, SendQueueID, ReceiveQueueID;
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    struct process *keeper = NULL;
    while (remainingProcesses > 0)
    {
        int clk = getClk();
        struct process *currentProcess = NULL, tmp;

        printf("Current clock = %d\n", clk);

        while (ReceiveProcess(minHeap, ReadyQueueID))
            ;
        if (minHeap->heap_size > 0)
        {

            currentProcess = getMin_ptr(minHeap);
            if (keeper == NULL)
            {
                keeper = (struct process *)malloc(sizeof(struct process));
                *keeper = *currentProcess;
                LogStartedSRTN(currentProcess);
            }
            if (keeper != NULL && keeper->id != currentProcess->id)
            {
                LogFinishedSRTN(keeper, noOfProcesses);
                LogStartedSRTN(currentProcess);
                *keeper = *currentProcess;
            }

            struct msgbuff receivedmsg;
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1)
            {
                currentProcess->remainingtime = receivedmsg.msg;
            }

            if (currentProcess->remainingtime == 0)
            {
                printf("Process with ID: %d has finished\n", currentProcess->id);
                LogFinishedSRTN(currentProcess, noOfProcesses);
                struct process Terminated = extractMin(minHeap, 1);
                remainingProcesses--;
                wait(NULL);
                if (minHeap->heap_size != 0)
                {
                    currentProcess = getMin_ptr(minHeap);
                    LogStartedSRTN(currentProcess);
                    *keeper = *currentProcess;
                }
            }
            if (minHeap->heap_size == 0)
            {
                continue;
            }
            struct msgbuff sendmsg;
            sendmsg.mtype = currentProcess->pid;
            sendmsg.msg = 1;
            // Send the turn to the current process
            int send = msgsnd(SendQueueID, &sendmsg, sizeof(sendmsg.msg), IPC_NOWAIT);
            printf("Process %d with pid = %d is running\n", currentProcess->id, currentProcess->pid);
        }
        while (clk == getClk())
            ;
    }
    free(keeper);
    destroy(minHeap);
}