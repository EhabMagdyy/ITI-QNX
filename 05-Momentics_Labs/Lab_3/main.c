#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

int main(void) {
    pid_t pid;
    int status;

    char *argv[] = {"echo", "Hello from QNX 8!", NULL};

    printf("[Parent] PID: %d, TID: %d\n", getpid(), (int)pthread_self());
    printf("--------------------------------------------------\n");

    /*
     * posix_spawnp:
     * - &pid: stores the PID of the new process
     * - argv[0]: the command to run
     * - NULL (file_actions): no special file descriptor mapping
     * - NULL (attrp): use default process attributes
     * - argv: the argument list
     * - environ: use the current environment variables
     */
    status = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);

    if(status == 0){
        printf("[Parent] Successfully spawned child with PID: %d\n", pid);

        // Wait for the child to complete
        if(waitpid(pid, &status, 0) != -1){
            printf("[Parent] Child (PID %d) finished.\n", pid);
        }
        else {
            perror("waitpid failed");
        }
    }
    else{
        printf("posix_spawn failed with error code: %d\n", status);
        return EXIT_FAILURE;
    }

    printf("--------------------------------------------------\n");

    return EXIT_SUCCESS;
}
