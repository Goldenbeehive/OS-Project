#include "headers.h"
#include "RoundRobin.h"
int numOfProcesses = 0;
struct process *processQueue = NULL;
int ReadyQueueID, SendQueueID, ReceiveQueueID, GUIID;
pid_t Scheduler ;
void clearResources(int);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    if (argc != 4)
    {
        perror("Invalid number of arguments");
        return -1;
    }
    // Read the file
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        perror("Error opening file");
        return -1;
    }
    skipLine(f);
    numOfProcesses = getnoOfProcesses(f);
    skipLine(f);
    int i = 0;

    // Initialize the process queue
    processQueue = (struct process *)malloc(numOfProcesses * sizeof(struct process));
    char line[MAX_SIZE];
    // Read the processes from the file
    while (fgets(line, sizeof(line), f) != NULL)
    {
        int ID, AT, RunningTime, PRI, MEMSIZE;
        sscanf(line, "%d %d %d %d %d", &ID, &AT, &RunningTime, &PRI, &MEMSIZE);
        processQueue[i++] = initializeProcess(ID, AT, RunningTime, PRI,MEMSIZE);
        testerfunction(&processQueue[i - 1]);
    }
    fclose(f);
    i = 0;
    char *SchedParam[4]; // Array to hold the parameters of the scheduler
    SchedParam[0] = argv[2];
    char numOfProc[10];
    sprintf(numOfProc, "%d", numOfProcesses);
    SchedParam[1] = numOfProc;
    SchedParam[2] = argv[3];
    SchedParam[3] = NULL;
    // Fork the clock process
    pid_t Clock = fork();
    char *args[] = {"./clk.out", NULL};
    if (Clock == 0)
    {
        execv(args[0], args);
    }
    initClk();
    initSync();
    *Synchro = 0;
    // Fork the scheduler process
    Scheduler = fork();
    if (Scheduler == 0)
    {
        execv("./scheduler.out", SchedParam);
    }
    key_t GUIKey;

    GUIKey = ftok("keys/Guiman", 'A');
    GUIID = msgget(GUIKey, 0666 | IPC_CREAT);
    if (GUIID == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    DefineKeys(&ReadyQueueID, &SendQueueID, &ReceiveQueueID);
    while (i < numOfProcesses)
    { // Send the processes to the scheduler according to their arrival time
        setSync(0);
        if (getClk() == processQueue[i].arrivaltime)
        {
            struct process temp = processQueue[i];
            struct process temp2 = processQueue[i];
            msgsnd(ReadyQueueID, &temp, sizeof(temp), IPC_NOWAIT); // Send the process to the scheduler
            msgsnd(GUIID, &temp2, sizeof(temp2), IPC_NOWAIT);
            // printf("Process %d sent to scheduler at time = %d\n",processQueue[i].id,processQueue[i].arrivaltime);
            i++;
        }
        else
        {
            setSync(1);
        }
    }
    setSync(1);
    // Wait for the scheduler to finish

    waitpid(Scheduler, NULL, 0);
    destroyClk(true);
    return 0;
}

/**
 * @brief  Clear the resources used by the process generator
 *
 * @param signum  The signal number
/*/
void clearResources(int signum)
{
    free(processQueue);
    destroySync(true);
    msgctl(GUIID, IPC_RMID, NULL);
    msgctl(ReadyQueueID, IPC_RMID, NULL);
    msgctl(SendQueueID, IPC_RMID, NULL);
    msgctl(ReceiveQueueID, IPC_RMID, NULL);
}
