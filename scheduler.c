#include "headers.h"
#include "RoundRobin.h"
#include "SRTN.h"

int main(int argc, char *argv[])
{

    printf("Scheduler\n");
    if (argc != 3)
    {
        perror("Invalid number of arguments");
        return -1;
    }
    printf("Number of arguments: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("Argument %d: %s\n", i, argv[i]);
    }
    initClk();
    printf("Clock initialized in scheduler\n");
    // TODO implement the scheduler :)
    int quantum = atoi(argv[2]);
    printf("Quantum: %d\n", quantum);
    int processCount = atoi(argv[1]);
    printf("Process Count: %d\n", processCount);
    int SelectedAlgorithm = atoi(argv[0]);
    printf("Selected Algorithm: %d\n", SelectedAlgorithm);
    if (SelectedAlgorithm == 1)
    {
        RoundRobin(quantum, processCount);
    }
    else if (SelectedAlgorithm == 2)
    {
        SRTN(processCount);
    }
    else if (SelectedAlgorithm == 3)
    {
        // TODO implement the other algorithms here
    }
    else
    {
        printf("Invalid Algorithm\n");
    }

    // upon termination release the clock resources.
    printf("SCHEDULER DONE\n");
    destroyClk(true);
    kill(getppid(), SIGUSR1);
}