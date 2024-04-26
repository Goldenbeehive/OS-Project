#include"headers.h"


int main() {
    printf("Forking Process\n");
   
    printf("Process Forked\n");
    pid_t pid = fork();
    char* args[] = {"./process.out","5",NULL};
    if (pid == 0){ execv(args[0],args); }
    
    printf("Process Forked with PID: %d\n",pid);
    wait(pid);
}
