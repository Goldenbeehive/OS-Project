#include "headers.h"
int isRunning = 0;

void SIGCONT_Handler(int signum){ isRunning = 1; }
void SIGSTSP_Handler(int signum){ isRunning = 0; }
int main(int argc, char * argv[])
{
    printf("Process Started\n");
    if (argc != 1) {
        perror("Invalid number of arguments");
        return -1;
    }
    signal(SIGSTOP,&SIGSTSP_Handler);
    signal(SIGCONT,&SIGCONT_Handler);
    initClk();
    int remainingTime = atoi(argv[1]), prevClock = getClk() ,currentClock;
    while (remainingTime > 0){
        while (!isRunning);
        printf("Process is running, current clk = %d\n", currentClock);
        currentClock = getClk();
        remainingTime--;
        while(currentClock == getClk());
    }
    kill(getppid(),SIGUSR1);
    destroyClk(false);
    return 0;
}
