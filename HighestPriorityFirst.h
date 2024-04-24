#ifndef HPF_H
#define HPF_H
#include "minHeap.h"
#include "headers.h"
void HighestPriorityFirst(int processCount){
int clk = getClk(); 
int remainingProcesses =processCount;
struct process *currentProcess;
while(remainingProcesses !=0) {
    currentProcess = getCurrent();
    kill(currentProcess->id,SIGCONT);
    clk = getClk();
    while(currentProcess->remainingtime > clk-getClk());
    remainingProcesses--;
}
}
#endif
