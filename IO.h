#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "headers.h"
#include <math.h>
struct message {
    long mtype;
    struct process data;
};

int main(int argc, char *argv[]) {
 
    
    int threshold = atoi(argv[1]);

 
    double total_cpu_util = 0.0;
    double total_wta = 0.0;
    double total_waiting = 0.0;
    double total_turnaround = 0.0;
    double total_wta_squared = 0.0;
    int num_processes = 0;

    
    key_t key = ftok("deadproc", 'a');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

 
    struct message msg;
    while (num_processes < threshold) {
        if (msgrcv(msgid, &msg, sizeof(struct process), 0, 0) == -1) {
            perror("msgrcv");
            return 1;
        }
        struct process p = msg.data;
        
        total_cpu_util += ((double)(p.runningtime - p.remainingtime) / p.runningtime);
        total_wta += p.turnaroundtime;
        total_waiting += p.waittime;
        total_turnaround += p.turnaroundtime;
        total_wta_squared += p.turnaroundtime * p.turnaroundtime;
        num_processes++;

      
        if (num_processes == threshold) {
        
            double avg_cpu_util = total_cpu_util / num_processes;
            double avg_wta = total_wta / num_processes;
            double avg_waiting = total_waiting / num_processes;
            double avg_turnaround = total_turnaround / num_processes;
            double std_wta = sqrt((total_wta_squared / num_processes) - (avg_wta * avg_wta));

           
            FILE *file = fopen("statistics.txt", "w");
            if (file == NULL) {
                perror("fopen");
                return 1;
            }
            fprintf(file, "Average CPU Utilization: %.2f\n", avg_cpu_util);
            fprintf(file, "Average Waiting Time: %.2f\n", avg_waiting);
            fprintf(file, "Average Turnaround Time: %.2f\n", avg_turnaround);
            fprintf(file, "Average Weighted Turnaround Time: %.2f\n", avg_wta);
            fprintf(file, "Standard Deviation of Weighted Turnaround Time: %.2f\n", std_wta);
            fclose(file);
            break;
        }
    }

     
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return 1;
    }

    return 0;
}
