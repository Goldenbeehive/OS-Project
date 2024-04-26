#include "headers.h"
#define IGNORE_LENGTH 128
#define MAX_SIZE 1024
int numOfProcesses = 0;
struct process** processQueue = NULL;

void testerfunction(struct process* p){
    printf("%d %d %d %d %d",p->id,p->arrivaltime,p->runningtime,p->remainingtime,p->priority);
    printf("\n");
}

void skipLine(FILE* f){
    char ignore[IGNORE_LENGTH];
    fgets(ignore,IGNORE_LENGTH,f);
}
int getnoOfProcesses(FILE* f){
    int c;
    int count = 0;
    while ((c = fgetc(f)) != EOF) { if (c == '\n') { count++; } }
    fseek(f,0,SEEK_SET);
    return count;
}

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
    processQueue = (struct process**)malloc(numOfProcesses * sizeof(struct process));
    char line[MAX_SIZE]; 
    while (fgets(line, sizeof(line), f) != NULL){
        int ID,AT,RunningTime,PRI;//,MEMSISZE;
        sscanf(line,"%d %d %d %d", &ID,&AT,&RunningTime,&PRI);               //,&MEMSIZE);
        processQueue[i++] = initializeProcess(ID,AT,RunningTime,PRI);    //,MEMSIZE)
        testerfunction(processQueue[i-1]);
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
    SchedParam[2] = RR_Quantum == 0 ? "0" : sprintf("%d",RR_Quantum);
    SchedParam[3] = NULL;
    pid_t Clock = fork();
    if (Clock == 0){ execv("./clk.o",NULL); }
    initClk();
    pid_t Scheduler = fork();
    if (Scheduler == 0){ execv("./scheduler.o",SchedParam); }
    int msgid;
    while (i < numOfProcesses){
        if (getClk() == processQueue[i]->arrivaltime){
            msgsnd(msgid, processQueue[i], sizeof(struct process),IPC_NOWAIT);
            printf("Process %d sent to scheduler at time = %d\n",processQueue[i]->id,processQueue[i]->arrivaltime);
            i++;
        }
    }
    signal(SIGCHLD,clearResources);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    for (int i = 0 ; i < numOfProcesses ; i++){ free(processQueue[i]); }
    free(processQueue);
    destroyClk(true);
}
