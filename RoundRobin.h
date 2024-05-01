#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularList.h"
void RoundRobin(int quantum, int processCount)
{
    printf("Round Robin Scheduler\n");
    int HasStartedArray[processCount+1];
    for(int i=0;i<=processCount;i++)
    {
        HasStartedArray[i]=0;
    }
    //Initialize Ready queue to receive processes from process generator
    int ReadyQueueID,SendQueueID,ReceiveQueueID;
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    int quantumCounter = 0;
    int remainingProcesses = processCount;
    struct CircularList *Running_List = createCircularList();
    //Main processing loop, keeps running until all processes are finished
    while(remainingProcesses>0)
    {
        int clk = getClk();
        //Prints the current cycle
        printf("Current Clk: %d\n",clk);
        // Check if there are any new processes
        bool StillArriving = true;
        while(StillArriving)
        {
            struct process rec;
            //Checks for processes arriving from the process generator
            int received = msgrcv(ReadyQueueID, &rec, sizeof(rec), 0, IPC_NOWAIT);
            if (received != -1)
            {
                //If process was received, add it to the running list and Fork it to start its execution
                printf("Process with ID: %d has arrived\n",rec.id);
                pid_t pid = fork();
                    if (pid == -1) {
                        // Fork failed
                        perror("fork");
                    }
                    if (pid == 0) {
                        //This is the code executed by the child, which will be replaced by the process
                        char RunningTimeStr[20]; // Assuming 20 characters is enough for the string representation of currentProcess.runningtime
                        sprintf(RunningTimeStr, "%d", rec.runningtime);
                        //printf("I'm child, my time is %s\n", RunningTimeStr);
                        char* args[] = {"./process.out", RunningTimeStr, NULL, NULL}; // NULL terminator required for the args array
                        execv(args[0], args);
                        // If execv returns, it means there was an error
                        perror("execv");
                        exit(EXIT_FAILURE); // Exit child process with failure
                    }
                rec.pid = pid; // Assign the PID of the child process to the process struct   
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
            //Checks for remaining time from the process
            struct msgbuff receivedmsg;
            int received = msgrcv(ReceiveQueueID, &receivedmsg, sizeof(receivedmsg.msg), 0, IPC_NOWAIT);
            if (received != -1)
            {
                //printf("Received remaining time %d from process with PID: %ld\n",receivedmsg.msg,receivedmsg.mtype);
                struct process p;
                getCurrent(Running_List,&p);
                //Updates the remaining time of the process
                p.remainingtime = receivedmsg.msg;
                changeCurrentData(Running_List,p);
                if(p.remainingtime == 0)
                {
                    //If the process has finished, remove it from the running list
                    printf("Process with ID: %d has finished\n",p.id);
                    struct process Terminated;
                    removeCurrent(Running_List,&Terminated);
                    displayList(Running_List);
                    remainingProcesses--;
                    quantumCounter = 0;
                    wait(NULL);
                }
            }
            if(quantumCounter == quantum)
            {
                //If the quantum has finished, change the current process
                //printf("Quantum has finished\n");
                quantumCounter = 0;
                changeCurrent(Running_List);
            }
            struct msgbuff sendmsg;
            //Check to handle the last process in the list
            if(isEmpty(Running_List))
            {
                continue;
            }
            sendmsg.mtype = Running_List->current->data.pid;
            sendmsg.msg = 1;
            //Send the turn to the current process
            int send = msgsnd(SendQueueID, &sendmsg, sizeof(sendmsg.msg), IPC_NOWAIT);
            if (send == -1)
            {
                perror("Error in send message");
                exit(-1);
            }
            quantumCounter++;
        }
        //Waits till the next clock cycle
        while(clk == getClk()){};
    }
    destroyList(Running_List);
}
#endif