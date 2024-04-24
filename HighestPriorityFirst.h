#ifndef HPF_H
#define HPF_H
#include "minHeap.h"
#include "headers.h"
int waitforproc = 1;
void handle_sigchild(int sig)
{
    waitforproc = 0;
}
void HighestPriorityFirst(int processCount)
{
    signal(SIGCHLD, handle_sigchild);
    int clk = getClk();
    int remainingProcesses = processCount;
    struct process *currentProcess;

    while (remainingProcesses != 0)
    {
        currentProcess = getCurrent();
        kill(currentProcess->id, SIGCONT);
        clk = getClk();
        while (waitforproc)
            ;
        waitforproc = 1;
        remainingProcesses--;
    }
}
#endif
