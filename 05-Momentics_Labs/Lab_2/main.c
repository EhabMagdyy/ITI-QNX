#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int main(void) {
    pid_t pid;

    // In QNX, the main thread of a process is always TID 1.
    pthread_t tid = pthread_self();

    printf("[Parent] PID: %d, TID: %d\n", getpid(), (int)tid);
    printf("--------------------------------------------------\n");

    pid = fork();

    if(pid < 0) {
        perror("fork failed");
        return EXIT_FAILURE;
    }
    else if(pid == 0) { // Child process
        pthread_t child_tid = pthread_self();

        printf("[Child] PID: %d, TID: %d, Parent PID: %d\n", getpid(), (int)child_tid, getppid());

        printf("\nExecuting ls -l ...\n");
        // Execute ls (Doesn't require full path)
        execlp("ls", "ls", "-l", NULL);
        // Requires the full path
        // execl("/bin/ls", "ls", "-l", NULL);

        // If exec* returns, it means it failed
        perror("execlp failed");
        exit(EXIT_FAILURE);
    }
    else {	// Parent process
        printf("[Parent] Created child with PID: %d\n", pid);

        int status;
        waitpid(pid, &status, 0);

        printf("--------------------------------------------------\n");
        printf("[Parent] Child has exited. Parent process cleaning up and exiting.\n");
    }

    return EXIT_SUCCESS;
}
