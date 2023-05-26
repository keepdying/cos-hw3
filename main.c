#include <stdio.h>
#include <stdlib.h>

#define PROCESS_COUNT 5
#define RESOURCE_COUNT 5

int resources[RESOURCE_COUNT];

struct process {
    int allocations[RESOURCE_COUNT];
    int requests[RESOURCE_COUNT];
};

struct process processes[PROCESS_COUNT];

int max(int a, int b) {
    return a > b ? a : b;
}

void read_resources(){
    FILE *file = fopen("resources.txt", "r");
    if (file == NULL) {
        printf("Could not open file resources.txt\n");
        return;
    }
    for(int i = 0; i < RESOURCE_COUNT; i++) {
        fscanf(file, "%d", &resources[i]);
    }
    fclose(file);
}

void read_allocations(){
    FILE *file = fopen("allocations.txt", "r");
    if (file == NULL) {
        printf("Could not open file allocations.txt\n");
        return;
    }
    for(int i = 0; i < PROCESS_COUNT; i++) {
        for(int j = 0; j < RESOURCE_COUNT; j++) {
            fscanf(file, "%d", &processes[i].allocations[j]);
        }
    }
    fclose(file);
}

void read_requests(){
    FILE *file = fopen("requests.txt", "r");
    if (file == NULL) {
        printf("Could not open file requests.txt\n");
        return;
    }
    for(int i = 0; i < PROCESS_COUNT; i++) {
        for(int j = 0; j < RESOURCE_COUNT; j++) {
            fscanf(file, "%d", &processes[i].requests[j]);
        }
    }
    fclose(file);
}

void print_information(int process) {
    printf("Information for process: P%d:\n", process+1);
    printf("Allocated resources: ");
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        printf("R%d:%d ", i+1, processes[process].allocations[i]);
    }
    printf("\n");
    printf("Resource request: ");
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        printf("R%d:%d ", i+1, processes[process].requests[i]);
    }
    printf("\n");
    printf("Need Vector: ");
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        printf("R%d:%d ", i+1, max(processes[process].requests[i] - processes[process].allocations[i], 0));
    }
    printf("\n\n");
}

int can_grant_request(int process, int work[RESOURCE_COUNT]) {
    int need[RESOURCE_COUNT];

    // Calculate the need vector for the process
    for (int r = 0; r < RESOURCE_COUNT; r++) {
        // The need is the maximum resources the process could request minus what it has already been allocated
        need[r] = max(processes[process].requests[r] - processes[process].allocations[r], 0);
    }

    for (int r = 0; r < RESOURCE_COUNT; r++) {
        // If the process needs more resources than are available, return 0
        if (need[r] > work[r]) {
            return 0;
        }
    }

    // If the process can be run with the available resources, return 1
    return 1;
}



void run_bankers_algorithm() {
    int finish[PROCESS_COUNT] = {0};
    int work[RESOURCE_COUNT];
    int deadlock = 0;

    // Initialize work to total resources
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        work[i] = resources[i];
    }

    // Subtract allocated resources from work
    for (int i = 0; i < PROCESS_COUNT; i++){
        for (int j = 0; j < RESOURCE_COUNT; j++){
            work[j] -= processes[i].allocations[j];
        }
    }

    // While not all processes have finished
    while (1) {
        int found = 0;
        for (int p = 0; p < PROCESS_COUNT; p++) {
            // If this process has not finished
            if (!finish[p]) {
                int can_run = 1;
                for (int r = 0; r < RESOURCE_COUNT; r++) {
                    // Check if process can run with available resources
                    if (processes[p].requests[r] > work[r]) {
                        can_run = 0;
                        break;
                    }
                }

                if (can_run) {
                    // Run the process and release its resources
                    printf("Running process: P%d\n", p+1);
                    for (int r = 0; r < RESOURCE_COUNT; r++) {
                        work[r] += processes[p].allocations[r];
                    }
                    finish[p] = 1;
                    found = 1;
                }
            }
        }

        // If no process was able to run, there might be a deadlock
        if (!found) {
            for (int p = 0; p < PROCESS_COUNT; p++) {
                if (!finish[p]) {
                    printf("Process P%d cannot finish, possible deadlock.\n", p+1);
                    deadlock = 1;
                }
            }
            break;
        }
    }

    if (!deadlock) {
        printf("All processes finished successfully.\n");
    }

    printf("\n");
}


int main() {
    read_resources();
    read_allocations();
    read_requests();

    for (int i = 0; i < PROCESS_COUNT; i++){
        print_information(i);
    }

    run_bankers_algorithm();

    return 0;
}
