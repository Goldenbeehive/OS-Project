#include "headers.h"
#include "RoundRobin.h"
int numOfProcesses = 0;
struct process* processQueue = NULL;
int msgid;

void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    FILE* f = fopen("processes.txt","r");
    if (f == NULL){
        perror("Error opening file");
        return -1;
    }
    skipLine(f);
    numOfProcesses = getnoOfProcesses(f);
    skipLine(f);
    int SchedAlgo,RR_Quantum = 0,i = 0;
    printf("Enter the scheduling algorithm you want: (1 for Round Robin, 2 for xxxx, 3 for xxxx) ");
    scanf("%d", &SchedAlgo);
    if (SchedAlgo == 1){
        while (RR_Quantum <= 0){
            printf("Enter your Round Robin Quantum: ");
            scanf("%d",&RR_Quantum);
        }
    }
    processQueue = (struct process*)malloc(numOfProcesses * sizeof(struct process));
    char line[MAX_SIZE]; 
    while (fgets(line, sizeof(line), f) != NULL){
        int ID,AT,RunningTime,PRI;//,MEMSISZE;
        sscanf(line,"%d %d %d %d", &ID,&AT,&RunningTime,&PRI);               //,&MEMSIZE);
        struct process proc;
         proc.id = ID;
         proc.arrivaltime = AT;
         proc.runningtime = RunningTime;
         proc.priority = PRI;
         processQueue[i++] =  proc;
        //processQueue[i++] = initializeProcess(ID,AT,RunningTime,PRI);    //,MEMSIZE)
        testerfunction(&processQueue[i-1]);
    }
    fclose(f);
    i = 0;
    char* SchedParam[4];
    switch (SchedAlgo){
        case 1:
            SchedParam[0] = "1";
            break;
        case 2:
            SchedParam[0] = "2";
            break;
        case 3:
            SchedParam[0] = "3";
            break;
        default:
            perror("Invalid Scheduling Algorithm");
            return -1;
            break;
    }
    char numOfProc[10];
    sprintf(numOfProc,"%d",numOfProcesses);
    SchedParam[1] = numOfProc;
    char RRQuantum[10];
    if(RR_Quantum == 0){ SchedParam[2] = "0"; }
    else{sprintf(RRQuantum,"%d",RR_Quantum); SchedParam[2] = RRQuantum;}
    SchedParam[3] = NULL;
    pid_t Clock = fork();
    char* args[] = {"./clk.out",NULL};
    if (Clock == 0){ execv(args[0],args); }
    initClk();
    pid_t Scheduler = fork();
    if (Scheduler == 0){ execv("./scheduler.out",SchedParam); }
    key_t key = ftok("Funnyman", 'A');
    msgid = msgget(key, 0666 | IPC_CREAT);
    while (i < numOfProcesses){
        if (getClk() == processQueue[i].arrivaltime){
            struct msg message;
            message.proc = processQueue[i];
            msgsnd(msgid, &message, sizeof(message.proc),IPC_NOWAIT);
            printf("Process %d sent to scheduler at time = %d\n",processQueue[i].id,processQueue[i].arrivaltime);
            i++;
        }
    }
    
    signal(SIGCHLD,clearResources);
    signal(SIGINT,clearResources);
    waitpid(Scheduler,NULL,0);
    msgctl(msgid, IPC_RMID,NULL);
    destroyClk(true);
    return 0;
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    for (int i = 0 ; i < numOfProcesses ; i++){ free(processQueue[i]); }
    free(processQueue);
    msgctl(msgid, IPC_RMID,NULL);
    destroyClk(true);
}
