#include "headers.h"
int isRunning = 0;

int main(int argc, char * argv[])
{
    if (argc != 2) {
        //printf("%i",argc);
        perror("Invalid number of arguments");
        return -1;
    }
    //Initialize Send queue to receive turn from scheduler
    key_t SendQueueKey;
    SendQueueKey= ftok("Sendman",'A');
    int SendQueueID = msgget(SendQueueKey, 0666 | IPC_CREAT);
    //printf("Process Send Queue ID: %d\n",SendQueueID);
    if (SendQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //Initialize Receive queue to send remaining time to scheduler
    key_t ReceiveQueueKey;
    ReceiveQueueKey= ftok("Receiveman",'A');
    int ReceiveQueueID = msgget(ReceiveQueueKey, 0666 | IPC_CREAT);
    //printf("Process Receive Queue ID: %d\n",ReceiveQueueID);
    if (ReceiveQueueID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    initClk();
    int remainingTime = atoi(argv[1]),currentClock=getClk();
    //printf("Initial Remaining Time = %d\n", remainingTime);
    while (remainingTime > 0){
        currentClock = getClk();
        struct msgbuff turn;
        int received=msgrcv(SendQueueID,&turn,sizeof(turn.msg),getpid(),!IPC_NOWAIT);
        if(received != -1){
            printf("Process with ID: %d is operating\n",getpid());
        }
        remainingTime--;
        //printf("My Remaining Time = %d\n", remainingTime);
        struct msgbuff rem;
        rem.mtype=getpid();
        rem.msg=remainingTime;
        int sent=msgsnd(ReceiveQueueID,&rem,sizeof(rem.msg),!IPC_NOWAIT);
        while(currentClock == getClk()){};
    }
    printf("Process Finished\n");
    destroyClk(false);
    return 0;
}
