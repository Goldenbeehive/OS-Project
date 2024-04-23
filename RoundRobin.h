#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include "headers.h"
#include "CircularQueue.h"
int ProcessFinished = 0;
void RoundRobin(int quantum, int processCount)
{
    // Create necessary variables and queues
    struct CircularQueue *Running_Queue = createQueue(processCount);
    int clk = getClk();
    int quantumCounter = 0;
    int remainingProcesses = processCount;
    int previousClk;
    struct process *currentProcess;
    // While there are still processes in the running queue
    while (remainingProcesses != 0)
    {
        clk = getClk();
        if (!isEmpty(Running_Queue))
        {
            if (quantumCounter > quantum)
            {
                quantumCounter = 0;
                if (!isEmpty(Running_Queue))
                {
                    currentProcess = getCurrent(Running_Queue);
                    kill(currentProcess->id, SIGSTOP);
                    changeCurrent(Running_Queue);
                }
            }
            currentProcess = getCurrent(Running_Queue);
            if (currentProcess != NULL)
            {
                kill(currentProcess->id, SIGCONT);
            }
            quantumCounter++;
        }
        while (getClk() == clk)
        {
            // Do nothing
        }
    }
}

#endif