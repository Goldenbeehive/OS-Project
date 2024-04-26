#include "headers.h"
int isRunning = 0;

int SIGCONT_Handler(int signum){ isRunning = 1; }

int main(int agrc, char * argv[])
{
    if (argc != 2) {
        perror("Invalid number of arguments");
        return -1;
    }
    signal(SIGCONT, SIGCONT_Handler);
    initClk();
    int remainingtime = atoi(argv[1]), prevClock = getclk() ,currentClock;
    while (remainingTime > 0){
        while (!isRunning);
        currentClock = getclk();
        if (currentClock > prevClock) {
            prevClock = currentClock;
            remainingTime--;
        }
    }
    destroyClk(false);
    return 0;
}
