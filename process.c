#include "headers.h"
int isRunning = 0;

void SIGUSR1_Handler(int signum){ isRunning = 0; 
signal(SIGUSR1,&SIGUSR1_Handler);
}
void SIGUSR2_Handler(int signum){ isRunning = 1; 
signal(SIGUSR2,&SIGUSR2_Handler );
}
int main(int argc, char * argv[])
{
    printf("Process Started\n");
    printf("PROCESSFILE_Process PID: %d\n",getpid());
    printf("%d\n",argc);
    if (argc != 2) {
        printf("%i",argc);
        perror("Invalid number of arguments");
        return -1;
    }
    printf("Valid number of arguments\n");
    signal(SIGUSR1,&SIGUSR1_Handler);
    signal(SIGUSR2,&SIGUSR2_Handler );
    initClk();
    int remainingTime = atoi(argv[1]), prevClock = getClk() ,currentClock;
    printf("Remaining Time = %d\n", remainingTime);
    while (remainingTime > 0){
        printf("Process is waiting, remaining time = %d\n", remainingTime);
        while (!isRunning){};
        currentClock = getClk();
        printf("Process is running, current clk = %d\n", currentClock);
        remainingTime--;
        isRunning = 0;
        while(currentClock == getClk()){};
    }
    printf("Process Finished\n");
    kill(getppid(),SIGUSR1);
    destroyClk(false);
    return 0;
}
