#include "headers.h"
#include "CircularList.h"
int msgid,SendQueueID,ReceiveQueueID;
void RoundRobin(int quantum, int processCount)
{
    printf("Round Robin Scheduler\n");
    int HasStartedArray[processCount+1];
    for(int i=0;i<=processCount;i++)
    {
        HasStartedArray[i]=0;
    }
    //Initialize Ready queue to receive processes from process generator
    key_t ReadyQueueKey;
    ReadyQueueKey= ftok("Funnyman",'A');
    int ReadyQueueID = msgget(ReadyQueueKey, 0666 | IPC_CREAT);
    printf("Ready Queue ID: %d\n",ReadyQueueID);
    if (ReadyQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //Initialize Send queue to send turn to process
    key_t SendQueueKey;
    SendQueueKey= ftok("Sendman",'A');
    int SendQueueID = msgget(SendQueueKey, 0666 | IPC_CREAT);
    printf("Send Queue ID: %d\n",SendQueueID);
    if (SendQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //Initialize Receive queue to receive remaining time from process
    key_t ReceiveQueueKey;
    ReceiveQueueKey= ftok("Receiveman",'A');
    int ReceiveQueueID = msgget(ReceiveQueueKey, 0666 | IPC_CREAT);
    printf("Ready Queue ID: %d\n",ReceiveQueueID);
    if (ReceiveQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }

    struct process ArrivedProcess;
    int quantumCounter = 0;
    int remainingProcesses = processCount;
    struct CircularList *Running_List = createCircularList();

    while(remainingProcesses>0)
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
                printf("Process with ID: %d has arrived\n",rec.id);
                pid_t pid = fork();
                    if (pid == -1) {
                        // Fork failed
                        perror("fork");
                    }
                    if (pid == 0) {
                        // Child process
                        char RunningTimeStr[20]; // Assuming 20 characters is enough for the string representation of currentProcess.runningtime
                        sprintf(RunningTimeStr, "%d", rec.runningtime);
                        printf("I'm child, my time is %s\n", RunningTimeStr);
                        char* args[] = {"./process.out", RunningTimeStr, NULL, NULL}; // NULL terminator required for the args array
                        execv(args[0], args);
                        // If execv returns, it means there was an error
                        perror("execv");
                        exit(EXIT_FAILURE); // Exit child process with failure
                    }
                rec.pid = pid;    
                insertAtEnd(Running_List,rec);
                displayList(Running_List);
            }
            else
            {
                StillArriving = false;
            }
        }
        if(!isEmpty(Running_List))
        {
            struct msgbuff receivedmsg;
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1)
            {
                printf("Received remaining time %d from process with PID: %ld\n",receivedmsg.msg,receivedmsg.mtype);
                struct process p;
                getCurrent(Running_List,&p);
                p.remainingtime = receivedmsg.msg;
                changeCurrentData(Running_List,p);
                if(p.remainingtime == 0)
                {
                    printf("Process with ID: %d has finished\n",p.id);
                    struct process Terminated;
                    removeCurrent(Running_List,&Terminated);
                    remainingProcesses--;
                    quantumCounter = 0;
                    wait(NULL);
                }
            }
            if(quantumCounter == quantum)
            {
                printf("Quantum has finished\n");
                quantumCounter = 0;
                changeCurrent(Running_List);
            }
            struct msgbuff sendmsg;
            if(isEmpty(Running_List))
            {
                continue;
            }
            sendmsg.mtype = Running_List->current->data.pid;
            sendmsg.msg = 1;
            int send = msgsnd(SendQueueID, &sendmsg, sizeof(sendmsg.msg), IPC_NOWAIT);
            if (send == -1)
            {
                perror("Error in send message");
                exit(-1);
            }
            quantumCounter++;
        }
        while(clk == getClk()){};
    }
}
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
    msgctl(SendQueueID, IPC_RMID,NULL);
    msgctl(ReceiveQueueID, IPC_RMID,NULL);
    destroyClk(true);
    return 0;
}
