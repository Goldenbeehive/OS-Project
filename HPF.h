#include "MinHeap.h"
#include "headers.h"

// int ProcessFinished = false;

// void DefineKeys(int* ReadyQueueID, int* SendQueueID, int* ReceiveQueueID){
//     key_t ReadyQueueKey;
//     ReadyQueueKey= ftok("Funnyman",'A');
//     *ReadyQueueID = msgget(ReadyQueueKey, 0666 | IPC_CREAT);
//     if (*ReadyQueueID == -1)
//     {
//         perror("Error in create message queue");
//         exit(-1);
//     }
//     //Initialize Send queue to send turn to process
//     key_t SendQueueKey;
//     SendQueueKey= ftok("Sendman",'A');
//     *SendQueueID = msgget(SendQueueKey, 0666 | IPC_CREAT);
//     if (*SendQueueID == -1)
//     {
//         perror("Error in create message queue");
//         exit(-1);
//     }
//     //Initialize Receive queue to receive remaining time from process
//     key_t ReceiveQueueKey;
//     ReceiveQueueKey= ftok("Receiveman",'A');
//     *ReceiveQueueID = msgget(ReceiveQueueKey, 0666 | IPC_CREAT);
//     if (*ReceiveQueueID == -1)
//     {
//         perror("Error in create message queue");
//         exit(-1);
//     }
// }

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
        insertKeyHPF(minHeap, ArrivedProcess);
        return true;
    }
    return false;
}

void HPF(int noOfProcesses){
    int remainingProcesses = noOfProcesses;
    struct MinHeap* minHeap = createMinHeap(noOfProcesses);
    int ReadyQueueID, SendQueueID, ReceiveQueueID;
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    while (remainingProcesses > 0){
        int clk = getClk();
        struct process currentProcess;
        printf("Current clock = %d\n",clk);
        while (ReceiveProcessHPF(minHeap,ReadyQueueID));
        if (minHeap->heap_size > 0){
            if (remainingProcesses == noOfProcesses){ currentProcess = getMin(minHeap); }
            struct msgbuff receivedmsg;
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1){ 
                minHeap->harr[0].remainingtime = receivedmsg.msg;
                currentProcess.remainingtime = receivedmsg.msg;
            }
            if (currentProcess.remainingtime == 0){
                printf("Process with ID: %d has finished\n",currentProcess.id);
                struct process Terminated = extractMin(minHeap,0);
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