#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void) {
    const char *commands[4] = {"echo $0", "df -h", "pidin", "pidin info"};

    printf("--- QNX 8 System Command Test ---\n\n");

    for(int cmdIdx = 0; cmdIdx < 4; cmdIdx++) {
        printf("Executing: [%s]\n", commands[cmdIdx]);
        printf("---------------------------------\n");

        // Execute the command
        int ret = system(commands[cmdIdx]);

        if(ret == -1)
            perror("Error: system() failed to execute");

        printf("---------------------------------\n\n");
    }

    return EXIT_SUCCESS;
}
