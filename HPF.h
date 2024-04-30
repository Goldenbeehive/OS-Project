#include "MinHeap.h"
#include "headers.h"

/**
 * @brief Inserts the arrived process in the MinHeap
 * 
 * @param minHeap  The MinHeap to insert the process in
 * @param ReadyQueueID  The ID of the Ready Queue
 * @return true 
 * @return false 
 */
bool ReceiveProcessHPF(struct MinHeap* minHeap,int ReadyQueueID){
    struct process ArrivedProcess;
    int received = msgrcv(ReadyQueueID, &ArrivedProcess, sizeof(ArrivedProcess), 0, IPC_NOWAIT);
    if(received!=-1){ 
        printf("Process with id %d has arrived\n", ArrivedProcess.id);
        char RunningTimeStr[12];
        sprintf(RunningTimeStr, "%d", ArrivedProcess.runningtime);
        char* args[3] = {"./process.out",RunningTimeStr,NULL};
        pid_t pid = fork();
        if (pid == 0){ execv(args[0], args); }
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
void HPF(int noOfProcesses){
    int remainingProcesses = noOfProcesses;
    struct MinHeap* minHeap = createMinHeap(noOfProcesses);
    int ReadyQueueID, SendQueueID, ReceiveQueueID;
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    bool firstarrived = true;
    struct process currentProcess;
    while (remainingProcesses > 0){
        int clk = getClk();
        printf("Current clock = %d\n",clk);
        while (ReceiveProcessHPF(minHeap,ReadyQueueID));
        if (minHeap->heap_size > 0){
            if (firstarrived){ currentProcess = getMin(minHeap); firstarrived = false; }
            struct msgbuff receivedmsg;
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1){ 
                currentProcess.remainingtime = receivedmsg.msg;
            }
            if (currentProcess.remainingtime == 0){
                printf("Process with ID: %d has finished\n",currentProcess.id);
                Remove(minHeap,currentProcess);
                struct process Terminated = currentProcess;
                remainingProcesses--;
                wait(NULL);
                if (minHeap->heap_size != 0){ currentProcess = getMin(minHeap); }
            }
            if (minHeap->heap_size == 0){ continue; }
            struct msgbuff sendmsg;
            sendmsg.mtype = currentProcess.pid;
            sendmsg.msg = 1;
            //Send the turn to the current process
            int send = msgsnd(SendQueueID, &sendmsg, sizeof(sendmsg.msg), IPC_NOWAIT);
            printf("Process %d with pid = %d is running\n", currentProcess.id,currentProcess.pid);
        }
        while (clk == getClk());
    }
    destroy(minHeap);
}